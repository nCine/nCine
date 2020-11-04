#include "BaseSprite.h"
#include "RenderCommand.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
BaseSprite::BaseSprite(SceneNode *parent, Texture *texture, float xx, float yy)
    : DrawableNode(parent, xx, yy), texture_(texture), texRect_(0, 0, 0, 0),
      flippedX_(false), flippedY_(false), spriteBlock_(nullptr)
{
	renderCommand_->material().setBlendingEnabled(true);
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
	// Update anchor points when size changes
	if (anchorPoint_.x != 0.0f)
		anchorPoint_.x = (anchorPoint_.x / width_) * width;
	if (anchorPoint_.y != 0.0f)
		anchorPoint_.y = (anchorPoint_.y / height_) * height;

	width_ = width;
	height_ = height;
}

/*! This method should be also called if just the content of a texture changes */
void BaseSprite::setTexture(Texture *texture)
{
	if (texture)
	{
		// Allow self-assignment to take into account the case where the texture stays the same but it loads new data
		textureHasChanged(texture);
		texture_ = texture;
	}
}

void BaseSprite::setTexRect(const Recti &rect)
{
	texRect_ = rect;
	setSize(static_cast<float>(rect.w), static_cast<float>(rect.h));

	if (flippedX_)
	{
		texRect_.x += texRect_.w;
		texRect_.w *= -1;
	}

	if (flippedY_)
	{
		texRect_.y += texRect_.h;
		texRect_.h *= -1;
	}
}

void BaseSprite::setFlippedX(bool flippedX)
{
	if (flippedX_ != flippedX)
	{
		texRect_.x += texRect_.w;
		texRect_.w *= -1;
		flippedX_ = flippedX;
	}
}

void BaseSprite::setFlippedY(bool flippedY)
{
	if (flippedY_ != flippedY)
	{
		texRect_.y += texRect_.h;
		texRect_.h *= -1;
		flippedY_ = flippedY;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void BaseSprite::updateRenderCommand()
{
	renderCommand_->transformation() = worldMatrix_;
	renderCommand_->material().setTexture(*texture_);

	spriteBlock_->uniform("color")->setFloatVector(Colorf(absColor()).data());

	const Vector2i texSize = texture_->size();
	const float texScaleX = texRect_.w / float(texSize.x);
	const float texBiasX = texRect_.x / float(texSize.x);
	const float texScaleY = texRect_.h / float(texSize.y);
	const float texBiasY = texRect_.y / float(texSize.y);

	spriteBlock_->uniform("texRect")->setFloatValue(texScaleX, texBiasX, texScaleY, texBiasY);
	spriteBlock_->uniform("spriteSize")->setFloatValue(width_, height_);
}

}
