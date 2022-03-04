#include "Sprite.h"
#include "RenderCommand.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
Sprite::Sprite()
    : Sprite(nullptr, nullptr, 0.0f, 0.0f)
{
}

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
Sprite::Sprite(SceneNode *parent, Texture *texture)
    : Sprite(parent, texture, 0.0f, 0.0f)
{
}

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
Sprite::Sprite(Texture *texture)
    : Sprite(nullptr, texture, 0.0f, 0.0f)
{
}

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
Sprite::Sprite(SceneNode *parent, Texture *texture, float xx, float yy)
    : BaseSprite(parent, texture, xx, yy)
{
	init();
}

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
Sprite::Sprite(SceneNode *parent, Texture *texture, const Vector2f &position)
    : Sprite(parent, texture, position.x, position.y)
{
}

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
Sprite::Sprite(Texture *texture, float xx, float yy)
    : Sprite(nullptr, texture, xx, yy)
{
}

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
Sprite::Sprite(Texture *texture, const Vector2f &position)
    : Sprite(nullptr, texture, position.x, position.y)
{
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

Sprite::Sprite(const Sprite &other)
    : BaseSprite(other)
{
	init();
	setLayer(other.layer());
	setTexRect(other.texRect_);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Sprite::init()
{
	ZoneScoped;
	if (texture_ && texture_->name() != nullptr)
	{
		// When Tracy is disabled the statement body is empty and braces are needed
		ZoneText(texture_->name(), nctl::strnlen(texture_->name(), Object::MaxNameLength));
	}

	type_ = ObjectType::SPRITE;
	setLayer(DrawableNode::LayerBase::SCENE);
	renderCommand_->setType(RenderCommand::CommandTypes::SPRITE);

	const Material::ShaderProgramType shaderProgramType = [](Texture *texture)
	{
		if (texture)
			return (texture->numChannels() >= 3) ? Material::ShaderProgramType::SPRITE
			                                     : Material::ShaderProgramType::SPRITE_GRAY;
		else
			return Material::ShaderProgramType::SPRITE_NO_TEXTURE;
	}(texture_);
	renderCommand_->material().setShaderProgramType(shaderProgramType);
	spriteBlock_ = renderCommand_->material().uniformBlock("SpriteBlock");
	renderCommand_->geometry().setDrawParameters(GL_TRIANGLE_STRIP, 0, 4);

	if (texture_)
		setTexRect(Recti(0, 0, texture_->width(), texture_->height()));
}

void Sprite::textureHasChanged(Texture *newTexture)
{
	if (renderCommand_->material().shaderProgramType() != Material::ShaderProgramType::CUSTOM)
	{
		const Material::ShaderProgramType shaderProgramType = [](Texture *texture)
		{
			if (texture)
				return (texture->numChannels() >= 3) ? Material::ShaderProgramType::SPRITE
				                                     : Material::ShaderProgramType::SPRITE_GRAY;
			else
				return Material::ShaderProgramType::SPRITE_NO_TEXTURE;
		}(newTexture);
		const bool shaderHasChanged = renderCommand_->material().setShaderProgramType(shaderProgramType);
		if (shaderHasChanged)
		{
			spriteBlock_ = renderCommand_->material().uniformBlock("SpriteBlock");
			dirtyBits_.set(DirtyBitPositions::ColorBit);
		}
	}

	if (texture_ && newTexture && texture_ != newTexture)
	{
		Recti texRect = texRect_;
		texRect.x = (texRect.x / float(texture_->width())) * float(newTexture->width());
		texRect.y = (texRect.y / float(texture_->height())) * float(newTexture->width());
		texRect.w = (texRect.w / float(texture_->width())) * float(newTexture->width());
		texRect.h = (texRect.h / float(texture_->height())) * float(newTexture->width());
		setTexRect(texRect); // it also sets width_ and height_
	}
	else if (texture_ == nullptr && newTexture)
	{
		// Assigning a texture when there wasn't any
		setTexRect(Recti(0, 0, newTexture->width(), newTexture->height()));
	}
}

}
