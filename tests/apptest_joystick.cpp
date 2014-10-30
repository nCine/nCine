#include "apptest_joystick.h"
#include "Application.h"
#include "ServiceLocator.h"
#include "Texture.h"
#include "Sprite.h"
#include "Font.h"
#include "TextNode.h"

// Xbox 360 controller defines from XInput includes
#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30

nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	nc::IInputManager::setHandler(this);
	nc::SceneNode &rootNode = nc::Application::rootNode();

#ifdef __ANDROID__
	texture_ = new nc::Texture("/sdcard/ncine/checker_256_ETC1_MIP.ktx");
#else
	texture_ = new nc::Texture("textures/texture3.png");
#endif
	sprites_[0] = new nc::Sprite(&rootNode, texture_, nc::Application::width() * 0.25f, nc::Application::height() * 0.5f);
	sprites_[1] = new nc::Sprite(&rootNode, texture_, nc::Application::width() * 0.75f, nc::Application::height() * 0.5f);
	sprites_[0]->setScale(0.5f);
	sprites_[1]->setScale(0.5f);

#ifdef __ANDROID__
	font_ = new nc::Font("/sdcard/ncine/trebuchet32_256_4444.pvr", "/sdcard/ncine/trebuchet32_256.fnt");
#else
	font_ = new nc::Font("fonts/trebuchet32_256.png", "fonts/trebuchet32_256.fnt");
#endif
	textNode_ = new nc::TextNode(&rootNode, font_);
	textNode_->setScale(0.85f);
	textNode_->setPosition(nc::Application::width() * 0.1f, nc::Application::height() * 0.35f);

	for (int i = 0; i < NumJoysticks; i++)
	{
		if (nc::Application::inputManager().isJoyPresent(i))
		{
			LOGI_X("Joystick %d (%s) - %d axes, %d buttons", i,
				nc::Application::inputManager().joyName(i),
				nc::Application::inputManager().joyNumAxes(i),
				nc::Application::inputManager().joyNumButtons(i));
		}
	}
}

void MyEventHandler::onFrameStart()
{
	int firstJoy = 0;
	for (int i = 0; i < NumJoysticks; i++)
	{
		if (nc::Application::inputManager().isJoyPresent(i))
		{
			firstJoy = i;
			break;
		}
	}

	memset(joyString_, 0, NumChars);
	for (int i = 0; i < NumJoysticks; i++)
	{
		if (nc::Application::inputManager().isJoyPresent(i))
		{
			sprintf(&joyString_[strlen(joyString_)], "Joystick %d: %s (%d axes, %d buttons)\n", i,
				nc::Application::inputManager().joyName(i),
				nc::Application::inputManager().joyNumAxes(i),
				nc::Application::inputManager().joyNumButtons(i));
		}
	}
	strncat(joyString_, "Axes:", 5);
	for (int i = 0; i < NumAxes; i++)
	{
		axisValues_[i] = nc::Application::inputManager().joyAxisNormValue(firstJoy, i);
		sprintf(&joyString_[strlen(joyString_)], " %.2f", axisValues_[i]);
	}
	strncat(joyString_, "\nButtons:", 9);
	for (int i = 0; i < NumButtons; i++)
	{
		buttonStates_[i] = nc::Application::inputManager().isJoyButtonPressed(firstJoy, i);
		sprintf(&joyString_[strlen(joyString_)], " %u", buttonStates_[i]);
	}
	textNode_->setString(joyString_);


	float xOffset1 = axisValues_[0] * 0.1f;
	float yOffset1 = -axisValues_[1] * 0.1f;
	float scale1 = 1.0f + 0.5f * axisValues_[2];
	sprites_[0]->setPosition(nc::Application::width() * (0.25f + xOffset1), nc::Application::height() * (0.5f + yOffset1));
	sprites_[0]->setScale(scale1);

	float xOffset2 = axisValues_[3] * 0.1f;
	float yOffset2 = -axisValues_[4] * 0.1f;
	float scale2 = 1.0f + 0.5f * axisValues_[5];
	sprites_[1]->setPosition(nc::Application::width() * (0.75f + xOffset2), nc::Application::height() * (0.5f + yOffset2));
	sprites_[1]->setScale(scale2);
}

void MyEventHandler::onShutdown()
{
	delete textNode_;
	delete font_;
	delete sprites_[0];
	delete sprites_[1];
	delete texture_;
}

#ifndef __ANDROID__
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
#endif
