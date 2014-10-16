#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include "apptest_overdraw.h"
#include "ncSceneNode.h"
#include "ncTexture.h"
#include "ncSprite.h"
#include "ncApplication.h"

ncIAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	ncIInputManager::setHandler(this);
	ncSceneNode &rRootNode = ncApplication::rootNode();

#ifdef __ANDROID__
	megaTexture_ = new ncTexture("/sdcard/ncine/megatexture_256.dds");
	alphaTexture_ = new ncTexture("/sdcard/ncine/transparent_128.png");
#else
	megaTexture_ = new ncTexture("textures/megatexture_256.png");
	alphaTexture_ = new ncTexture("textures/transparent_128.png");
#endif

	dummy_ = new ncSceneNode(&rRootNode, ncApplication::width() * 0.5f, ncApplication::height() * 0.5f);

	sprites_ = new ncSprite*[NumSprites];
	for (int i = 0; i < NumSprites; i++)
	{
		sprites_[i] = new ncSprite(dummy_, megaTexture_, 0, 0);
		sprites_[i]->setScale(1.35f);
		sprites_[i]->setPriority(ncDrawableNode::SCENE_PRIORITY + 5);
	}

	alphaSpriteBottom_ = new ncSprite(dummy_, alphaTexture_, 0, 150);
	alphaSpriteBottom_->setScale(2.0f);
	alphaSpriteBottom_->setPriority(ncDrawableNode::SCENE_PRIORITY + 1);
	alphaSpriteBottom_->setColor(0, 0, 255, 255);
	alphaSpriteTop_ = new ncSprite(dummy_, alphaTexture_, -0, -150);
	alphaSpriteTop_->setScale(2.0f);
	alphaSpriteTop_->setPriority(ncDrawableNode::SCENE_PRIORITY + 10);
	alphaSpriteTop_->setColor(255, 0, 0, 255);
}

void MyEventHandler::onShutdown()
{
	delete dummy_; // and all its children
	delete[] sprites_;
	delete megaTexture_;
	delete alphaTexture_;
}

#ifndef __ANDROID__
void MyEventHandler::onKeyReleased(const ncKeyboardEvent &event)
{
	if (event.sym == NCKEY_ESCAPE || event.sym == NCKEY_Q)
	{
		ncApplication::quit();
	}
}
#endif
