#include "ncAnimatedSprite.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncAnimatedSprite::ncAnimatedSprite(ncSceneNode* pParent, ncTexture *pTexture)
	: ncSprite(pParent, pTexture), m_vAnims(4), m_iCurrentAnim(-1)
{

}

ncAnimatedSprite::ncAnimatedSprite(ncTexture *pTexture)
	: ncSprite(pTexture), m_vAnims(4), m_iCurrentAnim(-1)
{

}

ncAnimatedSprite::ncAnimatedSprite(ncSceneNode* pParent, ncTexture *pTexture, int iX, int iY)
	: ncSprite(pParent, pTexture, iX, iY), m_vAnims(4), m_iCurrentAnim(-1)
{

}

ncAnimatedSprite::ncAnimatedSprite(ncTexture *pTexture, int iX, int iY)
	: ncSprite(pTexture, iX, iY), m_vAnims(4), m_iCurrentAnim(-1)
{

}

ncAnimatedSprite::~ncAnimatedSprite()
{
	for (unsigned int i = 0; i < m_vAnims.Size(); i++)
		delete m_vAnims[i];
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncAnimatedSprite::Update(float fInterval)
{
	unsigned int uPreviousFrame = m_vAnims[m_iCurrentAnim]->Frame();
	m_vAnims[m_iCurrentAnim]->UpdateFrame(fInterval);

	// Updating sprite texture rectangle only on change
	if (uPreviousFrame != m_vAnims[m_iCurrentAnim]->Frame())
		SetTexRect(m_vAnims[m_iCurrentAnim]->Rect());

	ncSprite::Update(fInterval);
}

/// Adds a new animation
void ncAnimatedSprite::AddAnimation(ncRectAnimation* pAnim)
{
	if (pAnim)
	{
		m_vAnims.InsertBack(pAnim);
		m_iCurrentAnim = m_vAnims.Size() - 1;
		SetTexRect(m_vAnims[m_iCurrentAnim]->Rect());
	}
}

/// Sets the current animation and its frame number
void ncAnimatedSprite::SetAnimation(int iAnimNum)
{
	if ((unsigned int)iAnimNum >= m_vAnims.Size())
		m_iCurrentAnim = m_vAnims.Size() -1;
	else if (iAnimNum < 0)
		m_iCurrentAnim = 0;
	else
		m_iCurrentAnim = iAnimNum;

	SetTexRect(m_vAnims[m_iCurrentAnim]->Rect());
}
