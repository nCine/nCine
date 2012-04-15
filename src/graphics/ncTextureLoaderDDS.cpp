#include <cstdlib> // for exit()
#include "ncTextureLoaderDDS.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureLoaderDDS::ncTextureLoaderDDS(const char *pFilename)
	: ncITextureLoader(pFilename)
{
	Init();
}

ncTextureLoaderDDS::ncTextureLoaderDDS(ncIFile *pFileHandle)
	: ncITextureLoader(pFileHandle)
{
	Init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncTextureLoaderDDS::Init()
{
	DDS_header header;

	m_pFileHandle->Open(ncIFile::MODE_READ|ncIFile::MODE_BINARY);
	ReadHeader(header);
	ParseFormat(header);
}

/// Reads the DDS header and fills the corresponding structure
void ncTextureLoaderDDS::ReadHeader(DDS_header& header)
{
	// DDS header is 128 bytes long
	m_pFileHandle->Read(&header, 128);

	// Checking for the header presence
	if (ncIFile::Int32FromLE(header.dwMagic) == 0x20534444) // "DDS "
	{
		m_iHeaderSize = 128;
		m_iWidth = ncIFile::Int32FromLE(header.dwWidth);
		m_iHeight = ncIFile::Int32FromLE(header.dwHeight);
	}
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderDDS::ReadHeader - Not a DDS file");
		exit(-1);
	}
}

/// Parses the DDS header to determine its format
void ncTextureLoaderDDS::ParseFormat(const DDS_header& header)
{
	GLenum eInternalFormat;
	const ncGfxCapabilities& gfxCaps = ncServiceLocator::GfxCapabilities();

	uint32_t uFlags = ncIFile::Int32FromLE(header.ddspf.dwFlags);

	// Texture contains compressed RGB data, dwFourCC contains valid data
	if (uFlags & DDPF_FOURCC)
	{
		uint32_t uFourCC = ncIFile::Int32FromLE(header.ddspf.dwFourCC);

		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderDDS::ParseFormat - FourCC: \"%c%c%c%c\" (0x%x)",
			((char*)&uFourCC)[0], ((char*)&uFourCC)[1], ((char*)&uFourCC)[2], ((char*)&uFourCC)[3], uFourCC);

		// Check for OpenGL extension support
		switch(uFourCC)
		{
#ifndef __ANDROID__
			case DDS_DXT1:
			case DDS_DXT3:
			case DDS_DXT5:
				if (gfxCaps.EXTTextureCompressionS3TC() == false)
				{
					ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderDDS::ParseFormat - GL_EXT_texture_compression_s3tc not available");
					exit(-1);
				}
				break;
#else
			case DDS_ATC:
			case DDS_ATCA:
			case DDS_ATCI:
				if (gfxCaps.AMDCompressedATCTexture() == false)
				{
					ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderDDS::ParseFormat - GL_AMD_compressed_ATC_texture not available");
					exit(-1);
				}
				break;
#endif
			default:
				ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderDDS::ParseFormat - Unsupported DDS compression \"%s\"", uFourCC);
				exit(-1);
				break;
		}

		// Parsing the FourCC format
		switch(uFourCC)
		{
#ifndef __ANDROID__
			case DDS_DXT1:
				eInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				break;
			case DDS_DXT3:
				eInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				break;
			case DDS_DXT5:
				eInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;
#else
			case DDS_ATC:
				eInternalFormat = GL_ATC_RGB_AMD;
				break;
			case DDS_ATCA:
				eInternalFormat = GL_ATC_RGBA_EXPLICIT_ALPHA_AMD;
				break;
			case DDS_ATCI:
				eInternalFormat = GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD;
				break;
#endif
		}

		LoadPixels(eInternalFormat);
	}
	// Texture contains uncompressed RGB data
	// dwRGBBitCount and the RGB masks (dwRBitMask, dwRBitMask, dwRBitMask) contain valid data
	else if (uFlags & DDPF_RGB)
	{
		GLenum eType = GL_UNSIGNED_BYTE;

		uint32_t uBitCount = ncIFile::Int32FromLE(header.ddspf.dwRGBBitCount);
		uint32_t uRedMask = ncIFile::Int32FromLE(header.ddspf.dwRBitMask);
		uint32_t uGreenMask = ncIFile::Int32FromLE(header.ddspf.dwGBitMask);
		uint32_t uBlueMask = ncIFile::Int32FromLE(header.ddspf.dwBBitMask);
		uint32_t uAlphaMask = ncIFile::Int32FromLE(header.ddspf.dwABitMask);

		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderDDS::ParseFormat - Pixel masks (%ubit): R:0x%x G:0x%x B:0x%x A:0x%x",
			uBitCount, uRedMask, uGreenMask, uBlueMask, uAlphaMask);

		if (uRedMask == 0x00FF0000 && uGreenMask == 0x0000FF00 && uBlueMask == 0x000000FF && uAlphaMask == 0x0 ||
			uBlueMask == 0x00FF0000 && uGreenMask == 0x0000FF00 && uRedMask == 0x000000FF && uAlphaMask == 0x0) // 888
		{
			eInternalFormat = GL_RGB;
		}
		else if (uAlphaMask == 0xFF000000 && uRedMask == 0x00FF0000 && uGreenMask == 0x0000FF00 && uBlueMask == 0x000000FF ||
				 uAlphaMask == 0xFF000000 && uBlueMask == 0x00FF0000 && uGreenMask == 0x0000FF00 && uRedMask == 0x000000FF) // 8888
		{
			eInternalFormat = GL_RGBA;
		}
// 16 bits uncompressed DDS data is not compatbile with OpenGL color channels order
/*
		else if (uRedMask == 0xF800 && uGreenMask == 0x07E0 && uBlueMask == 0x001F) // 565
		{
			eInternalFormat = GL_RGB;
			eType = GL_UNSIGNED_SHORT_5_6_5;
		}
		else if (uAlphaMask == 0x8000 && uRedMask == 0x7C00 && uGreenMask == 0x03E0 && uBlueMask == 0x001F) // 5551
		{
			eInternalFormat = GL_RGBA;
			eType = GL_UNSIGNED_SHORT_5_5_5_1;
		}
		else if (uAlphaMask == 0xF000 && uRedMask == 0x0F00 && uGreenMask == 0x00F0 && uBlueMask == 0x000F) // 4444
		{
			eInternalFormat = GL_RGBA;
			eType = GL_UNSIGNED_SHORT_4_4_4_4;
		}
*/
		else
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderDDS::ParseFormat - Unsupported DDS pixel format");
			exit(-1);
		}

		LoadPixels(eInternalFormat, eType);
#ifndef __ANDROID__
		if (uRedMask > uBlueMask && uBitCount > 16)
			m_texFormat.BGRFormat();
#endif
	}
}
