#include "return_macros.h"
#include "TextureLoaderPvr.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderPvr::TextureLoaderPvr(nctl::UniquePtr<IFile> fileHandle)
    : ITextureLoader(nctl::move(fileHandle))
{
	Pvr3Header header;

	fileHandle_->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);
	RETURN_ASSERT_MSG_X(fileHandle_->isOpened(), "File \"%s\" cannot be opened", fileHandle_->filename());
	const bool headerRead = readHeader(header);
	RETURN_ASSERT_MSG(headerRead, "PVR header cannot be read");
	const bool formatParsed = parseFormat(header);
	RETURN_ASSERT_MSG(formatParsed, "PVR format cannot be parsed");

	hasLoaded_ = true;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool TextureLoaderPvr::readHeader(Pvr3Header &header)
{
	// PVR3 header is 52 bytes long
	fileHandle_->read(&header, 52);

	// Checking for the header presence ("PVR"03)
	RETURNF_ASSERT_MSG(IFile::int32FromLE(header.version) == 0x03525650, "Not a PVR3 file");

	headerSize_ = 52 + IFile::int32FromLE(header.metaDataSize);
	width_ = IFile::int32FromLE(header.width);
	height_ = IFile::int32FromLE(header.height);
	mipMapCount_ = header.numMipmaps;

	if (mipMapCount_ == 0)
		mipMapCount_ = 1;

	return true;
}

bool TextureLoaderPvr::parseFormat(const Pvr3Header &header)
{
	GLenum internalFormat = GL_RGB8; // to suppress uninitialized variable warning

	uint64_t pixelFormat = IFile::int64FromLE(header.pixelFormat);

	// Texture contains compressed data, most significant 4 bytes have been set to zero
	if (pixelFormat < 0x0000000100000000ULL)
	{
		LOGI_X("Compressed format: %u", pixelFormat);

		// Parsing the pixel format
		switch (pixelFormat)
		{
#ifndef __ANDROID__
			case FMT_DXT1:
				internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				break;
			case FMT_DXT3:
				internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				break;
			case FMT_DXT5:
				internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;
#else
			case FMT_ETC1:
				internalFormat = GL_ETC1_RGB8_OES;
				break;
			case FMT_PVRTC_2BPP_RGB:
				internalFormat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
				break;
			case FMT_PVRTC_2BPP_RGBA:
				internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
				break;
			case FMT_PVRTC_4BPP_RGB:
				internalFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
				break;
			case FMT_PVRTC_4BPP_RGBA:
				internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
				break;
			case FMT_PVRTCII_2BPP:
			case FMT_PVRTCII_4BPP:
				FATAL_MSG("No support for PVRTC-II compression");
				break;
			case FMT_ETC2_RGB:
				internalFormat = GL_COMPRESSED_RGB8_ETC2;
				break;
			case FMT_ETC2_RGBA:
				internalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC;
				break;
			case FMT_ETC2_RGB_A1:
				internalFormat = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
				break;
			case FMT_EAC_R11:
				internalFormat = GL_COMPRESSED_R11_EAC;
				break;
			case FMT_EAC_RG11:
				internalFormat = GL_COMPRESSED_RG11_EAC;
				break;
	#if __ANDROID_API__ >= 21
			case FMT_ASTC_4x4:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
				break;
			case FMT_ASTC_5x4:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_5x4_KHR;
				break;
			case FMT_ASTC_5x5:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_5x5_KHR;
				break;
			case FMT_ASTC_6x5:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_6x5_KHR;
				break;
			case FMT_ASTC_6x6:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
				break;
			case FMT_ASTC_8x5:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_8x5_KHR;
				break;
			case FMT_ASTC_8x6:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_8x6_KHR;
				break;
			case FMT_ASTC_8x8:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_8x8_KHR;
				break;
			case FMT_ASTC_10x5:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_10x5_KHR;
				break;
			case FMT_ASTC_10x6:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_10x6_KHR;
				break;
			case FMT_ASTC_10x8:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_10x8_KHR;
				break;
			case FMT_ASTC_10x10:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_10x10_KHR;
				break;
			case FMT_ASTC_12x10:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_12x10_KHR;
				break;
			case FMT_ASTC_12x12:
				internalFormat = GL_COMPRESSED_RGBA_ASTC_12x12_KHR;
				break;
	#endif
#endif
			default:
				RETURNF_MSG_X("Unsupported PVR3 compressed format: 0x%llx", pixelFormat);
				break;
		}

		loadPixels(internalFormat);
	}
	// Texture contains uncompressed data
	else
	{
		GLenum type = GL_UNSIGNED_BYTE;

		LOGI_X("Uncompressed format: %c%c%c%c (%u, %u, %u, %u)",
		       reinterpret_cast<char *>(&pixelFormat)[0], reinterpret_cast<char *>(&pixelFormat)[1],
		       reinterpret_cast<char *>(&pixelFormat)[2], reinterpret_cast<char *>(&pixelFormat)[3],
		       reinterpret_cast<unsigned char *>(&pixelFormat)[4], reinterpret_cast<unsigned char *>(&pixelFormat)[5],
		       reinterpret_cast<unsigned char *>(&pixelFormat)[6], reinterpret_cast<unsigned char *>(&pixelFormat)[7]);

		switch (pixelFormat)
		{
			case FMT_BGRA_8888:
#if !defined(__ANDROID__) && !defined(WITH_ANGLE)
				internalFormat = GL_BGRA;
#else
				internalFormat = GL_BGRA_EXT;
#endif
				break;
			case FMT_RGBA_8888:
				internalFormat = GL_RGBA8;
				break;
			case FMT_RGB_888:
				internalFormat = GL_RGB8;
				break;
			case FMT_RGB_565:
				internalFormat = GL_RGB565;
				type = GL_UNSIGNED_SHORT_5_6_5;
				break;
			case FMT_RGBA_5551:
				internalFormat = GL_RGB5_A1;
				type = GL_UNSIGNED_SHORT_5_5_5_1;
				break;
			case FMT_RGBA_4444:
				internalFormat = GL_RGBA4;
				type = GL_UNSIGNED_SHORT_4_4_4_4;
				break;
			case FMT_LA_88:
				internalFormat = GL_RG8;
				break;
			case FMT_L_8:
				internalFormat = GL_R8;
				break;
			case FMT_A_8:
				internalFormat = GL_R8;
				break;
			default:
				RETURNF_MSG_X("Unsupported PVR3 uncompressed format: 0x%llx", pixelFormat);
				break;
		}

		loadPixels(internalFormat, type);
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
