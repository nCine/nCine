#ifdef __ANDROID__
	#include <cstdlib> // for exit()
#endif
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
