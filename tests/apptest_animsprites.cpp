#include <ncine/common_constants.h>
#include "apptest_animsprites.h"
#include <ncine/Application.h>
#include <ncine/Texture.h>
#include <ncine/AnimatedSprite.h>
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const char *TextureFile = "spritesheet.webp";
#else
const char *TextureFile = "spritesheet.png";
#endif

const float SpriteSpeed = 100.0f;

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	texture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", TextureFile)).data());
	animSprite_ = nctl::makeUnique<nc::AnimatedSprite>(&rootNode, texture_.get());

	nc::RectAnimation animation(0.12f, nc::RectAnimation::LoopMode::ENABLED, nc::RectAnimation::RewindMode::FROM_START);
	animation.addRects(nc::Vector2i(48, 48), texture_->rect());
	animSprite_->addAnimation(nctl::move(animation));

	animSprite_->setPosition(nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	animSprite_->setAnimationIndex(0);
	animSprite_->setFrame(0);
	animSprite_->setPaused(true);
	destVector_ = animSprite_->position();
	joyVector_ = nc::Vector2f::Zero;
	pause_ = false;
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

	if (pause_ == false && reachVector.length() > 1.0f)
	{
		reachVector.normalize();
		animSprite_->setPaused(false);

		const float angle = 180.0f + (atan2f(reachVector.y, reachVector.x) - atan2f(1.0f, 0.0f)) * nc::fRadToDeg;
		animSprite_->setRotation(angle);

		reachVector *= nc::theApplication().frameTime() * SpriteSpeed;
		animSprite_->move(reachVector);
	}
	else
	{
		if (pause_ == false)
			animSprite_->setFrame(0);
		animSprite_->setPaused(true);
	}
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
	if (event.sym == nc::KeySym::F)
	{
		nc::IGfxDevice &gfxDevice = nc::theApplication().gfxDevice();
		gfxDevice.setFullScreen(!gfxDevice.isFullScreen());
		if (gfxDevice.isFullScreen() == false)
			gfxDevice.setWindowSize(nc::theApplication().appConfiguration().resolution);
	}
	else if (event.sym == nc::KeySym::P)
		pause_ = !pause_;
	else if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.button == nc::MouseButton::LEFT)
	{
		destVector_.x = static_cast<float>(event.x);
		destVector_.y = static_cast<float>(event.y);
	}
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state)
{
	if (state.isButtonDown(nc::MouseButton::LEFT))
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

void MyEventHandler::onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event)
{
	if (event.buttonName == nc::ButtonName::START)
		pause_ = !pause_;
	else if (event.buttonName == nc::ButtonName::GUIDE)
		nc::theApplication().quit();
}

void MyEventHandler::onJoyDisconnected(const nc::JoyConnectionEvent &event)
{
	joyVector_ = nc::Vector2f::Zero;
}
