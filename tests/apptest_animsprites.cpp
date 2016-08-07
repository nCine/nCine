#include "apptest_animsprites.h"
#include "Application.h"
#include "Texture.h"
#include "AnimatedSprite.h"
#include "Font.h"
#include "TextNode.h"
#include "IFile.h" // for dataPath()
#include "apptest_joymapping.h"

namespace {

#ifdef __ANDROID__
	const char *TextureFile = "spritesheet.webp";
#else
	const char *TextureFile = "spritesheet.png";
#endif

const float SpriteSpeed = 100.0f;

}

nc::IAppEventHandler *createAppEventHandler()
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
	joyVector_ = nc::Vector2f::Zero;
}

void MyEventHandler::onFrameStart()
{
	nc::Vector2f reachVector = destVector_ - animSprite_->position();

#ifndef __ANDROID__
	const nc::KeyboardState &keyState = nc::theApplication().inputManager().keyboardState();

	if (keyState.isKeyDown(nc::KEY_RIGHT) || keyState.isKeyDown(nc::KEY_D) ||
	    keyState.isKeyDown(nc::KEY_LEFT) || keyState.isKeyDown(nc::KEY_A) ||
	    keyState.isKeyDown(nc::KEY_UP) || keyState.isKeyDown(nc::KEY_W) ||
	    keyState.isKeyDown(nc::KEY_DOWN) || keyState.isKeyDown(nc::KEY_S))
	{
		if (reachVector.length() > 1.0f) { reachVector.normalize(); }
	}

	if (keyState.isKeyDown(nc::KEY_RIGHT) || keyState.isKeyDown(nc::KEY_D))
	{
		reachVector.x = 2.0f;
		destVector_ = animSprite_->position() + reachVector;
	}
	else if (keyState.isKeyDown(nc::KEY_LEFT) || keyState.isKeyDown(nc::KEY_A))
	{
		reachVector.x = -2.0f;
		destVector_ = animSprite_->position() + reachVector;
	}
	if (keyState.isKeyDown(nc::KEY_UP) || keyState.isKeyDown(nc::KEY_W))
	{
		reachVector.y = 2.0f;
		destVector_ = animSprite_->position() + reachVector;
	}
	else if (keyState.isKeyDown(nc::KEY_DOWN) || keyState.isKeyDown(nc::KEY_S))
	{
		reachVector.y = -2.0f;
		destVector_ = animSprite_->position() + reachVector;
	}
#endif

	if (joyVector_.length() > LeftStickDeadZone)
	{
		reachVector = joyVector_.normalized() * 2.0;
		destVector_ = animSprite_->position() + reachVector;
	}

	if (reachVector.length() > 1.0f)
	{
		reachVector.normalize();
		animSprite_->setPaused(false);

		float angle = 180.0f + (atan2(reachVector.y, reachVector.x) - atan2(1.0f, 0.0f)) * 180.0f / static_cast<float>(M_PI); // TODO clamp
		animSprite_->setRotation(angle);

		reachVector *= nc::theApplication().interval() * SpriteSpeed;
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
	destVector_.x = event.pointers[0].x;
	destVector_.y = event.pointers[0].y;
}

void MyEventHandler::onTouchMove(const nc::TouchEvent &event)
{
	destVector_.x = event.pointers[0].x;
	destVector_.y = event.pointers[0].y;
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

void MyEventHandler::onJoyAxisMoved(const nc::JoyAxisEvent &event)
{
	if (isAxis(event, AXIS_LX))
	{
		joyVector_.x = normValue(event, AXIS_LX);
	}
	else if (isAxis(event, AXIS_LY))
	{
		joyVector_.y = -normValue(event, AXIS_LY);
	}
}

void MyEventHandler::onJoyDisconnected(const nc::JoyConnectionEvent &event)
{
	joyVector_ = nc::Vector2f::Zero;
}
