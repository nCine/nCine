#include <cstdlib> // for exit()
#include "ncTextureLoaderPVR.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureLoaderPVR::ncTextureLoaderPVR(const char *filename)
	: ncITextureLoader(filename)
{
	init();
}

ncTextureLoaderPVR::ncTextureLoaderPVR(ncIFile *fileHandle)
	: ncITextureLoader(fileHandle)
{
	init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncTextureLoaderPVR::init()
{
	Pvr3Header header;

	fileHandle_->open(ncIFile::MODE_READ | ncIFile::MODE_BINARY);
	readHeader(header);
	parseFormat(header);
}

/// Reads the PVR3 header and fills the corresponding structure
void ncTextureLoaderPVR::readHeader(Pvr3Header &header)
{
	// PVR3 header is 52 bytes long
	fileHandle_->read(&header, 52);

	// Checking for the header presence
	if (ncIFile::int32FromLE(header.version) == 0x03525650) // "PVR"03
	{
		headerSize_ = 52 + ncIFile::int32FromLE(header.metaDataSize);
		width_ = ncIFile::int32FromLE(header.width);
		height_ = ncIFile::int32FromLE(header.height);
		mipMapCount_ = header.numMipmaps;

		if (mipMapCount_ == 0)
		{
			mipMapCount_ = 1;
		}
	}
	else
	{
		LOGF("Not a PVR3 file");
		exit(EXIT_FAILURE);
	}
}

/// Parses the PVR3 header to determine its format
void ncTextureLoaderPVR::parseFormat(const Pvr3Header& header)
{
	GLenum internalFormat = GL_RGB; // to suppress uninitialized variable warning
	const ncGfxCapabilities& gfxCaps = ncServiceLocator::gfxCapabilities();

	uint64_t pixelFormat = ncIFile::int64FromLE(header.pixelFormat);

	// Texture contains compressed data, most significant 4 bytes have been set to zero
	if (pixelFormat < 0x0000000100000000ULL)
	{
		LOGI_X("Compressed format: %u", pixelFormat);

		// Check for OpenGL extension support
		switch (pixelFormat)
		{
#ifndef __ANDROID__
			case FMT_DXT1:
			case FMT_DXT3:
			case FMT_DXT5:
				if (gfxCaps.extTextureCompressionS3TC() == false)
				{
					LOGF("GL_EXT_texture_compression_s3tc not available");
					exit(EXIT_FAILURE);
				}
				break;
#else
			case FMT_PVRTC_2BPP_RGB:
			case FMT_PVRTC_2BPP_RGBA:
			case FMT_PVRTC_4BPP_RGB:
			case FMT_PVRTC_4BPP_RGBA:
				if (gfxCaps.imgTextureCompressionPVRTC() == false)
				{
					LOGF("GL_IMG_texture_compression_pvrtc not available");
					exit(EXIT_FAILURE);
				}
				break;
			case FMT_PVRTCII_2BPP:
			case FMT_PVRTCII_4BPP:
				LOGF("No support for PVRTC-II compression");
				exit(EXIT_FAILURE);
				break;
#endif
			default:
				LOGF_X("Unsupported PVR3 compressed format: %u", pixelFormat);
				exit(EXIT_FAILURE);
				break;
		}

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
#endif
		}

		loadPixels(internalFormat);
	}
	// Texture contains uncompressed RGB data
	else
	{
		GLenum type = GL_UNSIGNED_BYTE;

		LOGI_X("Uncompressed format: %c%c%c%c (%u, %u, %u, %u)",
			((char*)&pixelFormat)[0], ((char*)&pixelFormat)[1], ((char*)&pixelFormat)[2], ((char*)&pixelFormat)[3],
			((unsigned char*)&pixelFormat)[4], ((unsigned char*)&pixelFormat)[5], ((unsigned char*)&pixelFormat)[6], ((unsigned char*)&pixelFormat)[7]);

		switch (pixelFormat)
		{
			case FMT_RGB_888:
				internalFormat = GL_RGB;
				break;
			case FMT_RGBA_8888:
				internalFormat = GL_RGBA;
				break;
			case FMT_RGB_565:
				internalFormat = GL_RGB;
				type = GL_UNSIGNED_SHORT_5_6_5;
				break;
			case FMT_RGBA_5551:
				internalFormat = GL_RGBA;
				type = GL_UNSIGNED_SHORT_5_5_5_1;
				break;
			case FMT_RGBA_4444:
				internalFormat = GL_RGBA;
				type = GL_UNSIGNED_SHORT_4_4_4_4;
				break;
			default:
				LOGF_X("Unsupported PVR3 uncompressed format: %llx", pixelFormat);
				exit(EXIT_FAILURE);
				break;
		}

		loadPixels(internalFormat, type);
	}

	if (mipMapCount_ > 1)
	{
		LOGI_X("MIP Maps: %d", mipMapCount_);
		mipDataOffsets_ = new long[mipMapCount_];
		mipDataSizes_ = new long[mipMapCount_];
		long int dataSizesSum = ncTextureFormat::calculateMipSizes(internalFormat, width_, height_, mipMapCount_, mipDataOffsets_, mipDataSizes_);
		if (dataSizesSum != dataSize_)
		{
			LOGW_X("The sum of MIP maps size (%ld) is different than texture total data (%ld)", dataSizesSum, dataSize_);
		}
	}
}
