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
	: ncDrawableNode(pParent, 0, 0), m_pTexture(pTexture), m_vVertices(16), m_vTexCoords(16), m_vColors(16)
{
	m_eType = SPRITEBATCH_TYPE;
	SetPriority(ncDrawableNode::SCENE_PRIORITY);
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
	m_vColors.Clear();

	// TODO: only the first level of children gets accounted
	ncList<ncSceneNode *>::Const_Iterator i = m_children.Begin();
	while(i != m_children.End())
	{
		if ((*i)->Type() == ncSprite::sType())
		{
			ncSprite *pSprite = static_cast<ncSprite *>((*i));

			if (pSprite->bShouldDraw && pSprite->Texture()->GLId() == m_pTexture->GLId())
				ProcessSprite(*pSprite);
		}
		i++;
	}

	ncDrawableNode::Draw(rRenderQueue);
}

ncSpriteBatchNode* ncSpriteBatchNode::FromId(unsigned int uId)
{
	ncObject *pObject = ncServiceLocator::Indexer().Object(uId);

	if(pObject)
	{
		if (pObject->Type() == sType())
			return static_cast<ncSpriteBatchNode *>(pObject);
		else // Cannot cast
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, "ncSpriteBatchNode::FromId - Object of wrong type");
			exit(-1);
		}
	}
	else // NULL
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, "ncSpriteBatchNode::FromId - Object not found");
		return NULL;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncSpriteBatchNode::ProcessSprite(ncSprite& rSprite)
{
	ncPoint size = rSprite.Size();
	ncVector2f pos = rSprite.Position();
	float rot = rSprite.Rotation();

	// Parent transformation and sprite relative position
	float X = m_absX + pos.x;
	float Y = m_absY + pos.y;

	float leftPos = size.x*0.5f;
	float rightPos = -size.x*0.5f;
	float bottomPos = -size.y*0.5f;
	float topPos = size.y*0.5f;

	if (rot > ncSprite::sMinRotation && rot < 360.0f - ncSprite::sMinRotation)
	{
		float sine = sinf(-rot * M_PI/180.0f);
		float cosine = 1 - sine*sine;

		m_vVertices.InsertBack(X + leftPos*cosine - bottomPos*sine);	m_vVertices.InsertBack(Y + bottomPos*cosine + leftPos*sine);
		m_vVertices.InsertBack(X + leftPos*cosine - topPos*sine);		m_vVertices.InsertBack(Y + topPos*cosine + leftPos*sine);
		m_vVertices.InsertBack(X + rightPos*cosine - bottomPos*sine);	m_vVertices.InsertBack(Y + bottomPos*cosine + rightPos*sine);

		m_vVertices.InsertBack(X + rightPos*cosine - bottomPos*sine);	m_vVertices.InsertBack(Y + bottomPos*cosine + rightPos*sine);
		m_vVertices.InsertBack(X + rightPos*cosine - topPos*sine);		m_vVertices.InsertBack(Y + topPos*cosine + rightPos*sine);
		m_vVertices.InsertBack(X + leftPos*cosine - topPos*sine);		m_vVertices.InsertBack(Y + topPos*cosine + leftPos*sine);
	}
	else
	{
		m_vVertices.InsertBack(X + leftPos);	m_vVertices.InsertBack(Y + bottomPos);
		m_vVertices.InsertBack(X + leftPos);	m_vVertices.InsertBack(Y + topPos);
		m_vVertices.InsertBack(X + rightPos);	m_vVertices.InsertBack(Y + bottomPos);

		m_vVertices.InsertBack(X + rightPos);	m_vVertices.InsertBack(Y + bottomPos);
		m_vVertices.InsertBack(X + rightPos);	m_vVertices.InsertBack(Y + topPos);
		m_vVertices.InsertBack(X + leftPos);	m_vVertices.InsertBack(Y + topPos);
	}


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


	m_vColors.Append(rSprite.Color().Vector(), 4);
	m_vColors.Append(rSprite.Color().Vector(), 4);
	m_vColors.Append(rSprite.Color().Vector(), 4);

	m_vColors.Append(rSprite.Color().Vector(), 4);
	m_vColors.Append(rSprite.Color().Vector(), 4);
	m_vColors.Append(rSprite.Color().Vector(), 4);
}

void ncSpriteBatchNode::UpdateRenderCommand()
{
	m_renderCmd.Material().SetTextureGLId(m_pTexture->GLId());
	m_renderCmd.Transformation().SetPosition(AbsPosition().x, AbsPosition().y);
	m_renderCmd.Geometry().SetData(GL_TRIANGLES, 0, m_vVertices.Size()/2, m_vVertices.Pointer(), m_vTexCoords.Pointer(), m_vColors.Pointer());
	m_renderCmd.CalculateSortKey();
}
