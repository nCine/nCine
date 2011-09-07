#ifdef __ANDROID__
	#include <cstdlib> // for exit()
#endif
#include "ncSprite.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncSprite::ncSprite(ncSceneNode* pParent, ncTexture *pTexture, int iX, int iY)
	: ncDrawableNode(pParent, iX, iY), m_pTexture(pTexture), m_texRect(0, 0, 0, 0), m_fScaleFactor(1.0f)
{
	Init();
}

ncSprite::ncSprite(ncTexture *pTexture, int iX, int iY)
	: ncDrawableNode(NULL, iX, iY), m_pTexture(pTexture), m_texRect(0, 0, 0, 0), m_fScaleFactor(1.0f)
{
	Init();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncSprite::Update(unsigned long int ulInterval)
{
	ncSceneNode::Update(ulInterval);
}

ncSprite* ncSprite::FromId(unsigned int uId)
{
	ncObject *pObject = ncServiceLocator::GetIndexer().Object(uId);

	if(pObject)
	{
		if (pObject->Type() == sType())
			return static_cast<ncSprite *>(pObject);
		else // Cannot cast
		{
			ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, "ncSprite::FromId - Object of wrong type");
			exit(-1);
		}
	}
	else // NULL
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, "ncSprite::FromId - Object not found");
		return NULL;
	}
}


void ncSprite::UpdateRenderCommand()
{
	m_renderCmd.Material().SetTextureGLId(m_pTexture->GLId());
	m_renderCmd.Transformation().SetPosition(AbsPosition());
	SetVertices();
	m_renderCmd.Geometry().SetData(GL_TRIANGLE_STRIP, 0, 4, m_fVertices, m_fTexCoords);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncSprite::Init()
{
	m_eType = SPRITE_TYPE;
	if (m_pTexture)
	{
		m_iWidth = m_pTexture->Width();
		m_iHeight = m_pTexture->Height();
	}
}

void ncSprite::SetVertices()
{
	int iHalfWidth = m_iWidth * 0.5f;
	int iHalfHeight = m_iHeight * 0.5f;

	m_fVertices[0] = m_absX - iHalfWidth;	m_fVertices[1] = m_absY - iHalfHeight;
	m_fVertices[2] = m_absX - iHalfWidth;	m_fVertices[3] = m_absY + iHalfHeight;
	m_fVertices[4] = m_absX + iHalfWidth;	m_fVertices[5] = m_absY - iHalfHeight;

	m_fVertices[6] = m_absX + iHalfWidth;	m_fVertices[7] = m_absY + iHalfHeight;


	m_fTexCoords[0] = 0.0f;		m_fTexCoords[1] = 1.0f;
	m_fTexCoords[2] = 0.0f;		m_fTexCoords[3] = 0.0f;
	m_fTexCoords[4] = 1.0f;		m_fTexCoords[5] = 1.0f;

	m_fTexCoords[6] = 1.0f;		m_fTexCoords[7] = 0.0f;
}
