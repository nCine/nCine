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

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
Sprite::Sprite(SceneNode* parent, Texture *texture)
	: DrawableNode(parent), texture_(texture), texRect_(0, 0, 0, 0)
{
	init();
}

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
Sprite::Sprite(Texture *texture)
	: DrawableNode(NULL), texture_(texture), texRect_(0, 0, 0, 0)
{
	init();
}

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
Sprite::Sprite(SceneNode* parent, Texture *texture, float x, float y)
	: DrawableNode(parent, x, y), texture_(texture), texRect_(0, 0, 0, 0)
{
	init();
}

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
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
	setLayer(DrawableNode::SCENE_LAYER);
	renderCommand_->setType(RenderCommand::SPRITE_TYPE);
	renderCommand_->material().setShaderProgram(Material::SPRITE_PROGRAM);
	renderCommand_->geometry().makeSharedQuad();

	if (texture_)
	{
		setTexRect(Recti(0, 0, texture_->width(), texture_->height()));
	}
}

/*! \todo Only the transformation matrix should be updated per frame */
void Sprite::updateRenderCommand()
{
	renderCommand_->transformation() = worldMatrix_;

	renderCommand_->material().setTexture(*texture_);

	renderCommand_->material().uniform("color")->setFloatValue(absColor().fR(), absColor().fG(), absColor().fB(), absColor().fA());
	bool isTransparent = absColor().a() < 255 || (texture()->hasAlpha() && opaqueTexture_ == false);
	renderCommand_->material().setTransparent(isTransparent);

	Vector2i texSize = texture_->size();
	float texScaleX = texRect_.w / float(texSize.x);
	float texBiasX = texRect_.x / float(texSize.x);
	float texScaleY = texRect_.h / float(texSize.y);
	float texBiasY = texRect_.y / float(texSize.y);
	renderCommand_->material().uniform("texRect")->setFloatValue(texScaleX, texBiasX, texScaleY, texBiasY);
	renderCommand_->material().uniform("spriteSize")->setFloatValue(width_, height_);
}

}
