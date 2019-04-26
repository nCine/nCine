#include "BaseSprite.h"
#include "RenderCommand.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
BaseSprite::BaseSprite(SceneNode *parent, Texture *texture, float xx, float yy)
    : DrawableNode(parent, xx, yy), texture_(texture), texRect_(0, 0, 0, 0), opaqueTexture_(false), spriteBlock_(nullptr)
{
}

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
BaseSprite::BaseSprite(SceneNode *parent, Texture *texture, const Vector2f &position)
    : BaseSprite(parent, texture, position.x, position.y)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void BaseSprite::setSize(float width, float height)
{
	width_ = width;
	height_ = height;
}

void BaseSprite::setSize(const Vector2f &size)
{
	width_ = size.x;
	height_ = size.y;
}

void BaseSprite::setTexRect(const Recti &rect)
{
	texRect_ = rect;
	height_ = static_cast<float>(rect.h);
	width_ = static_cast<float>(rect.w);
}

void BaseSprite::flipX()
{
	texRect_.x += texRect_.w;
	texRect_.w *= -1;
}

void BaseSprite::flipY()
{
	texRect_.y += texRect_.h;
	texRect_.h *= -1;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void BaseSprite::updateRenderCommand()
{
	renderCommand_->transformation() = worldMatrix_;
	renderCommand_->material().setTexture(*texture_);

	spriteBlock_->uniform("color")->setFloatVector(Colorf(absColor()).data());
	const bool isTransparent = absColor().a() < 255 || texture()->numChannels() == 1 ||
	                           (texture()->numChannels() == 4 && opaqueTexture_ == false);
	renderCommand_->material().setTransparent(isTransparent);

	const Vector2i texSize = texture_->size();
	const float texScaleX = texRect_.w / float(texSize.x);
	const float texBiasX = texRect_.x / float(texSize.x);
	const float texScaleY = texRect_.h / float(texSize.y);
	const float texBiasY = texRect_.y / float(texSize.y);

	spriteBlock_->uniform("texRect")->setFloatValue(texScaleX, texBiasX, texScaleY, texBiasY);
	spriteBlock_->uniform("spriteSize")->setFloatValue(width_, height_);
}

}
