#include "ncAnimatedSprite.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncAnimatedSprite::~ncAnimatedSprite()
{
	for (int i = 0; i < m_vAnims.Size(); i++)
		delete m_vAnims[i];
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncAnimatedSprite::Update(unsigned long int ulInterval)
{
	int iPreviousFrame = m_vAnims[m_iCurrentAnim]->Frame();
	m_vAnims[m_iCurrentAnim]->UpdateFrame(ulInterval);

	// Updating sprite texture rectangle only on change
	if (iPreviousFrame != m_vAnims[m_iCurrentAnim]->Frame())
		SetTexRect(m_vAnims[m_iCurrentAnim]->Rect());

	ncSprite::Update(ulInterval);
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
	if (iAnimNum >= m_vAnims.Size())
		m_iCurrentAnim = m_vAnims.Size() -1;
	else if (iAnimNum < 0)
		m_iCurrentAnim = 0;
	else
		m_iCurrentAnim = iAnimNum;

	SetTexRect(m_vAnims[m_iCurrentAnim]->Rect());
}

ncAnimatedSprite* ncAnimatedSprite::FromId(unsigned int uId)
{
	ncObject *pObject = ncServiceLocator::Indexer().Object(uId);

	if(pObject)
	{
		if (pObject->Type() == sType())
			return static_cast<ncAnimatedSprite *>(pObject);
		else // Cannot cast
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, "ncAnimatedSprite::FromId - Object of wrong type");
			exit(-1);
		}
	}
	else // NULL
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, "ncAnimatedSprite::FromId - Object not found");
		return NULL;
	}
}
