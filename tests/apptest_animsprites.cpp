#include "apptest_animsprites.h"
#include "Application.h"
#include "Texture.h"
#include "AnimatedSprite.h"
#include "Font.h"
#include "TextNode.h"
#include "IFile.h" // for dataPath()

namespace {

#ifdef __ANDROID__
const char* TextureFile = "spritesheet.webp";
#else
const char* TextureFile = "spritesheet.png";
#endif

}

nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	texture_ = new nc::Texture((nc::IFile::dataPath() + "textures/" + TextureFile).data());
	animSprite_ = new nc::AnimatedSprite(&rootNode, texture_);
	// Down
	nc::RectAnimation *animation = new nc::RectAnimation(0.12f, nc::RectAnimation::LOOPING, nc::RectAnimation::FROM_START);
	animation->addRect(0, 0, 48, 48);
	animation->addRect(48, 0, 48, 48);
	animation->addRect(96, 0, 48, 48);
	animation->addRect(144, 0, 48, 48);
	animation->addRect(0, 48, 48, 48);
	animation->addRect(48, 48, 48, 48);
	animation->addRect(96, 48, 48, 48);
	animation->addRect(144, 48, 48, 48);
	animSprite_->addAnimation(animation);

	animSprite_->setPosition(nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	animSprite_->setAnimation(0);
	animSprite_->setFrame(0);
	animSprite_->setPaused(true);
	destVector_ = animSprite_->position();
}

void MyEventHandler::onFrameStart()
{
	nc::Vector2f reachVector = destVector_ - animSprite_->absPosition();
	if (reachVector.length() > 1.0f)
	{
		reachVector.normalize();
		animSprite_->setPaused(false);

		float angle = 180.0f + (atan2(reachVector.y, reachVector.x) - atan2(1.0f, 0.0f)) * 180.0f / static_cast<float>(M_PI); // TODO clamp
		if (angle < 0.0f)
		{
			angle += 360.0f;
		}
		animSprite_->setRotation(angle);

		reachVector *= nc::theApplication().interval() * 100.0f;
		animSprite_->move(reachVector);
	}
	else
	{
		animSprite_->setFrame(0);
		animSprite_->setPaused(true);
	}
}

void MyEventHandler::onShutdown()
{
	delete animSprite_;
	delete texture_;
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	destVector_.x = event.x;
	destVector_.y = event.y;
}

void MyEventHandler::onTouchMove(const nc::TouchEvent &event)
{
	destVector_.x = event.x;
	destVector_.y = event.y;
}
#else
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

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
	{
		destVector_.x = static_cast<float>(event.x);
		destVector_.y = static_cast<float>(event.y);
	}
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state)
{
	if (state.isLeftButtonDown())
	{
		destVector_.x = static_cast<float>(state.x);
		destVector_.y = static_cast<float>(state.y);
	}
}
#endif
