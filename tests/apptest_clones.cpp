#include "apptest_clones.h"
#include <ncine/Application.h>
#include <ncine/Texture.h>
#include <ncine/AnimatedSprite.h>
#include <ncine/MeshSprite.h>
#include <ncine/TextNode.h>
#include <ncine/ParticleSystem.h>
#include <ncine/ParticleInitializer.h>
#include "apptest_datapath.h"

#ifdef __ANDROID__
	#include "AndroidApplication.h"
#endif

namespace {

#ifdef __ANDROID__
const char *SpriteTextureFile = "texture1_ETC2.ktx";
const char *AnimSpriteTextureFile = "spritesheet.webp";
const char *MeshSpriteTextureFile = "texture4_ETC2.ktx";
const char *FontTextureFile = "DroidSans32_256_ETC2.ktx";
const char *ParticleTextureFile = "smoke_256_ETC2.ktx";
#else
const char *SpriteTextureFile = "texture1.png";
const char *AnimSpriteTextureFile = "spritesheet.png";
const char *MeshSpriteTextureFile = "texture4.png";
const char *FontTextureFile = "DroidSans32_256.png";
const char *ParticleTextureFile = "smoke_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

const float PosY1 = 600.0f;
const float SpritePosX = 100.0f;
const float ClonedSpriteDistX = 125.0f;
const float AnimSpritePosX = 350.0f;
const float ClonedAnimSpriteDistX = 50.0f;
const float MeshSpritePosX = 500.0f;
const float ClonedMeshSpriteDistX = 110.0f;

const float PosY2 = 320.0f;
const float TextNodePosX = 600.0f;
const float ClonedTextNodeDistY = 100.0f;

const float PosY3 = 180.0f;
const float PartSystemPosX = 100.0f;
const float ClonedPartSystemDistX = 150.0f;

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	// Sprite setup and clone
	spriteTexture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", SpriteTextureFile)).data());
	sprite_ = nctl::makeUnique<nc::Sprite>(&rootNode, spriteTexture_.get(), SpritePosX, PosY1);
	sprite_->setScale(0.75f);
	sprite_->setRotation(180.0f);

	nc::Sprite toMoveSprite = sprite_->clone();
	clonedSprite_ = nctl::makeUnique<nc::Sprite>(nctl::move(toMoveSprite));
	clonedSprite_->move(ClonedSpriteDistX, 0.0f);

	// AnimatedSprite setup and clone
	animSpriteTexture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", AnimSpriteTextureFile)).data());
	animSprite_ = nctl::makeUnique<nc::AnimatedSprite>(&rootNode, animSpriteTexture_.get(), AnimSpritePosX, PosY1);
	animSprite_->setRotation(180.0f);

	nc::RectAnimation animation(0.12f, nc::RectAnimation::LoopMode::ENABLED, nc::RectAnimation::RewindMode::FROM_START);
	animation.addRect(0, 0, 48, 48);
	animation.addRect(48, 0, 48, 48);
	animation.addRect(96, 0, 48, 48);
	animation.addRect(144, 0, 48, 48);
	animation.addRect(0, 48, 48, 48);
	animation.addRect(48, 48, 48, 48);
	animation.addRect(96, 48, 48, 48);
	animation.addRect(144, 48, 48, 48);
	animSprite_->addAnimation(nctl::move(animation));

	animSprite_->setAnimationIndex(0);
	animSprite_->setFrame(0);
	animSprite_->setPaused(false);

	nc::AnimatedSprite toMoveAnimSprite = animSprite_->clone();
	clonedAnimSprite_ = nctl::makeUnique<nc::AnimatedSprite>(nctl::move(toMoveAnimSprite));
	clonedAnimSprite_->move(ClonedAnimSpriteDistX, 0.0f);

	// MeshSprite setup and clone
	meshSpriteTexture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", MeshSpriteTextureFile)).data());
	meshSprite_ = nctl::makeUnique<nc::MeshSprite>(&rootNode, meshSpriteTexture_.get(), MeshSpritePosX, PosY1);
	const nc::Vector2f texelPoints[4] = { { 20.0f, 20.0f }, { 107.0f, 20.0f }, { 20.0f, 107.0f }, { 107.0f, 107.0f } };
	meshSprite_->createVerticesFromTexels(4, texelPoints);

	nc::MeshSprite toMoveMeshSprite = meshSprite_->clone();
	clonedMeshSprite_ = nctl::makeUnique<nc::MeshSprite>(nctl::move(toMoveMeshSprite));
	clonedMeshSprite_->move(ClonedMeshSpriteDistX, 0.0f);

	// TextNode setup and clone
	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());
	textNode_ = nctl::makeUnique<nc::TextNode>(&rootNode, font_.get());
	textNode_->setString("This is a\nTextNode");
	textNode_->setPosition(TextNodePosX - textNode_->width(), PosY2 - textNode_->lineHeight());
	textNode_->setColor(255, 255, 0, 255);
	textNode_->setAlignment(nc::TextNode::Alignment::CENTER);

	nc::TextNode toMoveTextNode = textNode_->clone();
	clonedTextNode_ = nctl::makeUnique<nc::TextNode>(nctl::move(toMoveTextNode));
	clonedTextNode_->move(0.0f, ClonedTextNodeDistY);

	// Particle system setup and clone
	particleTexture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", ParticleTextureFile)).data());
	particleSystem_ = nctl::makeUnique<nc::ParticleSystem>(&rootNode, unsigned(NumParticles), particleTexture_.get(), particleTexture_->rect());
	particleSystem_->setPosition(PartSystemPosX, PosY3);

	nctl::UniquePtr<nc::ColorAffector> colAffector = nctl::makeUnique<nc::ColorAffector>();
	colAffector->addColorStep(0.0f, nc::Colorf(0.0f, 0.0f, 1.0f, 0.9f));
	colAffector->addColorStep(0.3f, nc::Colorf(0.86f, 0.7f, 0.0f, 0.65f));
	colAffector->addColorStep(0.35f, nc::Colorf(0.86f, 0.59f, 0.0f, 0.8f));
	colAffector->addColorStep(1.0f, nc::Colorf(0.86f, 0.39f, 0.0f, 0.75f));
	particleSystem_->addAffector(nctl::move(colAffector));
	nctl::UniquePtr<nc::SizeAffector> sizeAffector = nctl::makeUnique<nc::SizeAffector>(0.45f);
	sizeAffector->addSizeStep(0.0f, 0.4f);
	sizeAffector->addSizeStep(0.3f, 1.7f);
	sizeAffector->addSizeStep(1.0f, 0.01f);
	particleSystem_->addAffector(nctl::move(sizeAffector));

	nc::ParticleSystem toMoveParticleSystem = particleSystem_->clone();
	clonedParticleSystem_ = nctl::makeUnique<nc::ParticleSystem>(nctl::move(toMoveParticleSystem));
	clonedParticleSystem_->move(ClonedPartSystemDistX, 0.0f);

	lastEmissionTime_ = nc::TimeStamp::now();
}

void MyEventHandler::onFrameStart()
{
	if (lastEmissionTime_.secondsSince() > 0.085f)
	{
		nc::ParticleInitializer init;
		init.setAmount(3);
		init.setLife(0.85f, 1.0f);
		init.setPositionAndRadius(nc::Vector2f::Zero, 10.0f);
		init.setVelocityAndScale(0.0f, 350.0f, 0.8f, 1.0f);

		lastEmissionTime_ = nc::TimeStamp::now();
		particleSystem_->emitParticles(init);
		clonedParticleSystem_->emitParticles(init);
	}
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}
