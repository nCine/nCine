#ifdef __ANDROID__
	#include <stdlib.h> // for exit()
	#include <string.h>
#else
	#include <cstring>
#endif

#include "ncTextureLoader.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureLoader::ncTextureLoader(const char *pFilename)
	: m_iWidth(0), m_iHeight(0), m_iBpp(0), m_lFileSize(0),
	  m_uPixels(NULL), m_pFile(NULL)
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
		LoadCompressed(pFilename, GL_ETC1_RGB8_OES);
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
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncTexture::Load - Loading \"%s\"", pFilename);
	if (!(m_pSDLSurface = (SDL_Surface *)IMG_Load(pFilename))) {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncTexture::Load - Cannot load \"%s\"", pFilename);
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

void ncTextureLoader::LoadCompressed(const char *pFilename, GLenum eInternalFormat)
{
	m_texFormat = ncTextureFormat(eInternalFormat);

	m_pFile = fopen(pFilename, "r");
	if (!m_pFile)
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncTextureLoader::LoadCompressed - Cannot open the file \"%s\"", pFilename);
		exit(-1);
	}

	fseek(m_pFile, 0L, SEEK_END);
	m_lFileSize = ftell(m_pFile);
	fseek(m_pFile, 0L, SEEK_SET);

	m_uPixels =  new unsigned char[m_lFileSize];
	fread(m_uPixels, m_lFileSize, 1, m_pFile);
}
