#include "Sprite.h"
#include "RenderQueue.h"

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
	: DrawableNode(parent, x, y), texture_(texture), texRect_(0, 0, 0, 0), opaqueTexture_(false), spriteBlock_(nullptr)
{
	ASSERT(texture);

	type_ = ObjectType::SPRITE;
	setLayer(DrawableNode::LayerBase::SCENE);
	renderCommand_->setType(RenderCommand::CommandTypes::SPRITE);
	renderCommand_->material().setShaderProgramType(Material::ShaderProgramType::SPRITE);
	spriteBlock_ = renderCommand_->material().uniformBlock("SpriteBlock");
	renderCommand_->geometry().setDrawParameters(GL_TRIANGLE_STRIP, 0, 4);

	setTexRect(Recti(0, 0, texture_->width(), texture_->height()));
}

/*! \note The initial layer value for a sprite is `DrawableNode::SCENE_LAYER` */
Sprite::Sprite(Texture *texture, float x, float y)
	: Sprite(nullptr, texture, x, y)
{

}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Sprite::updateRenderCommand()
{
	renderCommand_->transformation() = worldMatrix_;
	renderCommand_->material().setTexture(*texture_);

	spriteBlock_->uniform("color")->setFloatValue(absColor().fR(), absColor().fG(), absColor().fB(), absColor().fA());
	const bool isTransparent = absColor().a() < 255 || (texture()->hasAlpha() && opaqueTexture_ == false);
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
