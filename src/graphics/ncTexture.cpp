#include <cstdlib> // for exit()
#include "ncTexture.h"
#include "ncITextureLoader.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTexture::ncTexture()
	: m_uGLId(0), m_iWidth(0), m_iHeight(0),
	  m_iMipMapLevels(1), m_bCompressed(false), m_bAlphaChannel(false)
{
	m_eType = TEXTURE_TYPE;
	glGenTextures(1, &m_uGLId);
}


ncTexture::ncTexture(const char *pFilename)
	: m_uGLId(0), m_iWidth(0), m_iHeight(0),
	  m_iMipMapLevels(1), m_bCompressed(false), m_bAlphaChannel(false)
{
	m_eType = TEXTURE_TYPE;
	SetName(pFilename);

	glGenTextures(1, &m_uGLId);
	Bind();

	ncITextureLoader *pTexLoader = ncITextureLoader::CreateFromFile(pFilename);
	Load(*pTexLoader);
	delete pTexLoader;
}

ncTexture::ncTexture(const char *pFilename, int iWidth, int iHeight)
	: m_uGLId(0), m_iWidth(0), m_iHeight(0), m_bCompressed(false), m_bAlphaChannel(false)
{
	m_eType = TEXTURE_TYPE;
	SetName(pFilename);

	glGenTextures(1, &m_uGLId);
	Bind();

	ncITextureLoader *pTexLoader = ncITextureLoader::CreateFromFile(pFilename);
	Load(*pTexLoader, iWidth, iHeight);
	delete pTexLoader;
}

ncTexture::ncTexture(const char *pFilename, ncPoint size)
	: m_uGLId(0), m_iWidth(0), m_iHeight(0), m_bCompressed(false), m_bAlphaChannel(false)
{
	m_eType = TEXTURE_TYPE;
	SetName(pFilename);

	glGenTextures(1, &m_uGLId);
	Bind();

	ncITextureLoader *pTexLoader = ncITextureLoader::CreateFromFile(pFilename);
	Load(*pTexLoader, size.x, size.y);
	delete pTexLoader;
}

ncTexture::~ncTexture()
{
	glDeleteTextures(1, &m_uGLId);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Sets texture filtering for both magnification and minification
void ncTexture::SetFiltering(GLenum eFilter)
{
	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, eFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, eFilter);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Loads a texture based on information from the texture format and loader
void ncTexture::Load(const ncITextureLoader& texLoader)
{
	Load(texLoader, texLoader.Width(), texLoader.Height());
}

/// Loads a texture overriding the size detected by the texture loader
void ncTexture::Load(const ncITextureLoader& texLoader, int iWidth, int iHeight)
{
	const ncGfxCapabilities& gfxCaps = ncServiceLocator::GfxCapabilities();
	if (iWidth > gfxCaps.MaxTextureSize() || iHeight > gfxCaps.MaxTextureSize())
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, "ncTexture::Load - Texture size is bigger than device maximum");
		exit(EXIT_FAILURE);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (texLoader.MipMapCount() > 1)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#ifndef __ANDROID__
		// To prevent artifacts if the MIP map chain is not complete
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, texLoader.MipMapCount());
#endif
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	const ncTextureFormat &texFormat = texLoader.TexFormat();

	int iLevelWidth = iWidth;
	int iLevelHeight = iHeight;
	for (int i = 0; i < texLoader.MipMapCount(); i++)
	{
		if (texFormat.isCompressed())
		{
			glCompressedTexImage2D(GL_TEXTURE_2D, i, texFormat.Internal(), iLevelWidth, iLevelHeight, 0,
				texLoader.DataSize(i), texLoader.Pixels(i));
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, i, texFormat.Internal(), iLevelWidth, iLevelHeight, 0,
				texFormat.Format(), texFormat.Type(), texLoader.Pixels(i));
		}

		iLevelWidth /= 2;
		iLevelHeight /= 2;
	}

	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iMipMapLevels = texLoader.MipMapCount();
	m_bCompressed = texFormat.isCompressed();
	m_bAlphaChannel = texFormat.hasAlpha();
}
