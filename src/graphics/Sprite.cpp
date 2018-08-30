#include "Sprite.h"
#include "RenderCommand.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

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
Sprite::Sprite(SceneNode *parent, Texture *texture, float x, float y)
	: BaseSprite(parent, texture, x, y)
{
	ASSERT(texture);

	type_ = ObjectType::SPRITE;
	setLayer(DrawableNode::LayerBase::SCENE);
	renderCommand_->setType(RenderCommand::CommandTypes::SPRITE);
	const Material::ShaderProgramType shaderProgramType = texture_->numChannels() == 4 ?
		Material::ShaderProgramType::SPRITE : Material::ShaderProgramType::SPRITE_GRAY;
	renderCommand_->material().setShaderProgramType(shaderProgramType);
	spriteBlock_ = renderCommand_->material().uniformBlock("SpriteBlock");
	renderCommand_->geometry().setDrawParameters(GL_TRIANGLE_STRIP, 0, 4);

	setTexRect(Recti(0, 0, texture_->width(), texture_->height()));
}

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
Sprite::Sprite(Texture *texture, float x, float y)
	: Sprite(nullptr, texture, x, y)
{

}

}
