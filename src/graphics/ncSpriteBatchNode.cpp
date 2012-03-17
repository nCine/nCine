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
	m_renderCmd.SetType(ncRenderCommand::SPRITE_TYPE);
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

	// TODO: only the first level of children gets accounted
	for(ncList<ncSceneNode *>::Const_Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		if ((*i)->Type() == ncSprite::sType())
		{
			ncSprite *pSprite = static_cast<ncSprite *>((*i));

			if (pSprite->bShouldDraw && pSprite->Texture()->GLId() == m_pTexture->GLId())
				pSprite->Transform();
		}
	}

	Draw(rRenderQueue);
}

void ncSpriteBatchNode::Draw(ncRenderQueue& rRenderQueue)
{
	// Clear every previous sprite data before drawing
	m_vVertices.Clear();
	m_vTexCoords.Clear();
	m_vColors.Clear();

	// TODO: only the first level of children gets accounted
	for(ncList<ncSceneNode *>::Const_Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		if ((*i)->Type() == ncSprite::sType())
		{
			ncSprite *pSprite = static_cast<ncSprite *>((*i));

			if (pSprite->bShouldDraw && pSprite->Texture()->GLId() == m_pTexture->GLId())
				ProcessSprite(*pSprite);
		}
	}

	ncDrawableNode::Draw(rRenderQueue);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncSpriteBatchNode::ProcessSprite(ncSprite& rSprite)
{
	ncPoint size = rSprite.Size();
	ncVector2f pos = rSprite.Position();
	float rot = rSprite.Rotation();
	float scale = rSprite.Scale();

	float leftPos = size.x*scale*0.5f;
	float rightPos = -size.x*scale*0.5f;
	float bottomPos = -size.y*scale*0.5f;
	float topPos = size.y*scale*0.5f;

	float sine = 0.0f;
	float cosine = 1.0f;
	if (abs(rot) > ncSprite::sMinRotation && abs(rot) < 360.0f - ncSprite::sMinRotation)
	{
		sine = sinf(-rot * M_PI/180.0f);
		cosine = cosf(-rot * M_PI/180.0f);
	}

	m_vVertices.InsertBack(pos.x + leftPos*cosine - bottomPos*sine);	m_vVertices.InsertBack(pos.y + bottomPos*cosine + leftPos*sine);
	m_vVertices.InsertBack(pos.x + leftPos*cosine - topPos*sine);		m_vVertices.InsertBack(pos.y + topPos*cosine + leftPos*sine);
	m_vVertices.InsertBack(pos.x + rightPos*cosine - bottomPos*sine);	m_vVertices.InsertBack(pos.y + bottomPos*cosine + rightPos*sine);

	m_vVertices.InsertBack(pos.x + rightPos*cosine - bottomPos*sine);	m_vVertices.InsertBack(pos.y + bottomPos*cosine + rightPos*sine);
	m_vVertices.InsertBack(pos.x + rightPos*cosine - topPos*sine);		m_vVertices.InsertBack(pos.y + topPos*cosine + rightPos*sine);
	m_vVertices.InsertBack(pos.x + leftPos*cosine - topPos*sine);		m_vVertices.InsertBack(pos.y + topPos*cosine + leftPos*sine);


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

	ApplyTransformations();
}
