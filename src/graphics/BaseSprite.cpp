#include "BaseSprite.h"
#include "RenderCommand.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

BaseSprite::BaseSprite(SceneNode *parent, Texture *texture, float xx, float yy)
    : DrawableNode(parent, xx, yy), texture_(texture), texRect_(0, 0, 0, 0),
      flippedX_(false), flippedY_(false), instanceBlock_(nullptr)
{
	renderCommand_->material().setBlendingEnabled(true);
}

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
	dirtyBits_.set(DirtyBitPositions::SizeBit);
	dirtyBits_.set(DirtyBitPositions::AabbBit);
}

/*! \note If you set a texture that is already assigned, this method would be equivalent to `resetTexture()` */
void BaseSprite::setTexture(Texture *texture)
{
	// Allow self-assignment to take into account the case where the texture stays the same but it loads new data
	textureHasChanged(texture);
	texture_ = texture;
	dirtyBits_.set(DirtyBitPositions::TextureBit);
}

/*! \note Use this method when the content of the currently assigned texture changes */
void BaseSprite::resetTexture()
{
	textureHasChanged(texture_);
	dirtyBits_.set(DirtyBitPositions::TextureBit);
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

	dirtyBits_.set(DirtyBitPositions::TextureBit);
}

void BaseSprite::setFlippedX(bool flippedX)
{
	if (flippedX_ != flippedX)
	{
		texRect_.x += texRect_.w;
		texRect_.w *= -1;
		flippedX_ = flippedX;

		dirtyBits_.set(DirtyBitPositions::TextureBit);
	}
}

void BaseSprite::setFlippedY(bool flippedY)
{
	if (flippedY_ != flippedY)
	{
		texRect_.y += texRect_.h;
		texRect_.h *= -1;
		flippedY_ = flippedY;

		dirtyBits_.set(DirtyBitPositions::TextureBit);
	}
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

BaseSprite::BaseSprite(const BaseSprite &other)
    : DrawableNode(other), texture_(other.texture_), texRect_(other.texRect_),
      flippedX_(other.flippedX_), flippedY_(other.flippedY_), instanceBlock_(nullptr)
{
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void BaseSprite::shaderHasChanged()
{
	renderCommand_->material().reserveUniformsDataMemory();
	instanceBlock_ = renderCommand_->material().uniformBlock(Material::InstanceBlockName);
	GLUniformCache *textureUniform = renderCommand_->material().uniform(Material::TextureUniformName);
	if (textureUniform && textureUniform->intValue(0) != 0)
		textureUniform->setIntValue(0); // GL_TEXTURE0

	dirtyBits_.set(DirtyBitPositions::TransformationBit);
	dirtyBits_.set(DirtyBitPositions::ColorBit);
	dirtyBits_.set(DirtyBitPositions::SizeBit);
	dirtyBits_.set(DirtyBitPositions::TextureBit);
}

void BaseSprite::updateRenderCommand()
{
	ZoneScoped;

	if (dirtyBits_.test(DirtyBitPositions::TransformationUploadBit))
		renderCommand_->setTransformation(worldMatrix_);

	if (dirtyBits_.test(DirtyBitPositions::ColorUploadBit))
	{
		GLUniformCache *colorUniform = instanceBlock_->uniform(Material::ColorUniformName);
		if (colorUniform)
			colorUniform->setFloatVector(Colorf(absColor()).data());
	}
	if (dirtyBits_.test(DirtyBitPositions::SizeBit))
	{
		GLUniformCache *spriteSizeUniform = instanceBlock_->uniform(Material::SpriteSizeUniformName);
		if (spriteSizeUniform)
			spriteSizeUniform->setFloatValue(width_, height_);
		dirtyBits_.reset(DirtyBitPositions::SizeBit);
	}

	if (dirtyBits_.test(DirtyBitPositions::TextureBit))
	{
		if (texture_)
		{
			renderCommand_->material().setTexture(*texture_);

			GLUniformCache *texRectUniform = instanceBlock_->uniform(Material::TexRectUniformName);
			if (texRectUniform)
			{
				const Vector2i texSize = texture_->size();
				const float texScaleX = texRect_.w / float(texSize.x);
				const float texBiasX = texRect_.x / float(texSize.x);
				const float texScaleY = texRect_.h / float(texSize.y);
				const float texBiasY = texRect_.y / float(texSize.y);

				texRectUniform->setFloatValue(texScaleX, texBiasX, texScaleY, texBiasY);
			}
		}
		else
			renderCommand_->material().setTexture(nullptr);

		dirtyBits_.reset(DirtyBitPositions::TextureBit);
	}
}

}
