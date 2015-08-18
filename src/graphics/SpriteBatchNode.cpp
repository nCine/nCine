#include <cstdlib> // for exit()
#include "SpriteBatchNode.h"
#include "ServiceLocator.h"
#include "Sprite.h"
#include "RenderQueue.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

SpriteBatchNode::SpriteBatchNode(SceneNode* parent, Texture *texture)
	: DrawableNode(parent, 0, 0), texture_(texture), vertices_(16), texCoords_(16), colors_(16)
{
	type_ = SPRITEBATCH_TYPE;
	setPriority(DrawableNode::SCENE_PRIORITY);
	renderCommand_->setType(RenderCommand::SPRITE_TYPE);
	renderCommand_->material().setTransparent(true);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void SpriteBatchNode::visit(RenderQueue& renderQueue)
{
	// early return if a node is invisible
	if (!shouldDraw_)
	{
		return;
	}

	transform();

	// Clear any previous data before processing sprites
	vertices_.clear();
	texCoords_.clear();
	colors_.clear();

	// TODO: only the first level of children gets accounted
	for (List<SceneNode *>::Const_Iterator i = children_.begin(); i != children_.end(); ++i)
	{
		if ((*i)->type() == Sprite::sType())
		{
/*
			Sprite *sprite = static_cast<Sprite *>((*i));

			if (sprite->shouldDraw_ && sprite->texture()->glId() == texture_->glId())
			{
				sprite->transform();
				processSprite(*sprite);
			}
*/
		}
	}

	draw(renderQueue);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void SpriteBatchNode::processSprite(Sprite& sprite)
{
	Point size = sprite.size();
	Vector2f pos = sprite.position();
	float rot = sprite.rotation();
	float scale = sprite.scale();

	float leftPos = size.x * scale * 0.5f;
	float rightPos = -size.x * scale * 0.5f;
	float bottomPos = -size.y * scale * 0.5f;
	float topPos = size.y * scale * 0.5f;

	float sine = 0.0f;
	float cosine = 1.0f;
	if (fabs(rot) > Sprite::MinRotation && fabs(rot) < 360.0f - Sprite::MinRotation)
	{
		sine = sinf(-rot * M_PI / 180.0f);
		cosine = cosf(-rot * M_PI / 180.0f);
	}

	unsigned int oldSize = vertices_.size();
	vertices_.setSize(oldSize + 12);
	float *vertices = vertices_.data() + oldSize;
	vertices[0] = pos.x + leftPos * cosine - bottomPos * sine;			vertices[1] = pos.y + bottomPos * cosine + leftPos * sine;
	vertices[2] = pos.x + leftPos * cosine - topPos * sine;				vertices[3] = pos.y + topPos * cosine + leftPos * sine;
	vertices[4] = pos.x + rightPos * cosine - bottomPos * sine;			vertices[5] = pos.y + bottomPos * cosine + rightPos * sine;

	vertices[6] = pos.x + rightPos * cosine - bottomPos * sine;			vertices[7] = pos.y + bottomPos * cosine + rightPos * sine;
	vertices[8] = pos.x + rightPos * cosine - topPos * sine;			vertices[9] = pos.y + topPos * cosine + rightPos * sine;
	vertices[10] = pos.x + leftPos * cosine - topPos * sine;			vertices[11] = pos.y + topPos * cosine + leftPos * sine;


	Point texSize = texture_->size();
	Rect texRect = sprite.texRect();

	float leftCoord = float(texRect.x) / float(texSize.x);
	float rightCoord = float(texRect.x + texRect.w) / float(texSize.x);
	float bottomCoord = float(texRect.y + texRect.h) / float(texSize.y);
	float topCoord = float(texRect.y) / float(texSize.y);

	oldSize = texCoords_.size();
	texCoords_.setSize(oldSize + 12);
	float *texCoords = texCoords_.data() + oldSize;
	texCoords[0] = leftCoord;				texCoords[1] = bottomCoord;
	texCoords[2] = leftCoord;				texCoords[3] = topCoord;
	texCoords[4] = rightCoord;				texCoords[5] = bottomCoord;

	texCoords[6] = rightCoord;				texCoords[7] = bottomCoord;
	texCoords[8] = rightCoord;				texCoords[9] = topCoord;
	texCoords[10] = leftCoord;				texCoords[11] = topCoord;


	colors_.append(sprite.color().vector(), 4);
	colors_.append(sprite.color().vector(), 4);
	colors_.append(sprite.color().vector(), 4);

	colors_.append(sprite.color().vector(), 4);
	colors_.append(sprite.color().vector(), 4);
	colors_.append(sprite.color().vector(), 4);
}

void SpriteBatchNode::updateRenderCommand()
{
/*
	renderCommand_->material().setTextureGLId(texture_->glId());
	renderCommand_->transformation().setPosition(absPosition().x, absPosition().y);
	renderCommand_->geometry().setData(GL_TRIANGLES, 0, vertices_.size() / 2, vertices_.data(), texCoords_.data(), colors_.data());

	applyTransformations();
*/
}

}
