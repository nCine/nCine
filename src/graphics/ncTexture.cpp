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

/// Set texture filtering for both magnification and minification
void ncTexture::SetFiltering(GLenum eFilter)
{
	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, eFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, eFilter);
}

ncTexture* ncTexture::FromId(unsigned int uId)
{
	ncObject *pObject = ncServiceLocator::GetIndexer().Object(uId);

	if(pObject)
	{
		if (pObject->Type() == sType())
			return static_cast<ncTexture *>(pObject);
		else // Cannot cast
		{
			ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, "ncTexture::FromId - Object of wrong type");
	//		exit(-1);
		}
	}
	else // NULL
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, "ncTexture::FromId - Object not found");
		return NULL;
	}
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
