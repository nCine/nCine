#ifdef __ANDROID__
	#include <stdlib.h> // for exit()
#endif
#include <cstring>
#include "ncTextureLoader.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureLoader::ncTextureLoader(const char *pFilename)
	: m_pFileHandle(NULL), m_iWidth(0), m_iHeight(0),
	  m_iBpp(0), m_iHeaderSize(0), m_lFileSize(0), m_uPixels(NULL)
#ifndef __ANDROID__
	,m_pSDLSurface(NULL)
#endif
{
	m_pFileHandle = ncIFile::CreateFileHandle(pFilename);

	const ncGfxCapabilities& gfxCaps = ncServiceLocator::GfxCapabilities();
#ifndef __ANDROID__
	if (m_pFileHandle->HasExtension("png") || m_pFileHandle->HasExtension("jpg"))
		LoadSDL();
#else
	if (m_pFileHandle->HasExtension("pkm"))
	{
		ReadETC1Header();

		// Checking for extension availability before loading
		if (gfxCaps.OESCompressedETC1RGB8Texture() == false)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, "ncTextureLoader::ncTextureLoader - GL_OES_compressed_ETC1_RGB8_texture not available");
			exit(-1);
		}

		LoadCompressed(GL_ETC1_RGB8_OES);
	}
#endif
	else if (m_pFileHandle->HasExtension("dds"))
	{
		ReadDDSHeader();
		GLenum eInternalFormat;

#ifndef __ANDROID__
		if (!strncmp((char *)&m_fourCC, "DXT1", 4))
			eInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		else if (!strncmp((char *)&m_fourCC, "DXT3", 4))
			eInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		else if (!strncmp((char *)&m_fourCC, "DXT5", 4))
			eInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		else
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoader::ncTextureLoader - Unsupported DDS compression \"%s\"", m_fourCC);
			exit(-1);
		}

		// Checking for extension availability before loading
		if (gfxCaps.EXTTextureCompressionS3TC() == false)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, "ncTextureLoader::ncTextureLoader - GL_EXT_texture_compression_s3tc not available");
			exit(-1);
		}
#else
		bool bATCFormat = false;
		if (!strncmp((char *)&m_fourCC, "ATC ", 4))
		{
			eInternalFormat = GL_ATC_RGB_AMD;
			bATCFormat = true;
		}
		else if (!strncmp((char *)&m_fourCC, "ATCA", 4))
		{
			eInternalFormat = GL_ATC_RGBA_EXPLICIT_ALPHA_AMD;
			bATCFormat = true;
		}
		else if (!strncmp((char *)&m_fourCC, "ATCI", 4))
		{
			eInternalFormat = GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD;
			bATCFormat = true;
		}
		else
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoader::ncTextureLoader - Unsupported DDS compression \"%s\"", m_fourCC);
			exit(-1);
		}

		// Checking for extension availability before loading
		if (bATCFormat && gfxCaps.AMDCompressedATCTexture() == false)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, "ncTextureLoader::ncTextureLoader - GL_AMD_compressed_ATC_texture not available");
			exit(-1);
		}
#endif

		LoadCompressed(eInternalFormat);
	}
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoader::ncTextureLoader - Extension unknown \"%s\"", m_pFileHandle->Extension());
		exit(-1);
	}
}

ncTextureLoader::~ncTextureLoader()
{
#ifndef __ANDROID__
	if (m_pSDLSurface)
	{
		SDL_FreeSurface(m_pSDLSurface);
		m_uPixels = NULL;
	}
#endif

	if (m_uPixels)
		delete[] m_uPixels;

	if (m_pFileHandle)
		delete m_pFileHandle;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

#ifndef __ANDROID__
void ncTextureLoader::LoadSDL()
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoader::LoadSDL - Loading \"%s\"", m_pFileHandle->Filename());
	m_pSDLSurface = (SDL_Surface *)IMG_Load(m_pFileHandle->Filename());
	if (!m_pSDLSurface)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoader::LoadSDL - Cannot load \"%s\"", m_pFileHandle->Filename());
		exit(-1);
	}

	m_iBpp = m_pSDLSurface->format->BitsPerPixel;
	if (m_iBpp == 32)
		m_texFormat = ncTextureFormat(GL_RGBA8);
	else if (m_iBpp  == 24)
		m_texFormat = ncTextureFormat(GL_RGB8);
	else if (m_iBpp == 8)
		m_texFormat = ncTextureFormat(GL_ALPHA8);
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoader::LoadSDL - Not a true color or alpha image: %d", m_iBpp);
		exit(-1);
	}

	if (m_pSDLSurface->format->Rmask != 0x000000ff)
		m_texFormat.BGRFormat();

	m_iWidth = m_pSDLSurface->w;
	m_iHeight = m_pSDLSurface->h;
	m_uPixels = static_cast<GLubyte*>(m_pSDLSurface->pixels);
}
#else
/// Reads an ETC1 header and fills the corresponding structure
void ncTextureLoader::ReadETC1Header()
{
	m_pFileHandle->Open(ncIFile::MODE_READ|ncIFile::MODE_BINARY);

	// ETC1 header is 16 bytes long
	// It have been splitted in two structures to avoid compiler-specific alignment pragmas
	ETC1_magic magic;
	ETC1_header header;
	m_pFileHandle->Read(&magic, 8);

	// Checking for the header presence
	if (strncmp(magic.cMagicId, "PKM 10", 6) == 0)
	{
		m_pFileHandle->Read(&header, 8);
		m_iHeaderSize = 16;
		m_iWidth = ncIFile::Int16FromBE(header.uWidth2);
		m_iHeight = ncIFile::Int16FromBE(header.uHeight2);

		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoader::ReadETC1Header - Header found: w:%d h:%d", m_iWidth, m_iHeight);
	}
}
#endif

/// Reads a DDS header and fills the corresponding structure
void ncTextureLoader::ReadDDSHeader()
{
	m_pFileHandle->Open(ncIFile::MODE_READ|ncIFile::MODE_BINARY);

	// DDS header is 128 bytes long
	// It have been splitted in two structures to avoid compiler-specific alignment pragmas
	DDS_header header;
	m_pFileHandle->Read(&header, 128);

	// Checking for the header presence
	if (strncmp(header.cMagicId, "DDS ", 4) == 0)
	{
		m_iHeaderSize = 128;
		m_iWidth = header.dwWidth;
		m_iHeight = header.dwHeight;
		m_fourCC = header.ddspf[2];

		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoader::ReadDDSHeader - Header found: w:%d h:%d FourCC: %c%c%c%c",
			m_iWidth, m_iHeight, ((char*)&m_fourCC)[0], ((char*)&m_fourCC)[1], ((char*)&m_fourCC)[2], ((char*)&m_fourCC)[3]);
	}
}

/// Loads a texture file holding compressed data
void ncTextureLoader::LoadCompressed(GLenum eInternalFormat)
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoader::LoadCompressed - Loading \"%s\"", m_pFileHandle->Filename());
	m_texFormat = ncTextureFormat(eInternalFormat);

	// If the file has not been already opened by a header reader method
	if (m_pFileHandle->IsOpened() == false)
		m_pFileHandle->Open(ncIFile::MODE_READ|ncIFile::MODE_BINARY);

	m_lFileSize = m_pFileHandle->Size() - m_iHeaderSize;
	m_pFileHandle->Seek(m_iHeaderSize, SEEK_SET);

	m_uPixels =  new unsigned char[m_lFileSize];
	m_pFileHandle->Read(m_uPixels, m_lFileSize);
}
