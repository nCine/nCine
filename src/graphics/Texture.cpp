#ifndef NO_GLEW
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
	#include <GL/glu.h>
#endif

#include "Texture.h"
#include "../ServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Texture::Texture() : m_uiId(0)
{
	glGenTextures(1, &m_uiId);
}


Texture::Texture(const char *pFilename) : m_uiId(0)
{
	glGenTextures(1, &m_uiId);
	Bind();
	Load(pFilename);
}


Texture::Texture(SDL_Surface *pImage) : m_uiId(0)
{
	glGenTextures(1, &m_uiId);
	Bind();
	Load(pImage);
}


Texture::Texture(TextureFormat format, int iWidth, int iHeight, GLubyte *pPixels) : m_uiId(0)
{
	glGenTextures(1, &m_uiId);
	Bind();
	Load(format, iWidth, iHeight, pPixels);
}


Texture::~Texture()
{
	glDeleteTextures(1, &m_uiId);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Set texture filtering for both magnification and minification
void Texture::SetFiltering(GLenum eFilter)
{
	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, eFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, eFilter);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Texture::Load(const char *pFilename)
{
	SDL_Surface *pImage;

	ServiceLocator::GetLogger().Write(1, (char *)"Texture::Load - Loading \"%s\"", pFilename);
	if (!(pImage = (SDL_Surface *)IMG_Load(pFilename))) {
		ServiceLocator::GetLogger().Write(2, (char *)"Texture::Load - Cannot load \"%s\"", pFilename);
		exit(-1);
	}

	Load(pImage);

	SDL_FreeSurface(pImage);
}


void Texture::Load(SDL_Surface *pSurface)
{
	Uint8 uiBpp;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	uiBpp = pSurface->format->BitsPerPixel;
	TextureFormat *pFormat = 0;
	if (uiBpp == 32)
		pFormat = new TextureFormat(GL_RGBA8);
	else if (uiBpp  == 24)
		pFormat = new TextureFormat(GL_RGB8);
	else {
		ServiceLocator::GetLogger().Write(2, (char *)"Texture::Load - Not a true color image: %d", uiBpp);
		exit(-1);
	}

	GLenum eFormat;
	if (pSurface->format->Rmask == 0x000000ff)
		eFormat = pFormat->Format();
	else
		eFormat = pFormat->BGRFormat();

	glTexImage2D(GL_TEXTURE_2D, 0, pFormat->Internal(), pSurface->w, pSurface->h, 0, pFormat->Format(), pFormat->Type(), pSurface->pixels);
}


void Texture::Load(TextureFormat format, int iWidth, int iHeight, GLubyte *pPixels)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format.Internal(), iWidth, iHeight, 0, format.Format(), format.Type(), pPixels);
}
