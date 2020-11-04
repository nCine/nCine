#include "return_macros.h"
#include "TextureLoaderDds.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderDds::TextureLoaderDds(nctl::UniquePtr<IFile> fileHandle)
    : ITextureLoader(nctl::move(fileHandle))
{
	DdsHeader header;

	fileHandle_->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);
	RETURN_ASSERT_MSG_X(fileHandle_->isOpened(), "File \"%s\" cannot be opened", fileHandle_->filename());
	const bool headerRead = readHeader(header);
	RETURN_ASSERT_MSG(headerRead, "DDS header cannot be read");
	const bool formatParsed = parseFormat(header);
	RETURN_ASSERT_MSG(formatParsed, "DDS format cannot be parsed");

	hasLoaded_ = true;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool TextureLoaderDds::readHeader(DdsHeader &header)
{
	// DDS header is 128 bytes long
	fileHandle_->read(&header, 128);

	// Checking for the header presence
	if (IFile::int32FromLE(header.dwMagic) == 0x20534444) // "DDS "
	{
		headerSize_ = 128;
		width_ = IFile::int32FromLE(header.dwWidth);
		height_ = IFile::int32FromLE(header.dwHeight);
		mipMapCount_ = IFile::int32FromLE(header.dwMipMapCount);

		if (mipMapCount_ == 0)
			mipMapCount_ = 1;
	}
	else
		RETURNF_MSG("Not a DDS file");

	return true;
}

bool TextureLoaderDds::parseFormat(const DdsHeader &header)
{
	GLenum internalFormat = GL_RGB; // to suppress uninitialized variable warning

	const uint32_t flags = IFile::int32FromLE(header.ddspf.dwFlags);

	// Texture contains compressed RGB data, dwFourCC contains valid data
	if (flags & DDPF_FOURCC)
	{
		const uint32_t fourCC = IFile::int32FromLE(header.ddspf.dwFourCC);

		const char *fourCCchars = reinterpret_cast<const char *>(&fourCC);
		LOGI_X("FourCC: \"%c%c%c%c\" (0x%x)", fourCCchars[0], fourCCchars[1], fourCCchars[2], fourCCchars[3], fourCC);

		// Parsing the FourCC format
		switch (fourCC)
		{
#ifndef __ANDROID__
			case DDS_DXT1:
				internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				break;
			case DDS_DXT3:
				internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				break;
			case DDS_DXT5:
				internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;
#else
			case DDS_ETC1:
				internalFormat = GL_ETC1_RGB8_OES;
				break;
			case DDS_ATC:
				internalFormat = GL_ATC_RGB_AMD;
				break;
			case DDS_ATCA:
				internalFormat = GL_ATC_RGBA_EXPLICIT_ALPHA_AMD;
				break;
			case DDS_ATCI:
				internalFormat = GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD;
				break;
#endif
			default:
				RETURNF_MSG_X("Unsupported FourCC compression code: %u", fourCC);
				break;
		}

		loadPixels(internalFormat);
	}
	// Texture contains uncompressed data
	else
	{
		GLenum type = GL_UNSIGNED_BYTE;

		const uint32_t bitCount = IFile::int32FromLE(header.ddspf.dwRGBBitCount);
		const uint32_t redMask = IFile::int32FromLE(header.ddspf.dwRBitMask);
		const uint32_t greenMask = IFile::int32FromLE(header.ddspf.dwGBitMask);
		const uint32_t blueMask = IFile::int32FromLE(header.ddspf.dwBBitMask);
		const uint32_t alphaMask = IFile::int32FromLE(header.ddspf.dwABitMask);

		LOGI_X("Pixel masks (%ubit): R:0x%x G:0x%x B:0x%x A:0x%x", bitCount, redMask, greenMask, blueMask, alphaMask);

		// Texture contains uncompressed RGB data
		// dwRGBBitCount and the RGB masks (dwRBitMask, dwRBitMask, dwRBitMask) contain valid data
		if (flags & DDPF_RGB || flags & (DDPF_RGB | DDPF_ALPHAPIXELS))
		{
			if ((redMask == 0x00FF0000 && greenMask == 0x0000FF00 && blueMask == 0x000000FF && alphaMask == 0x0) ||
			    (blueMask == 0x00FF0000 && greenMask == 0x0000FF00 && redMask == 0x000000FF && alphaMask == 0x0)) // 888
			{
				internalFormat = GL_RGB8;
			}
			else if ((alphaMask == 0xFF000000 && redMask == 0x00FF0000 && greenMask == 0x0000FF00 && blueMask == 0x000000FF) ||
			         (alphaMask == 0xFF000000 && blueMask == 0x00FF0000 && greenMask == 0x0000FF00 && redMask == 0x000000FF)) // 8888
			{
				internalFormat = GL_RGBA8;
			}
#if 0
			// 16 bits uncompressed DDS data is not compatbile with OpenGL color channels order
			else if (redMask == 0xF800 && greenMask == 0x07E0 && blueMask == 0x001F) // 565
			{
				internalFormat = GL_RGB565;
				type = GL_UNSIGNED_SHORT_5_6_5;
			}
			else if (alphaMask == 0x8000 && redMask == 0x7C00 && greenMask == 0x03E0 && blueMask == 0x001F) // 5551
			{
				internalFormat = GL_RGB5_A1;
				type = GL_UNSIGNED_SHORT_5_5_5_1;
			}
			else if (alphaMask == 0xF000 && redMask == 0x0F00 && greenMask == 0x00F0 && blueMask == 0x000F) // 4444
			{
				internalFormat = GL_RGBA4;
				type = GL_UNSIGNED_SHORT_4_4_4_4;
			}
#endif
			else
				RETURNF_MSG("Unsupported DDPF_RGB pixel format");
		}
		// Used in some older DDS files for single channel color uncompressed data
		// dwRGBBitCount contains the luminance channel bit count; dwRBitMask contains the channel mask
		// Can be combined with DDPF_ALPHAPIXELS for a two channel DDS file
		else if (flags & (DDPF_LUMINANCE | DDPF_ALPHAPIXELS))
			internalFormat = GL_RG8;
		else if (flags & DDPF_LUMINANCE)
			internalFormat = GL_R8;
		// Used in some older DDS files for alpha channel only uncompressed data
		// dwRGBBitCount contains the alpha channel bitcount; dwABitMask contains valid data
		else if (flags & DDPF_ALPHA)
			internalFormat = GL_R8;
		else
			RETURNF_MSG_X("Unsupported DDS uncompressed pixel format: %u", flags);

		loadPixels(internalFormat, type);

		if (redMask > blueMask && bitCount > 16)
			texFormat_.bgrFormat();
	}

	if (mipMapCount_ > 1)
	{
		LOGI_X("MIP Maps: %d", mipMapCount_);
		mipDataOffsets_ = nctl::makeUnique<unsigned long[]>(mipMapCount_);
		mipDataSizes_ = nctl::makeUnique<unsigned long[]>(mipMapCount_);
		unsigned long dataSizesSum = TextureFormat::calculateMipSizes(internalFormat, width_, height_, mipMapCount_, mipDataOffsets_.get(), mipDataSizes_.get());
		if (dataSizesSum != dataSize_)
			LOGW_X("The sum of MIP maps size (%ld) is different than texture total data (%ld)", dataSizesSum, dataSize_);
	}

	return true;
}

}
