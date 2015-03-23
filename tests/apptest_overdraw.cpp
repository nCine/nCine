#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include "apptest_overdraw.h"
#include "SceneNode.h"
#include "Texture.h"
#include "Sprite.h"

#ifdef __ANDROID__
	#include "AndroidApplication.h"
#else
	#include "Application.h"
#endif

nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	nc::IInputManager::setHandler(this);
	nc::SceneNode &rRootNode = nc::theApplication().rootNode();

#ifdef __ANDROID__
	megaTexture_ = new nc::Texture("/sdcard/ncine/megatexture_256.dds");
	alphaTexture_ = new nc::Texture("/sdcard/ncine/transparent_128.png");
#else
	megaTexture_ = new nc::Texture("textures/megatexture_256.png");
	alphaTexture_ = new nc::Texture("textures/transparent_128.png");
#endif

	dummy_ = new nc::SceneNode(&rRootNode, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);

	for (int i = 0; i < NumSprites; i++)
	{
		sprites_[i] = new nc::Sprite(dummy_, megaTexture_, 0, 0);
		sprites_[i]->setScale(1.35f);
		sprites_[i]->setPriority(nc::DrawableNode::SCENE_PRIORITY + 5);
	}

	alphaSpriteBottom_ = new nc::Sprite(dummy_, alphaTexture_, 0, 150);
	alphaSpriteBottom_->setScale(2.0f);
	alphaSpriteBottom_->setPriority(nc::DrawableNode::SCENE_PRIORITY + 1);
	alphaSpriteBottom_->setColor(0, 0, 255, 255);
	alphaSpriteTop_ = new nc::Sprite(dummy_, alphaTexture_, -0, -150);
	alphaSpriteTop_->setScale(2.0f);
	alphaSpriteTop_->setPriority(nc::DrawableNode::SCENE_PRIORITY + 10);
	alphaSpriteTop_->setColor(255, 0, 0, 255);
}

void MyEventHandler::onShutdown()
{
	delete dummy_; // and all its children (the sprites)
	delete megaTexture_;
	delete alphaTexture_;
}

#ifndef __ANDROID__
void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KEY_ESCAPE || event.sym == nc::KEY_Q)
	{
		nc::theApplication().quit();
	}
}
#endif
