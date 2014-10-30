#include <cstdlib> // for exit()
#include "TextureLoaderKtx.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

uint8_t TextureLoaderKtx::fileIdentifier_[12] =
{0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A}; // "«KTX 11»\r\n\x1A\n"};

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderKtx::TextureLoaderKtx(const char *filename)
	: ITextureLoader(filename)
{
	init();
}

TextureLoaderKtx::TextureLoaderKtx(IFile *fileHandle)
	: ITextureLoader(fileHandle)
{
	init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void TextureLoaderKtx::init()
{
	KtxHeader header;

	fileHandle_->open(IFile::MODE_READ | IFile::MODE_BINARY);
	readHeader(header);
	parseFormat(header);
}

/// Reads the KTX header and fills the corresponding structure
void TextureLoaderKtx::readHeader(KtxHeader &header)
{
	bool checkPassed = true;

	// KTX header is 64 bytes long
	fileHandle_->read(&header, 64);

	for (int i = 0; i < KtxIdentifierLength; i++)
	{
		if (header.identifier[i] != fileIdentifier_[i])
		{
			checkPassed = false;
		}
	}

	// Checking for the header identifier
	if (checkPassed)
	{
		if (header.endianess == 0x01020304)
		{
			LOGF("File endianess doesn't match machine one");
			exit(EXIT_FAILURE);
		}

		headerSize_ = 64 + IFile::int32FromLE(header.bytesOfKeyValueData);
		width_ = IFile::int32FromLE(header.pixelWidth);
		height_ = IFile::int32FromLE(header.pixelHeight);
		mipMapCount_ = header.numberOfMipmapLevels;
	}
	else
	{
		LOGF("Not a KTX file");
		exit(EXIT_FAILURE);
	}
}

/// Parses the PVR3 header to determine its format
void TextureLoaderKtx::parseFormat(const KtxHeader& header)
{
	const GfxCapabilities& gfxCaps = ServiceLocator::gfxCapabilities();

	GLenum internalFormat = IFile::int32FromLE(header.glInternalFormat);
	GLenum type = IFile::int32FromLE(header.glType);

	switch (internalFormat)
	{
#ifndef __ANDROID__
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			if (gfxCaps.extTextureCompressionS3TC() == false)
			{
				LOGF("GL_EXT_texture_compression_s3tc not available");
				exit(EXIT_FAILURE);
			}
			break;
#else
		case GL_ETC1_RGB8_OES:
			if (gfxCaps.oesCompressedETC1RGB8Texture() == false)
			{
				LOGF("GL_OES_compressed_ETC1_RGB8_texture not available");
				exit(EXIT_FAILURE);
			}
			break;

		case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
		case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
			if (gfxCaps.imgTextureCompressionPVRTC() == false)
			{
				LOGF("GL_IMG_texture_compression_pvrtc not available");
				exit(EXIT_FAILURE);
			}
			break;
#endif
	}

	loadPixels(internalFormat, type);

	if (mipMapCount_ > 1)
	{
		LOGI_X("MIP Maps: %d", mipMapCount_);
		mipDataOffsets_ = new long[mipMapCount_];
		mipDataSizes_ = new long[mipMapCount_];
		long int dataSizesSum = TextureFormat::calculateMipSizes(internalFormat, width_, height_, mipMapCount_, mipDataOffsets_, mipDataSizes_);

		// HACK: accounting for "UInt32 imageSize;" on top of each MIP level
		for (int i = 0; i < mipMapCount_; i++)
		{
			mipDataOffsets_[i] += 4 * (i + 1);
		}
		dataSizesSum += 4 * mipMapCount_;

		if (dataSizesSum != dataSize_)
		{
			LOGW_X("The sum of MIP maps size (%ld) is different than texture total data (%ld)", dataSizesSum, dataSize_);
		}
	}
}

}
