#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include "apptest_mipmapping.h"
#include "ncSceneNode.h"
#include "ncTexture.h"
#include "ncSprite.h"
#include "ncApplication.h"

static const unsigned int maxDivider = 16;

ncIAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	ncIInputManager::setHandler(this);

	pause_ = false;

	ncSceneNode &rRootNode = ncApplication::rootNode();

#ifdef __ANDROID__
	texture_ = new ncTexture("/sdcard/ncine/checker_256_ETC1_MIP.ktx");
//	texture_ = new ncTexture("/sdcard/ncine/megatexture_256.dds");
#else
//	texture_ = new ncTexture("textures/checker_256_MIP.pvr");
//	texture_ = new ncTexture("textures/compressed/checker_256_MIP.dds");
	texture_ = new ncTexture("textures/compressed/checker_256_DXT1_MIP.ktx");
//	texture_ = new ncTexture("textures/checker_256.png");
#endif

	dummy_ = new ncSceneNode(&rRootNode, ncApplication::width() * 0.5f, ncApplication::height() * 0.5f);
	divider_ = 1;

	sprite_ = new ncSprite(dummy_, texture_);
}

void MyEventHandler::onFrameStart()
{

}

void MyEventHandler::onShutdown()
{
	delete dummy_; // and all its children
	delete texture_;
}

#ifdef __ANDROID__
void MyEventHandler::onTouchUp(const ncTouchEvent &event)
{
	divider_ *= 2;
	if (divider_ > maxDivider)
	{
		divider_ = 1;
	}

	sprite_->setScale(1.0f / divider_);
}
#else
void MyEventHandler::onMouseButtonReleased(const ncMouseEvent &event)
{
	if (event.isLeftButton())
	{
		divider_ *= 2;
		if (divider_ > maxDivider)
		{
			divider_ = 1;
		}
	}
	else if (event.isRightButton())
	{
		divider_ /= 2;
		if (divider_ < 1)
		{
			divider_ = maxDivider;
		}
	}

	sprite_->setScale(1.0f / divider_);
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
