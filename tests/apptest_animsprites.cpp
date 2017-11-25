#include "common_constants.h"
#include "apptest_animsprites.h"
#include "Application.h"
#include "Texture.h"
#include "AnimatedSprite.h"
#include "IFile.h" // for dataPath()
#include "apptest_datapath.h"

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

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	texture_ = new nc::Texture((nc::IFile::dataPath() + "textures/" + TextureFile).data());
	animSprite_ = new nc::AnimatedSprite(&rootNode, texture_);
	// Down
	nc::RectAnimation *animation = new nc::RectAnimation(0.12f,
	                                                     nc::RectAnimation::LoopMode::ENABLED,
	                                                     nc::RectAnimation::RewindMode::FROM_START);
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

	const nc::KeyboardState &keyState = nc::theApplication().inputManager().keyboardState();

	if (keyState.isKeyDown(nc::KeySym::RIGHT) || keyState.isKeyDown(nc::KeySym::D) ||
	    keyState.isKeyDown(nc::KeySym::LEFT) || keyState.isKeyDown(nc::KeySym::A) ||
	    keyState.isKeyDown(nc::KeySym::UP) || keyState.isKeyDown(nc::KeySym::W) ||
	    keyState.isKeyDown(nc::KeySym::DOWN) || keyState.isKeyDown(nc::KeySym::S))
	{
		if (reachVector.length() > 1.0f)
			reachVector.normalize();
	}

	if (keyState.isKeyDown(nc::KeySym::RIGHT) || keyState.isKeyDown(nc::KeySym::D))
	{
		reachVector.x = 2.0f;
		destVector_ = animSprite_->position() + reachVector;
	}
	else if (keyState.isKeyDown(nc::KeySym::LEFT) || keyState.isKeyDown(nc::KeySym::A))
	{
		reachVector.x = -2.0f;
		destVector_ = animSprite_->position() + reachVector;
	}
	if (keyState.isKeyDown(nc::KeySym::UP) || keyState.isKeyDown(nc::KeySym::W))
	{
		reachVector.y = 2.0f;
		destVector_ = animSprite_->position() + reachVector;
	}
	else if (keyState.isKeyDown(nc::KeySym::DOWN) || keyState.isKeyDown(nc::KeySym::S))
	{
		reachVector.y = -2.0f;
		destVector_ = animSprite_->position() + reachVector;
	}

	if (joyVector_.length() > nc::IInputManager::LeftStickDeadZone)
	{
		reachVector = joyVector_.normalized() * 2.0;
		destVector_ = animSprite_->position() + reachVector;
	}

	if (reachVector.length() > 1.0f)
	{
		reachVector.normalize();
		animSprite_->setPaused(false);

		float angle = 180.0f + (atan2f(reachVector.y, reachVector.x) - atan2f(1.0f, 0.0f)) * 180.0f / nc::fPi; // TODO clamp
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
#endif

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
	else if (event.sym == nc::KeySym::SPACE)
		nc::theApplication().togglePause();
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

void MyEventHandler::onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event)
{
	if (event.axisName == nc::AxisName::LX)
		joyVector_.x = event.value;
	else if (event.axisName == nc::AxisName::LY)
		joyVector_.y = -event.value;
}

void MyEventHandler::onJoyDisconnected(const nc::JoyConnectionEvent &event)
{
	joyVector_ = nc::Vector2f::Zero;
}
