#ifdef __ANDROID__
	#include <stdlib.h> // for exit()
#endif
#include "ncTexture.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTexture::ncTexture() : m_uId(0), m_iWidth(0), m_iHeight(0)
{
	glGenTextures(1, &m_uId);
}

#ifndef __ANDROID__
ncTexture::ncTexture(const char *pFilename) : m_uId(0), m_iWidth(0), m_iHeight(0)
{
	glGenTextures(1, &m_uId);
	Bind();
	Load(pFilename);
}
#endif

#ifndef __ANDROID__
ncTexture::ncTexture(SDL_Surface *pImage) : m_uId(0)
{
	glGenTextures(1, &m_uId);
	Bind();
	Load(pImage);
}
#endif

ncTexture::ncTexture(ncTextureFormat format, ncPoint size, GLubyte *pPixels, GLsizei iBytes)
	: m_uId(0)
{
	glGenTextures(1, &m_uId);
	Bind();
	Load(format, size.x, size.y, pPixels, iBytes);
}


ncTexture::ncTexture(ncTextureFormat format, int iWidth, int iHeight, GLubyte *pPixels, GLsizei iBytes)
	: m_uId(0)
{
	glGenTextures(1, &m_uId);
	Bind();
	Load(format, iWidth, iHeight, pPixels, iBytes);
}

ncTexture::~ncTexture()
{
	glDeleteTextures(1, &m_uId);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Set texture filtering for both magnification and minification
void ncTexture::SetFiltering(GLenum eFilter)
{
	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, eFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, eFilter);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

#ifndef __ANDROID__
void ncTexture::Load(const char *pFilename)
{
	SDL_Surface *pImage;

	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncTexture::Load - Loading \"%s\"", pFilename);
	if (!(pImage = (SDL_Surface *)IMG_Load(pFilename))) {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncTexture::Load - Cannot load \"%s\"", pFilename);
		exit(-1);
	}

	Load(pImage);

	SDL_FreeSurface(pImage);
}
#endif

#ifndef __ANDROID__
void ncTexture::Load(SDL_Surface *pSurface)
{
	Uint8 uBpp;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	uBpp = pSurface->format->BitsPerPixel;
	ncTextureFormat *pFormat = 0;
	if (uBpp == 32)
		pFormat = new ncTextureFormat(GL_RGBA8);
	else if (uBpp  == 24)
		pFormat = new ncTextureFormat(GL_RGB8);
	else {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncTexture::Load - Not a true color image: %d", uBpp);
		exit(-1);
	}

	GLenum eFormat;
	if (pSurface->format->Rmask == 0x000000ff)
		eFormat = pFormat->Format();
	else
		eFormat = pFormat->BGRFormat();

	m_iWidth = pSurface->w;
	m_iHeight = pSurface->h;
	glTexImage2D(GL_TEXTURE_2D, 0, pFormat->Internal(), m_iWidth, m_iHeight, 0, pFormat->Format(), pFormat->Type(), pSurface->pixels);

	delete pFormat;
}
#endif

void ncTexture::Load(ncTextureFormat format, int iWidth, int iHeight, GLubyte *pPixels, GLsizei iBytes)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if (iBytes <= 0 && format.isCompressed())
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncTexture::Load - Compressed format and no size in bytes specified");
		exit(-1);
	}

	m_iWidth = iWidth;
	m_iHeight = iHeight;
	if (format.isCompressed())
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, format.Internal(), m_iWidth, m_iHeight, 0, iBytes, pPixels);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format.Internal(), m_iWidth, m_iHeight, 0, format.Format(), format.Type(), pPixels);
}
