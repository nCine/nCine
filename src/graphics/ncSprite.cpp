#include <cmath>
#ifdef __ANDROID__
	#include <cstdlib> // for exit()
#endif
#include "ncSprite.h"
#include "ncServiceLocator.h"
#include "ncRenderQueue.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncSprite::ncSprite(ncSceneNode* pParent, ncTexture *pTexture)
	: ncDrawableNode(pParent), m_pTexture(pTexture), m_texRect(0, 0, 0, 0)
{
	Init();
}

ncSprite::ncSprite(ncTexture *pTexture)
	: ncDrawableNode(NULL), m_pTexture(pTexture), m_texRect(0, 0, 0, 0)
{
	Init();
}

ncSprite::ncSprite(ncSceneNode* pParent, ncTexture *pTexture, int iX, int iY)
	: ncDrawableNode(pParent, iX, iY), m_pTexture(pTexture), m_texRect(0, 0, 0, 0)
{
	Init();
}

ncSprite::ncSprite(ncTexture *pTexture, int iX, int iY)
	: ncDrawableNode(NULL, iX, iY), m_pTexture(pTexture), m_texRect(0, 0, 0, 0)
{
	Init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncSprite::Init()
{
	m_eType = SPRITE_TYPE;
	SetPriority(ncDrawableNode::SCENE_PRIORITY);

	if (m_pTexture)
		SetTexRect(ncRect(0, 0, m_pTexture->Width(), m_pTexture->Height()));
}

void ncSprite::SetVertices()
{
	float leftPos = m_iWidth*m_fAbsScaleFactor*0.5f;
	float rightPos = -m_iWidth*m_fAbsScaleFactor*0.5f;
	float bottomPos = -m_iHeight*m_fAbsScaleFactor*0.5f;
	float topPos = m_iHeight*m_fAbsScaleFactor*0.5f;

	float sine = 0.0f;
	float cosine = 1.0f;
	if (m_fAbsRotation > sMinRotation && m_fAbsRotation < 360.0f - sMinRotation)
	{
		sine = sinf(-m_fAbsRotation * M_PI/180.0f);
		cosine = cosf(-m_fAbsRotation * M_PI/180.0f);
	}

	m_fVertices[0] = m_fAbsX + leftPos*cosine - bottomPos*sine;		m_fVertices[1] = m_fAbsY + bottomPos*cosine + leftPos*sine;
	m_fVertices[2] = m_fAbsX + leftPos*cosine - topPos*sine;		m_fVertices[3] = m_fAbsY + topPos*cosine + leftPos*sine;
	m_fVertices[4] = m_fAbsX + rightPos*cosine - bottomPos*sine;	m_fVertices[5] = m_fAbsY + bottomPos*cosine + rightPos*sine;

	m_fVertices[6] = m_fAbsX + rightPos*cosine - topPos*sine;		m_fVertices[7] = m_fAbsY + topPos*cosine + rightPos*sine;
}

void ncSprite::SetTexCoords()
{
	ncPoint texSize = m_pTexture->Size();
	float leftCoord = float(m_texRect.x)/float(texSize.x);
	float rightCoord = float(m_texRect.x+m_texRect.w)/float(texSize.x);
	float bottomCoord = float(m_texRect.y+m_texRect.h)/float(texSize.y);
	float topCoord = float(m_texRect.y)/float(texSize.y);

	m_fTexCoords[0] = leftCoord;	m_fTexCoords[1] = bottomCoord;
	m_fTexCoords[2] = leftCoord;	m_fTexCoords[3] = topCoord;
	m_fTexCoords[4] = rightCoord;	m_fTexCoords[5] = bottomCoord;

	m_fTexCoords[6] = rightCoord;	m_fTexCoords[7] = topCoord;
}

void ncSprite::UpdateRenderCommand()
{
	m_renderCmd.Material().SetTextureGLId(m_pTexture->GLId());
	m_renderCmd.Material().SetColor(m_color);
	m_renderCmd.Transformation().SetPosition(AbsPosition().x, AbsPosition().y);
	SetVertices();
	m_renderCmd.Geometry().SetData(GL_TRIANGLE_STRIP, 0, 4, m_fVertices, m_fTexCoords, NULL);
	m_renderCmd.CalculateSortKey();
}
