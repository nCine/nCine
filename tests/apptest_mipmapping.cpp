#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include "apptest_mipmapping.h"
#include "SceneNode.h"
#include "Texture.h"
#include "Sprite.h"
#include "IFile.h" // for dataPath()

#ifdef __ANDROID__
	#include "AndroidApplication.h"
#else
	#include "Application.h"
#endif

static const unsigned int maxDivider = 16;

nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	nc::IInputManager::setHandler(this);

	pause_ = false;

	nc::SceneNode &rootNode = nc::theApplication().rootNode();

#ifdef __ANDROID__
	texture_ = new nc::Texture((nc::IFile::dataPath() + "checker_256_ETC1_MIP.ktx").data());
//	texture_ = new nc::Texture((nc::IFile::dataPath() + "megatexture_256.dds").data());
#else
//	texture_ = new nc::Texture((nc::IFile::dataPath() + "textures/checker_256_MIP.pvr").data());
//	texture_ = new nc::Texture((nc::IFile::dataPath() + "textures/compressed/checker_256_MIP.dds").data());
	texture_ = new nc::Texture((nc::IFile::dataPath() + "textures/compressed/checker_256_DXT1_MIP.ktx").data());
//	texture_ = new nc::Texture((nc::IFile::dataPath() + "textures/checker_256.png").data());
#endif

	dummy_ = new nc::SceneNode(&rootNode, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	divider_ = 1;

	sprite_ = new nc::Sprite(dummy_, texture_);
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
void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	divider_ *= 2;
	if (divider_ > maxDivider)
	{
		divider_ = 1;
	}

	sprite_->setScale(1.0f / divider_);
}
#else
void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
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

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KEY_ESCAPE || event.sym == nc::KEY_Q)
	{
		nc::theApplication().quit();
	}
	else if (event.sym == nc::KEY_SPACE)
	{
		nc::theApplication().togglePause();
	}
}
#endif
