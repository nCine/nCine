#include "ncSprite.h"
#include "ncServiceLocator.h"


///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncSprite::ncSprite(ncSceneNode* pParent, ncTexture *pTexture, int iX, int iY)
	: ncSceneNode(pParent, iX, iY), m_pTexture(pTexture)
{
	m_eType = SPRITE_TYPE;
	m_iWidth = pTexture->Width();
	m_iHeight = pTexture->Height();
}

ncSprite::ncSprite(ncTexture *pTexture, int iX, int iY)
	: ncSceneNode(NULL, iX, iY), m_pTexture(pTexture)
{
	m_eType = SPRITE_TYPE;
	m_iWidth = pTexture->Width();
	m_iHeight = pTexture->Height();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncSprite::Update(unsigned long int ulInterval)
{
	ncSceneNode::Update(ulInterval);
}
