#ifdef __ANDROID__
	#include <cstdlib> // for exit()
#endif
#include "ncSpriteBatchNode.h"
#include "ncServiceLocator.h"
#include "ncSprite.h"
#include "ncRenderQueue.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncSpriteBatchNode::ncSpriteBatchNode(ncSceneNode* pParent, ncTexture *pTexture)
	: ncDrawableNode(pParent, 0, 0), m_pTexture(pTexture), m_vVertices(16), m_vTexCoords(16)
{
	m_eType = SPRITEBATCH_TYPE;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncSpriteBatchNode::Visit(ncRenderQueue& rRenderQueue)
{
	// early return if a node is invisible
	if(!bShouldDraw)
		return;

	Transform();

	Draw(rRenderQueue);
}

void ncSpriteBatchNode::Draw(ncRenderQueue& rRenderQueue)
{
	// Clear every previous sprite data before drawing
	m_vVertices.Clear();
	m_vTexCoords.Clear();

	// TODO: only the first level of children gets accounted
	if (!m_children.isEmpty())
	{
		const ncListNode<ncSceneNode *> *pListNode = m_children.Head();

		while (pListNode)
		{
			// List nodes contain pointers, they need deferencing for visiting
			ncSceneNode& rChildNode = *(pListNode->Data());

			if (rChildNode.Type() == ncSprite::sType())
			{
				ncSprite& rSprite = static_cast<ncSprite &>(rChildNode);

				if (rSprite.bShouldDraw && rSprite.Texture()->GLId() == m_pTexture->GLId())
					ProcessSprite(rSprite);
			}
			pListNode = pListNode->Next();
		}
	}

	ncDrawableNode::Draw(rRenderQueue);
}

ncSpriteBatchNode* ncSpriteBatchNode::FromId(unsigned int uId)
{
	ncObject *pObject = ncServiceLocator::GetIndexer().Object(uId);

	if(pObject)
	{
		if (pObject->Type() == sType())
			return static_cast<ncSpriteBatchNode *>(pObject);
		else // Cannot cast
		{
			ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, "ncSpriteBatchNode::FromId - Object of wrong type");
			exit(-1);
		}
	}
	else // NULL
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, "ncSpriteBatchNode::FromId - Object not found");
		return NULL;
	}
}

void ncSpriteBatchNode::UpdateRenderCommand()
{
	m_renderCmd.Material().SetTextureGLId(m_pTexture->GLId());
	m_renderCmd.Transformation().SetPosition(AbsPosition());
	m_renderCmd.Geometry().SetData(GL_TRIANGLES, 0, m_vVertices.Size()/2, m_vVertices.Pointer(), m_vTexCoords.Pointer(), NULL);
	m_renderCmd.CalculateSortKey();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncSpriteBatchNode::ProcessSprite(ncSprite& rSprite)
{
	ncPoint size = rSprite.Size();
	ncPoint pos = rSprite.Position();
	// Parent transformation
	pos.x += m_absX;
	pos.y += m_absY;

	float leftPos = pos.x - size.x*0.5f;
	float rightPos = pos.x + size.x*0.5f;
	float bottomPos = pos.y - size.y*0.5f;
	float topPos = pos.y + size.y*0.5f;

	m_vVertices.InsertBack(leftPos);	m_vVertices.InsertBack(bottomPos);
	m_vVertices.InsertBack(leftPos);	m_vVertices.InsertBack(topPos);
	m_vVertices.InsertBack(rightPos);	m_vVertices.InsertBack(bottomPos);

	m_vVertices.InsertBack(rightPos);	m_vVertices.InsertBack(bottomPos);
	m_vVertices.InsertBack(rightPos);	m_vVertices.InsertBack(topPos);
	m_vVertices.InsertBack(leftPos);	m_vVertices.InsertBack(topPos);


	ncPoint texSize = m_pTexture->Size();
	ncRect texRect = rSprite.TexRect();

	float leftCoord = float(texRect.x)/float(texSize.x);
	float rightCoord = float(texRect.x+texRect.w)/float(texSize.x);
	float bottomCoord = float(texRect.y+texRect.h)/float(texSize.y);
	float topCoord = float(texRect.y)/float(texSize.y);

	m_vTexCoords.InsertBack(leftCoord);		m_vTexCoords.InsertBack(bottomCoord);
	m_vTexCoords.InsertBack(leftCoord);		m_vTexCoords.InsertBack(topCoord);
	m_vTexCoords.InsertBack(rightCoord);	m_vTexCoords.InsertBack(bottomCoord);

	m_vTexCoords.InsertBack(rightCoord);	m_vTexCoords.InsertBack(bottomCoord);
	m_vTexCoords.InsertBack(rightCoord);	m_vTexCoords.InsertBack(topCoord);
	m_vTexCoords.InsertBack(leftCoord);		m_vTexCoords.InsertBack(topCoord);
}
