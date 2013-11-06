#include <cstdlib> // for exit()
#include "ncTextureLoaderKTX.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

uint8_t ncTextureLoaderKTX::m_uFileIdentifier[12] =
	{0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A}; // "«KTX 11»\r\n\x1A\n"};

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureLoaderKTX::ncTextureLoaderKTX(const char *pFilename)
	: ncITextureLoader(pFilename)
{
	Init();
}

ncTextureLoaderKTX::ncTextureLoaderKTX(ncIFile *pFileHandle)
	: ncITextureLoader(pFileHandle)
{
	Init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncTextureLoaderKTX::Init()
{
	KTX_header header;

	m_pFileHandle->Open(ncIFile::MODE_READ|ncIFile::MODE_BINARY);
	ReadHeader(header);
	ParseFormat(header);
}

/// Reads the KTX header and fills the corresponding structure
void ncTextureLoaderKTX::ReadHeader(KTX_header &header)
{
	bool bCheckPassed = true;

	// KTX header is 64 bytes long
	m_pFileHandle->Read(&header, 64);

	for (int i = 0; i < KTX_IDENTIFIER_LENGTH; i++)
	{
		if (header.identifier[i] != m_uFileIdentifier[i])
			bCheckPassed = false;
	}

	// Checking for the header identifier
	if (bCheckPassed)
	{
		if (header.endianess == 0x01020304)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderKTX::ReadHeader - File endianess doesn't match machine one");
			exit(EXIT_FAILURE);
		}

		m_iHeaderSize = 64 + ncIFile::Int32FromLE(header.bytesOfKeyValueData);
		m_iWidth = ncIFile::Int32FromLE(header.pixelWidth);
		m_iHeight = ncIFile::Int32FromLE(header.pixelHeight);
		m_iMipMapCount = header.numberOfMipmapLevels;
	}
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderKTX::ReadHeader - Not a KTX file");
		exit(EXIT_FAILURE);
	}
}

/// Parses the PVR3 header to determine its format
void ncTextureLoaderKTX::ParseFormat(const KTX_header& header)
{
	const ncGfxCapabilities& gfxCaps = ncServiceLocator::GfxCapabilities();

	GLenum eInternalFormat = ncIFile::Int32FromLE(header.glInternalFormat);
	GLenum eType = ncIFile::Int32FromLE(header.glType);

	switch(eInternalFormat)
	{
#ifndef __ANDROID__
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			if (gfxCaps.EXTTextureCompressionS3TC() == false)
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderKTR::ParseFormat - GL_EXT_texture_compression_s3tc not available");
				exit(EXIT_FAILURE);
			}
			break;
#else
		case GL_ETC1_RGB8_OES:
			if (gfxCaps.OESCompressedETC1RGB8Texture() == false)
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderKTX::ParseFormat - GL_OES_compressed_ETC1_RGB8_texture not available");
				exit(EXIT_FAILURE);
			}
			break;

		case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
		case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
			if (gfxCaps.IMGTextureCompressionPVRTC() == false)
			{
				ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderKTX::ParseFormat - GL_IMG_texture_compression_pvrtc not available");
				exit(EXIT_FAILURE);
			}
			break;
#endif
	}

	LoadPixels(eInternalFormat, eType);

	if (m_iMipMapCount > 1)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderKTX::ParseFormat - MIP Maps: %d", m_iMipMapCount);
		m_lMipDataOffsets = new long[m_iMipMapCount];
		m_lMipDataSizes = new long[m_iMipMapCount];
		long int lDataSizesSum = ncTextureFormat::CalculateMipSizes(eInternalFormat, m_iWidth, m_iHeight, m_iMipMapCount, m_lMipDataOffsets, m_lMipDataSizes);

		// HACK: accounting for "UInt32 imageSize;" on top of each MIP level
		for (int i = 0; i < m_iMipMapCount; i++)
			m_lMipDataOffsets[i] += 4 * (i+1);
		lDataSizesSum += 4 * m_iMipMapCount;

		if (lDataSizesSum != m_lDataSize)
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncTextureLoaderKTX::ParseFormat - The sum of MIP maps size (%ld) is different than texture total data (%ld)", lDataSizesSum, m_lDataSize);
	}
}
