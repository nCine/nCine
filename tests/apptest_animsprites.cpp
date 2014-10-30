#include "apptest_animsprites.h"
#include "Application.h"
#include "AudioStreamPlayer.h"
#include "Texture.h"
#include "AnimatedSprite.h"
#include "Font.h"
#include "TextNode.h"
#include "IInputManager.h"

//#define WITH_8DIRECTIONS

nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	nc::IInputManager::setHandler(this);
	nc::SceneNode &rootNode = nc::Application::rootNode();

#ifdef __ANDROID__
	audioPlayer_ = new nc::AudioStreamPlayer("sdcard/ncine/music.ogg");
	texture_ = new nc::Texture("/sdcard/ncine/abta_playertwo.dds");
//	audioPlayer_ = new nc::AudioStreamPlayer("asset::bomb.ogg");
//	texture_ = new nc::Texture("asset::abta_player.dds.mp3");
#else
	audioPlayer_ = new nc::AudioStreamPlayer("sounds/music.ogg");
	texture_ = new nc::Texture("textures/abta_playertwo.png");
//	texture_ = new nc::Texture("textures/abta_playertwo_bc3.dds");
#endif

	audioPlayer_->setLooping(true);
	audioPlayer_->play();
	animSprite_ = new nc::AnimatedSprite(&rootNode, texture_);
	// Up
	nc::RectAnimation *animation = new nc::RectAnimation(0.06f, true, true);
	animation->addRect(0, 0, 32, 32);
	animation->addRect(32, 0, 32, 32);
	animation->addRect(64, 0, 32, 32);
	animation->addRect(96, 0, 32, 32);
	animation->addRect(128, 0, 32, 32);
	animSprite_->addAnimation(animation);
#ifdef WITH_8DIRECTIONS
	// Up-right
	animation = new nc::RectAnimation(0.06f, true, true);
	animation->addRect(160, 0, 32, 32);
	animation->addRect(192, 0, 32, 32);
	animation->addRect(224, 0, 32, 32);
	animation->addRect(256, 0, 32, 32);
	animation->addRect(288, 0, 32, 32);
	animSprite_->addAnimation(animation);
	// Right
	animation = new nc::RectAnimation(0.06f, true, true);
	animation->addRect(320, 0, 32, 32);
	animation->addRect(352, 0, 32, 32);
	animation->addRect(384, 0, 32, 32);
	animation->addRect(416, 0, 32, 32);
	animation->addRect(448, 0, 32, 32);
	animSprite_->addAnimation(animation);
	// Down-right
	animation = new nc::RectAnimation(0.06f, true, true);
	animation->addRect(480, 0, 32, 32);
	animation->addRect(512, 0, 32, 32);
	animation->addRect(544, 0, 32, 32);
	animation->addRect(576, 0, 32, 32);
	animation->addRect(608, 0, 32, 32);
	animSprite_->addAnimation(animation);
	// Down
	animation = new nc::RectAnimation(0.06f, true, true);
	animation->addRect(640, 0, 32, 32);
	animation->addRect(0, 32, 32, 32);
	animation->addRect(32, 32, 32, 32);
	animation->addRect(64, 32, 32, 32);
	animation->addRect(96, 32, 32, 32);
	animSprite_->addAnimation(animation);
	// Down-left
	animation = new nc::RectAnimation(0.06f, true, true);
	animation->addRect(128, 32, 32, 32);
	animation->addRect(160, 32, 32, 32);
	animation->addRect(192, 32, 32, 32);
	animation->addRect(224, 32, 32, 32);
	animation->addRect(256, 32, 32, 32);
	animSprite_->addAnimation(animation);
	// Left
	animation = new nc::RectAnimation(0.06f, true, true);
	animation->addRect(288, 32, 32, 32);
	animation->addRect(320, 32, 32, 32);
	animation->addRect(352, 32, 32, 32);
	animation->addRect(384, 32, 32, 32);
	animation->addRect(416, 32, 32, 32);
	animSprite_->addAnimation(animation);
	// Up-left
	animation = new nc::RectAnimation(0.06f, true, true);
	animation->addRect(448, 32, 32, 32);
	animation->addRect(480, 32, 32, 32);
	animation->addRect(512, 32, 32, 32);
	animation->addRect(544, 32, 32, 32);
	animation->addRect(576, 32, 32, 32);
	animSprite_->addAnimation(animation);
	// Special
	animation = new nc::RectAnimation(0.06f, true, true);
	animation->addRect(608, 32, 32, 32);
	animation->addRect(640, 32, 32, 32);
	animation->addRect(0, 64, 32, 32);
	animation->addRect(32, 64, 32, 32);
	animation->addRect(64, 64, 32, 32);
	animSprite_->addAnimation(animation);
#endif

	animSprite_->setPosition(nc::Application::width() * 0.5f, nc::Application::height() * 0.5f);
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

#ifdef WITH_8DIRECTIONS
		const float dirTolerance = 0.3f;
		if (reachVector.x < -dirTolerance) // Right
		{
			if (reachVector.y > dirTolerance)
			{
				animSprite_->setAnimation(1);    // Up-right
			}
			else if (reachVector.y < -dirTolerance)
			{
				animSprite_->setAnimation(3);    // Down-right
			}
			else
			{
				animSprite_->setAnimation(2);    // Right
			}
		}
		else if (reachVector.x > dirTolerance) // Left
		{
			if (reachVector.y > dirTolerance)
			{
				animSprite_->setAnimation(7);    // Up-left
			}
			else if (reachVector.y < -dirTolerance)
			{
				animSprite_->setAnimation(5);    // Down-left
			}
			else
			{
				animSprite_->setAnimation(6);    // Left
			}
		}
		else // Pure up or down
		{
			if (reachVector.y > 0.0f)
			{
				animSprite_->setAnimation(0);    // Up
			}
			else
			{
				animSprite_->setAnimation(4);    // Down
			}
		}
#else
		float angle = -(atan2(reachVector.y, reachVector.x) - atan2(1.0f, 0.0f)) * 180.0f / M_PI;
		if (angle < 0.0f)
		{
			angle += 360.0f;
		}
		animSprite_->setRotation(angle);
#endif

		reachVector *= nc::Application::interval() * 100.0f;
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
	delete audioPlayer_;
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
		nc::Application::quit();
	}
	else if (event.sym == nc::KEY_SPACE)
	{
		nc::Application::togglePause();
	}
}

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
	{
		destVector_.x = event.x;
		destVector_.y = event.y;
	}
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state)
{
	if (state.isLeftButtonDown())
	{
		destVector_.x = state.x;
		destVector_.y = state.y;
	}
}
#endif
