#ifdef __ANDROID__
	#include <cstdlib> // for exit()
#endif
#include "ncTexture.h"
#include "ncTextureLoader.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTexture::ncTexture()
	: m_uGLId(0), m_iWidth(0), m_iHeight(0)
{
	m_eType = TEXTURE_TYPE;
	glGenTextures(1, &m_uGLId);
}


ncTexture::ncTexture(const char *pFilename)
	: m_uGLId(0), m_iWidth(0), m_iHeight(0)
{
	m_eType = TEXTURE_TYPE;
	SetName(pFilename);

	glGenTextures(1, &m_uGLId);
	Bind();

	ncTextureLoader texLoader(pFilename);
	Load(texLoader);
}

ncTexture::ncTexture(const char *pFilename, int iWidth, int iHeight)
	: m_uGLId(0), m_iWidth(0), m_iHeight(0)
{
	m_eType = TEXTURE_TYPE;
	SetName(pFilename);

	glGenTextures(1, &m_uGLId);
	Bind();

	ncTextureLoader texLoader(pFilename);
	Load(texLoader, iWidth, iHeight);
}

ncTexture::ncTexture(const char *pFilename, ncPoint size)
	: m_uGLId(0), m_iWidth(0), m_iHeight(0)
{
	m_eType = TEXTURE_TYPE;
	SetName(pFilename);

	glGenTextures(1, &m_uGLId);
	Bind();

	ncTextureLoader texLoader(pFilename);
	Load(texLoader, size.x, size.y);
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

/// Sets the red channel of a second texture as the alpha channel
void ncTexture::SetAlphaFromRed(ncTexture *pAlphaTex)
{
#ifndef __ANDROID__
	GLubyte	*pPixels = NULL;
	GLubyte	*pAlphaPixels = NULL;

	int iTextureArea = Width() * Height();
	if (pAlphaTex->Width() * pAlphaTex->Height() == iTextureArea)
	{

		pAlphaPixels = new GLubyte[iTextureArea];
		pPixels = new GLubyte[iTextureArea * 4];

		pAlphaTex->Bind();
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pAlphaPixels);
		Bind();
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pPixels);

		// FIXME: It does not work as expected
		for (int i = 0; i < iTextureArea; i++)
			pPixels[i*4 + 3] = pAlphaPixels[i];

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width(), Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pPixels);

		delete[] pAlphaPixels;
		delete[] pPixels;
	}
	else
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, "ncTexture::SetAlphaFromRed - Alpha texture has a different size");
#endif
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Loads a texture based on information from the texture format and loader
void ncTexture::Load(const ncTextureLoader& texLoader)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	const ncTextureFormat &texFormat = texLoader.TexFormat();
	if (texFormat.isCompressed())
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, texFormat.Internal(),
							   texLoader.Width(), texLoader.Height(), 0,
							   texLoader.FileSize(), texLoader.Pixels());
	else
		glTexImage2D(GL_TEXTURE_2D, 0, texFormat.Internal(),
					 texLoader.Width(), texLoader.Height(), 0,
					 texFormat.Format(), texFormat.Type(), texLoader.Pixels());

	m_iWidth = texLoader.Width();
	m_iHeight = texLoader.Height();
}

/// Loads a texture overriding the size detected by the texture loader
void ncTexture::Load(const ncTextureLoader& texLoader, int iWidth, int iHeight)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	const ncTextureFormat &texFormat = texLoader.TexFormat();
	if (texFormat.isCompressed())
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, texFormat.Internal(),
							   iWidth, iHeight, 0,
							   texLoader.FileSize(), texLoader.Pixels());
	else
		glTexImage2D(GL_TEXTURE_2D, 0, texFormat.Internal(),
					 iWidth, iHeight, 0,
					 texFormat.Format(), texFormat.Type(), texLoader.Pixels());

	m_iWidth = iWidth;
	m_iHeight = iHeight;
}
