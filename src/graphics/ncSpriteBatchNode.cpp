#include <cstdlib> // for exit()
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
	m_renderCmd.Material().SetAlwaysTransparent(true);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncSpriteBatchNode::Visit(ncRenderQueue& rRenderQueue)
{
	// early return if a node is invisible
	if (!bShouldDraw)
	{
		return;
	}

	Transform();

	// Clear any previous data before processing sprites
	m_vVertices.Clear();
	m_vTexCoords.Clear();
	m_vColors.Clear();

	// TODO: only the first level of children gets accounted
	for (ncList<ncSceneNode *>::Const_Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		if ((*i)->Type() == ncSprite::sType())
		{
			ncSprite *pSprite = static_cast<ncSprite *>((*i));

			if (pSprite->bShouldDraw && pSprite->Texture()->GLId() == m_pTexture->GLId())
			{
				pSprite->Transform();
				ProcessSprite(*pSprite);
			}
		}
	}

	Draw(rRenderQueue);
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

	float leftPos = size.x * scale * 0.5f;
	float rightPos = -size.x * scale * 0.5f;
	float bottomPos = -size.y * scale * 0.5f;
	float topPos = size.y * scale * 0.5f;

	float sine = 0.0f;
	float cosine = 1.0f;
	if (abs(rot) > ncSprite::sMinRotation && abs(rot) < 360.0f - ncSprite::sMinRotation)
	{
		sine = sinf(-rot * M_PI / 180.0f);
		cosine = cosf(-rot * M_PI / 180.0f);
	}

	float *pVertices = m_vVertices.MapBuffer(12);
	pVertices[0] = pos.x + leftPos * cosine - bottomPos * sine;			pVertices[1] = pos.y + bottomPos * cosine + leftPos * sine;
	pVertices[2] = pos.x + leftPos * cosine - topPos * sine;			pVertices[3] = pos.y + topPos * cosine + leftPos * sine;
	pVertices[4] = pos.x + rightPos * cosine - bottomPos * sine;		pVertices[5] = pos.y + bottomPos * cosine + rightPos * sine;

	pVertices[6] = pos.x + rightPos * cosine - bottomPos * sine;		pVertices[7] = pos.y + bottomPos * cosine + rightPos * sine;
	pVertices[8] = pos.x + rightPos * cosine - topPos * sine;			pVertices[9] = pos.y + topPos * cosine + rightPos * sine;
	pVertices[10] = pos.x + leftPos * cosine - topPos * sine;			pVertices[11] = pos.y + topPos * cosine + leftPos * sine;


	ncPoint texSize = m_pTexture->Size();
	ncRect texRect = rSprite.TexRect();

	float leftCoord = float(texRect.x) / float(texSize.x);
	float rightCoord = float(texRect.x + texRect.w) / float(texSize.x);
	float bottomCoord = float(texRect.y + texRect.h) / float(texSize.y);
	float topCoord = float(texRect.y) / float(texSize.y);

	float *pTexCoords = m_vTexCoords.MapBuffer(12);
	pTexCoords[0] = leftCoord;				pTexCoords[1] = bottomCoord;
	pTexCoords[2] = leftCoord;				pTexCoords[3] = topCoord;
	pTexCoords[4] = rightCoord;				pTexCoords[5] = bottomCoord;

	pTexCoords[6] = rightCoord;				pTexCoords[7] = bottomCoord;
	pTexCoords[8] = rightCoord;				pTexCoords[9] = topCoord;
	pTexCoords[10] = leftCoord;				pTexCoords[11] = topCoord;


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
	m_renderCmd.Geometry().SetData(GL_TRIANGLES, 0, m_vVertices.Size() / 2, m_vVertices.Pointer(), m_vTexCoords.Pointer(), m_vColors.Pointer());
	m_renderCmd.CalculateSortKey();

	ApplyTransformations();
}
