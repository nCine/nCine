#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include "apptest_rotozoom.h"
#include "SceneNode.h"
#include "Texture.h"
#include "Sprite.h"
#include "IFile.h" // for dataPath()

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

	pause_ = false;
	angle_ = 0.0f;
	nc::StaticArray<nc::Rect, NumTextures> texRects;

	nc::SceneNode &rootNode = nc::theApplication().rootNode();

#ifdef __ANDROID__
	megaTexture_ = new nc::Texture((nc::IFile::dataPath() + "megatexture_256.dds").data());
#else
	megaTexture_ = new nc::Texture((nc::IFile::dataPath() + "textures/megatexture_256.png").data());
#endif
	texRects[0] = nc::Rect(0, 0, 145, 121);
	texRects[1] = nc::Rect(256 - 100, 0, 100, 100);
	texRects[2] = nc::Rect(0, 256 - 96, 96, 96);
	texRects[3] = nc::Rect(256 - 96, 256 - 96, 96, 96);

	dummy_ = new nc::SceneNode(&rootNode, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	dummy_->setScale(0.75f);

	int index = 0;
	for (int i = -NumRowSprites * 0.5f; i < NumRowSprites * 0.5f; i++)
	{
		for (int j = -NumColSprites * 0.5f; j < NumColSprites * 0.5f; j++)
		{
			sprites_[index] = new nc::Sprite(dummy_, megaTexture_, i * 100.0f, j * 100.0f);
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
		angle_ += 2.5f * nc::theApplication().interval();
	}
	float sine = sinf(angle_);
	float cosine = cosf(angle_);

	dummy_->x = nc::theApplication().width() * 0.5f + sine * 100.0f;
	dummy_->y = nc::theApplication().height() * 0.5f + cosine * 150.0f;
	dummy_->setRotation(angle_ * 8.0f);
	dummy_->setScale(((sine * 0.15f) + 1.0f) * 0.5f);

	for (int i = 0; i < NumColSprites * NumRowSprites; i++)
	{
		sprites_[i]->setRotation(-angle_ * 8.0f);
	}

	nc::theApplication().rootNode().x = (sine + 1.0f) * 50.0f;
	nc::theApplication().rootNode().y = (cosine + 1.0f) * 50.0f;
	nc::theApplication().rootNode().setRotation(sine * 10.0f);
	nc::theApplication().rootNode().setScale(((cosine * 0.1f) + 1.0f) * 0.75f);
}

void MyEventHandler::onShutdown()
{
	delete dummy_; // and all its children (the sprites)
	delete megaTexture_;
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	pause_ = true;
}

void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	pause_ = false;
}
#else
void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
	{
		pause_ = true;
	}
}

void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent& event)
{
	if (event.isLeftButton())
	{
		pause_ = false;
	}
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
