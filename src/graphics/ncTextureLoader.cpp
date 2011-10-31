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
	: m_iWidth(0), m_iHeight(0), m_iBpp(0), m_iHeaderSize(0),
	  m_lFileSize(0), m_uPixels(NULL), m_pFile(NULL)
	#ifndef __ANDROID__
	,m_pSDLSurface(NULL)
	#endif
{
	char *pDotChar = strrchr(const_cast<char *>(pFilename), '.');

	// A dot followed by at least three extension characters
	if (!pDotChar || strlen(pDotChar) < 4)
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncTextureLoader::ncTextureLoader - No extension for file \"%s\"", pFilename);
		exit(-1);
	}

	#ifndef __ANDROID__
	if (!strncmp(pDotChar, ".png", 4) || !strncmp(pDotChar, ".jpg", 4))
		LoadSDL(pFilename);
	#else
	if (!strncmp(pDotChar, ".pkm", 4))
	{
		ReadETC1Header(pFilename);
		LoadCompressed(pFilename, GL_ETC1_RGB8_OES);
	}
	// HACK: assuming all DDS textures are ATITC compressed
	else if (!strncmp(pDotChar, ".dds", 4))
	{
		ReadDDSHeader(pFilename);
		// HACK: assuming all ATITC textures have explicit alpha
		// There could also be interpolated or no alpha at all
		LoadCompressed(pFilename, GL_ATC_RGBA_EXPLICIT_ALPHA_AMD);
	}
	#endif
	else
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncTextureLoader::ncTextureLoader - Extension unknown \"%s\"", pDotChar);
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

	if (m_pFile)
		fclose(m_pFile);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

#ifndef __ANDROID__
void ncTextureLoader::LoadSDL(const char *pFilename)
{
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncTextureLoader::LoadSDL - Loading \"%s\"", pFilename);
	if (!(m_pSDLSurface = (SDL_Surface *)IMG_Load(pFilename))) {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncTextureLoader::LoadSDL - Cannot load \"%s\"", pFilename);
		exit(-1);
	}

	m_iBpp = m_pSDLSurface->format->BitsPerPixel;
	ncTextureFormat *pFormat = 0;
	if (m_iBpp == 32)
		m_texFormat = ncTextureFormat(GL_RGBA8);
	else if (m_iBpp  == 24)
		m_texFormat = ncTextureFormat(GL_RGB8);
	else {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncTextureLoader::LoadSDL - Not a true color image: %d", m_iBpp);
		exit(-1);
	}

	GLenum eFormat;
	if (m_pSDLSurface->format->Rmask != 0x000000ff)
		m_texFormat.BGRFormat();

	m_iWidth = m_pSDLSurface->w;
	m_iHeight = m_pSDLSurface->h;
	m_uPixels = static_cast<GLubyte*>(m_pSDLSurface->pixels);
}
#endif

void ncTextureLoader::ReadETC1Header(const char *pFilename)
{
	m_pFile = fopen(pFilename, "rb");
	if (!m_pFile)
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncTextureLoader::ReadETC1Header - Cannot open the file \"%s\"", pFilename);
		exit(-1);
	}

	// ETC1 header is 16 bytes long
	// It have been splitted in two structures to avoid compiler-specific alignment pragmas
	ETC1_magic magic;
	ETC1_header header;
	fread(&magic, 8, 1, m_pFile);

	// Checking for the header presence
	if (strncmp(magic.cMagicId, "PKM 10", 6) == 0)
	{
		fread(&header, 8, 1, m_pFile);
		m_iHeaderSize = 16;
		uint16_t swappedWidth2 = (header.uWidth2 >> 8) | (header.uWidth2 << 8); // HACK: Big to little endian
		uint16_t swappedHeight2 = (header.uHeight2 >> 8) | (header.uHeight2 << 8); // HACK: Big to little endian
		m_iWidth = swappedWidth2;
		m_iHeight = swappedHeight2;

		ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncTextureLoader::ReadETC1Header - Header found: w:%d h:%d", m_iWidth, m_iHeight);
	}
}

void ncTextureLoader::ReadDDSHeader(const char *pFilename)
{
	m_pFile = fopen(pFilename, "rb");
	if (!m_pFile)
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncTextureLoader::ReadDDSHeader - Cannot open the file \"%s\"", pFilename);
		exit(-1);
	}

	// DDS header is 128 bytes long
	// It have been splitted in two structures to avoid compiler-specific alignment pragmas
	DDS_header header;
	fread(&header, 128, 1, m_pFile);

	// Checking for the header presence
	if (strncmp(header.cMagicId, "DDS ", 4) == 0)
	{
		m_iHeaderSize = 128;
		m_iWidth = header.dwWidth;
		m_iHeight = header.dwHeight;

		ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncTextureLoader::ReadDDSHeader - Header found: w:%d h:%d", m_iWidth, m_iHeight);
	}
}

void ncTextureLoader::LoadCompressed(const char *pFilename, GLenum eInternalFormat)
{
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncTextureLoader::LoadCompressed - Loading \"%s\"", pFilename);
	m_texFormat = ncTextureFormat(eInternalFormat);

	// If the file has not been already opened by a header reader method
	if (!m_pFile)
	{
		m_pFile = fopen(pFilename, "rb");
		if (!m_pFile)
		{
			ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncTextureLoader::LoadCompressed - Cannot open the file \"%s\"", pFilename);
			exit(-1);
		}
	}

	fseek(m_pFile, 0L, SEEK_END);
	m_lFileSize = ftell(m_pFile) - m_iHeaderSize;
	fseek(m_pFile, m_iHeaderSize, SEEK_SET);

	m_uPixels =  new unsigned char[m_lFileSize];
	fread(m_uPixels, m_lFileSize, 1, m_pFile);
}
