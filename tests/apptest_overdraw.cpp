#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include "apptest_overdraw.h"
#include "Application.h"
#include "SceneNode.h"
#include "Texture.h"
#include "Sprite.h"
#include "IFile.h" // for dataPath()

nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	nc::IInputManager::setHandler(this);
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

#ifdef __ANDROID__
	opaqueTexture_ = new nc::Texture((nc::IFile::dataPath() + "texture3_explicit.dds").data());
	alphaTestedTexture_ = new nc::Texture((nc::IFile::dataPath() + "texture1_explicit.dds").data());
	transparentTexture_ = new nc::Texture((nc::IFile::dataPath() + "smoke_128_explicit.dds").data());
	alphaTexture_ = new nc::Texture((nc::IFile::dataPath() + "transparent_128_8888.pvr").data());
#else
	opaqueTexture_ = new nc::Texture((nc::IFile::dataPath() + "textures/texture3.png").data());
	alphaTestedTexture_ = new nc::Texture((nc::IFile::dataPath() + "textures/texture1.png").data());
	transparentTexture_ = new nc::Texture((nc::IFile::dataPath() + "textures/smoke_128.png").data());
	alphaTexture_ = new nc::Texture((nc::IFile::dataPath() + "textures/transparent_128.png").data());
#endif

	dummy_ = new nc::SceneNode(&rootNode, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);

	for (unsigned int i = 0; i < NumSprites; i++)
	{
		opaqueSprites_[i] = new nc::Sprite(dummy_, opaqueTexture_, -50, 200);
		opaqueSprites_[i]->setScale(1.25f);
		opaqueSprites_[i]->setPriority(nc::DrawableNode::SCENE_PRIORITY + 5);
		alphaTestedSprites_[i] = new nc::Sprite(dummy_, alphaTestedTexture_, 50, -200);
		alphaTestedSprites_[i]->setScale(1.25f);
		alphaTestedSprites_[i]->setPriority(nc::DrawableNode::SCENE_PRIORITY + 5);
		transparentSprites_[i] = new nc::Sprite(dummy_, transparentTexture_, 0, 0);
		transparentSprites_[i]->setScale(1.25f);
		transparentSprites_[i]->setPriority(nc::DrawableNode::SCENE_PRIORITY + 5);
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
	delete opaqueTexture_;
	delete alphaTestedTexture_;
	delete transparentTexture_;
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
