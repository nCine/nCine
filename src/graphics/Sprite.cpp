#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include <cstdlib> // for exit()
#include "Sprite.h"
#include "ServiceLocator.h"
#include "RenderQueue.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Sprite::Sprite(SceneNode* parent, Texture *texture)
	: DrawableNode(parent), texture_(texture), texRect_(0, 0, 0, 0)
{
	init();
}

Sprite::Sprite(Texture *texture)
	: DrawableNode(NULL), texture_(texture), texRect_(0, 0, 0, 0)
{
	init();
}

Sprite::Sprite(SceneNode* parent, Texture *texture, float x, float y)
	: DrawableNode(parent, x, y), texture_(texture), texRect_(0, 0, 0, 0)
{
	init();
}

Sprite::Sprite(Texture *texture, float x, float y)
	: DrawableNode(NULL, x, y), texture_(texture), texRect_(0, 0, 0, 0)
{
	init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Sprite::init()
{
	type_ = SPRITE_TYPE;
	setPriority(DrawableNode::SCENE_PRIORITY);
	renderCommand_->setType(RenderCommand::SPRITE_TYPE);

	if (texture_)
	{
		setTexRect(Rect(0, 0, texture_->width(), texture_->height()));
	}
}

void Sprite::setVertices()
{
	vertices_[0] = width_ * 0.5f;		vertices_[1] = -height_ * 0.5f;
	vertices_[2] = width_ * 0.5f;		vertices_[3] = height_ * 0.5f;
	vertices_[4] = -width_ * 0.5f;		vertices_[5] = -height_ * 0.5f;

	vertices_[6] = -width_ * 0.5f;		vertices_[7] = height_ * 0.5f;
}

void Sprite::setTexCoords()
{
	Point texSize = texture_->size();
	float leftCoord = float(texRect_.x) / float(texSize.x);
	float rightCoord = float(texRect_.x + texRect_.w) / float(texSize.x);
	float bottomCoord = float(texRect_.y + texRect_.h) / float(texSize.y);
	float topCoord = float(texRect_.y) / float(texSize.y);

	texCoords_[0] = leftCoord;		texCoords_[1] = bottomCoord;
	texCoords_[2] = leftCoord;		texCoords_[3] = topCoord;
	texCoords_[4] = rightCoord;		texCoords_[5] = bottomCoord;

	texCoords_[6] = rightCoord;		texCoords_[7] = topCoord;
}

void Sprite::updateRenderCommand()
{
	renderCommand_->material().setTextureGLId(texture_->glId());
	renderCommand_->material().setColor(absColor_);
	renderCommand_->transformation().setPosition(absPosition().x, absPosition().y);
	setVertices();
	renderCommand_->geometry().setData(GL_TRIANGLE_STRIP, 0, 4, vertices_.data(), texCoords_.data(), NULL);
	renderCommand_->calculateSortKey();

	applyTransformations();
}

}
