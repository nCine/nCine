#ifdef __ANDROID__
	#include <stdlib.h> // for exit()
#endif
#include "ncTexture.h"
#include "ncTextureLoader.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTexture::ncTexture()
	: m_uId(0), m_iWidth(0), m_iHeight(0)
{
	glGenTextures(1, &m_uId);
}


ncTexture::ncTexture(const char *pFilename)
	: m_uId(0), m_iWidth(0), m_iHeight(0)
{
	glGenTextures(1, &m_uId);
	Bind();

	ncTextureLoader texLoader(pFilename);
	Load(texLoader);
}

ncTexture::ncTexture(const char *pFilename, int iWidth, int iHeight)
	: m_uId(0), m_iWidth(0), m_iHeight(0)
{
	glGenTextures(1, &m_uId);
	Bind();

	ncTextureLoader texLoader(pFilename);
	Load(texLoader, iWidth, iHeight);
}

ncTexture::ncTexture(const char *pFilename, ncPoint size)
	: m_uId(0), m_iWidth(0), m_iHeight(0)
{
	glGenTextures(1, &m_uId);
	Bind();

	ncTextureLoader texLoader(pFilename);
	Load(texLoader, size.x, size.y);
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

void ncTexture::Load(const ncTextureLoader& texLoader)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	const ncTextureFormat &texFormat = texLoader.TexFormat();
	glTexImage2D(GL_TEXTURE_2D, 0, texFormat.Internal(),
				 texLoader.Width(), texLoader.Height(), 0,
				 texFormat.Format(), texFormat.Type(), texLoader.Pixels());

	m_iWidth = texLoader.Width();
	m_iHeight = texLoader.Height();
}

void ncTexture::Load(const ncTextureLoader& texLoader, int iWidth, int iHeight)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	const ncTextureFormat &texFormat = texLoader.TexFormat();
	if (texFormat.isCompressed())
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, texFormat.Internal(),
							   iWidth, iHeight, 0,
							   texLoader.FileSize(), texLoader.Pixels());
	else
		glTexImage2D(GL_TEXTURE_2D, 0, texFormat.Internal(),
					 texLoader.Width(), texLoader.Height(), 0,
					 texFormat.Format(), texFormat.Type(), texLoader.Pixels());

	m_iWidth = iWidth;
	m_iHeight = iHeight;
}
