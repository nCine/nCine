#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include "apptest_rotozoom.h"
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

	pause_ = false;
	angle_ = 0.0f;
	ncRect texRects[NumTextures];
	sprites_ = new ncSprite*[NumRowSprites * NumColSprites];

	ncSceneNode &rootNode = ncApplication::rootNode();

#ifdef __ANDROID__
	megaTexture_ = new ncTexture("/sdcard/ncine/megatexture_256.dds");
#else
	megaTexture_ = new ncTexture("textures/megatexture_256.png");
#endif
	texRects[0] = ncRect(0, 0, 145, 121);
	texRects[1] = ncRect(256 - 100, 0, 100, 100);
	texRects[2] = ncRect(0, 256 - 96, 96, 96);
	texRects[3] = ncRect(256 - 96, 256 - 96, 96, 96);

	dummy_ = new ncSceneNode(&rootNode, ncApplication::width() * 0.5f, ncApplication::height() * 0.5f);
	dummy_->setScale(0.75f);

	int index = 0;
	for (int i = -NumRowSprites * 0.5f; i < NumRowSprites * 0.5f; i++)
	{
		for (int j = -NumColSprites * 0.5f; j < NumColSprites * 0.5f; j++)
		{
			sprites_[index] = new ncSprite(dummy_, megaTexture_, i * 100, j * 100);
			sprites_[index]->setTexRect(texRects[index % NumTextures]);
			sprites_[index]->setScale(0.75f);
			index++;
		}
	}
}

void MyEventHandler::onFrameStart()
{
	if (pause_ == false)
	{
		angle_ += 2.5f * ncApplication::interval();
	}
	float sine = sinf(angle_);
	float cosine = cosf(angle_);

	dummy_->x = ncApplication::width() * 0.5f + sine * 100.0f;
	dummy_->y = ncApplication::height() * 0.5f + cosine * 150.0f;
	dummy_->setRotation(angle_ * 8.0f);
	dummy_->setScale(((sine * 0.15f) + 1.0f) * 0.5f);

	for (int i = 0; i < NumColSprites * NumRowSprites; i++)
	{
		sprites_[i]->setRotation(-angle_ * 8.0f);
	}

	ncApplication::rootNode().x = (sine + 1.0f) * 50.0f;
	ncApplication::rootNode().y = (cosine + 1.0f) * 50.0f;
	ncApplication::rootNode().setRotation(sine * 10.0f);
	ncApplication::rootNode().setScale(((cosine * 0.1f) + 1.0f) * 0.75f);
}

void MyEventHandler::onShutdown()
{
	delete dummy_; // and all its children
	delete[] sprites_;
	delete megaTexture_;
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const ncTouchEvent &event)
{
	pause_ = true;
}

void MyEventHandler::onTouchUp(const ncTouchEvent &event)
{
	pause_ = false;
}
#else
void MyEventHandler::onMouseButtonPressed(const ncMouseEvent &event)
{
	if (event.isLeftButton())
	{
		pause_ = true;
	}
}

void MyEventHandler::onMouseButtonReleased(const ncMouseEvent &event)
{
	if (event.isLeftButton())
	{
		pause_ = false;
	}
}

void MyEventHandler::onKeyReleased(const ncKeyboardEvent &event)
{
	if (event.sym == NCKEY_ESCAPE || event.sym == NCKEY_Q)
	{
		ncApplication::quit();
	}
	else if (event.sym == NCKEY_SPACE)
	{
		ncApplication::togglePause();
	}
}
#endif
