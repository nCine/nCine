#include <cstdlib> // for exit()
#include "ncTextureLoaderPVR.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureLoaderPVR::ncTextureLoaderPVR(const char *pFilename)
	: ncITextureLoader(pFilename)
{
	Init();
}

ncTextureLoaderPVR::ncTextureLoaderPVR(ncIFile *pFileHandle)
	: ncITextureLoader(pFileHandle)
{
	Init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncTextureLoaderPVR::Init()
{
	PVR3_header header;

	m_pFileHandle->Open(ncIFile::MODE_READ|ncIFile::MODE_BINARY);
	ReadHeader(header);
	ParseFormat(header);
}

/// Reads the PVR3 header and fills the corresponding structure
void ncTextureLoaderPVR::ReadHeader(PVR3_header &header)
{
	// PVR3 header is 52 bytes long
	m_pFileHandle->Read(&header, 52);

	// Checking for the header presence
	if (ncIFile::Int32FromLE(header.version) == 0x03525650) // "PVR"03
	{
		m_iHeaderSize = 52 + ncIFile::Int32FromLE(header.metaDataSize);
		m_iWidth = ncIFile::Int32FromLE(header.width);
		m_iHeight = ncIFile::Int32FromLE(header.height);
		m_iMipMapCount = header.numMipmaps;

		if (m_iMipMapCount == 0)
			m_iMipMapCount = 1;
	}
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPVR::ReadHeader - Not a PVR3 file");
		exit(-1);
	}
}

/// Parses the PVR3 header to determine its format
void ncTextureLoaderPVR::ParseFormat(const PVR3_header& header)
{
	GLenum eInternalFormat = GL_RGB; // to suppress uninitialized variable warning
	const ncGfxCapabilities& gfxCaps = ncServiceLocator::GfxCapabilities();

	uint64_t u64PixelFormat = ncIFile::Int64FromLE(header.pixelFormat);

	// Texture contains compressed data, most significant 4 bytes have been set to zero
	if (u64PixelFormat < 0x0000000100000000ULL)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderPVR::ParseFormat - Compressed format: %u", u64PixelFormat);

		// Check for OpenGL extension support
		switch(u64PixelFormat)
		{
#ifndef __ANDROID__
			case FMT_DXT1:
			case FMT_DXT3:
			case FMT_DXT5:
				if (gfxCaps.EXTTextureCompressionS3TC() == false)
				{
					ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPVR::ParseFormat - GL_EXT_texture_compression_s3tc not available");
					exit(-1);
				}
				break;
#else
			case FMT_PVRTC_2BPP_RGB:
			case FMT_PVRTC_2BPP_RGBA:
			case FMT_PVRTC_4BPP_RGB:
			case FMT_PVRTC_4BPP_RGBA:
				if (gfxCaps.IMGTextureCompressionPVRTC() == false)
				{
					ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPVR::ParseFormat - GL_IMG_texture_compression_pvrtc not available");
					exit(-1);
				}
				break;
			case FMT_PVRTCII_2BPP:
			case FMT_PVRTCII_4BPP:
				ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPVR::ParseFormat - No support for PVRTC-II compression");
				exit(-1);
				break;
#endif
			default:
				ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPVR::ParseFormat - Unsupported PVR3 compressed format: %u", u64PixelFormat);
				exit(-1);
				break;
		}

		// Parsing the pixel format
		switch(u64PixelFormat)
		{
#ifndef __ANDROID__
			case FMT_DXT1:
				eInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				break;
			case FMT_DXT3:
				eInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				break;
			case FMT_DXT5:
				eInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;
#else
			case FMT_ETC1:
				eInternalFormat = GL_ETC1_RGB8_OES;
				break;
			case FMT_PVRTC_2BPP_RGB:
				eInternalFormat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
				break;
			case FMT_PVRTC_2BPP_RGBA:
				eInternalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
				break;
			case FMT_PVRTC_4BPP_RGB:
				eInternalFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
				break;
			case FMT_PVRTC_4BPP_RGBA:
				eInternalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
				break;
#endif
		}

		LoadPixels(eInternalFormat);
	}
	// Texture contains uncompressed RGB data
	else
	{
		GLenum eType = GL_UNSIGNED_BYTE;

		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderPVR::ParseFormat - Uncompressed format: %c%c%c%c (%u, %u, %u, %u)",
			((char*)&u64PixelFormat)[0], ((char*)&u64PixelFormat)[1], ((char*)&u64PixelFormat)[2], ((char*)&u64PixelFormat)[3],
			((unsigned char*)&u64PixelFormat)[4], ((unsigned char*)&u64PixelFormat)[5], ((unsigned char*)&u64PixelFormat)[6], ((unsigned char*)&u64PixelFormat)[7]);

		switch(u64PixelFormat)
		{
			case FMT_RGB_888:
				eInternalFormat = GL_RGB;
				break;
			case FMT_RGBA_8888:
				eInternalFormat = GL_RGBA;
				break;
			case FMT_RGB_565:
				eInternalFormat = GL_RGB;
				eType = GL_UNSIGNED_SHORT_5_6_5;
				break;
			case FMT_RGBA_5551:
				eInternalFormat = GL_RGBA;
				eType = GL_UNSIGNED_SHORT_5_5_5_1;
				break;
			case FMT_RGBA_4444:
				eInternalFormat = GL_RGBA;
				eType = GL_UNSIGNED_SHORT_4_4_4_4;
				break;
			default:
				ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderPVR::ParseFormat - Unsupported PVR3 uncompressed format: %llx", u64PixelFormat);
				exit(-1);
				break;
		}

		LoadPixels(eInternalFormat, eType);
	}

	if (m_iMipMapCount > 1)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderPVR::ParseFormat - MIP Maps: %d", m_iMipMapCount);
		m_lMipDataOffsets = new long[m_iMipMapCount];
		m_lMipDataSizes = new long[m_iMipMapCount];
		long int lDataSizesSum = ncTextureFormat::CalculateMipSizes(eInternalFormat, m_iWidth, m_iHeight, m_iMipMapCount, m_lMipDataOffsets, m_lMipDataSizes);
		if (lDataSizesSum != m_lDataSize)
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncTextureLoaderPVR::ParseFormat - The sum of MIP maps size (%ld) is different than texture total data (%ld)", lDataSizesSum, m_lDataSize);
	}
}
