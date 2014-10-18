#include "apptest_joystick.h"
#include "ncApplication.h"
#include "ncServiceLocator.h"
#include "ncTexture.h"
#include "ncSprite.h"
#include "ncFont.h"
#include "ncTextNode.h"

// Xbox 360 controller defines from XInput includes
#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30

ncIAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	ncIInputManager::setHandler(this);
	ncSceneNode &rootNode = ncApplication::rootNode();

#ifdef __ANDROID__
	texture_ = new ncTexture("/sdcard/ncine/checker_256_ETC1_MIP.ktx");
#else
	texture_ = new ncTexture("textures/texture3.png");
#endif
	sprites_[0] = new ncSprite(&rootNode, texture_, ncApplication::width() * 0.25f, ncApplication::height() * 0.5f);
	sprites_[1] = new ncSprite(&rootNode, texture_, ncApplication::width() * 0.75f, ncApplication::height() * 0.5f);
	sprites_[0]->setScale(0.5f);
	sprites_[1]->setScale(0.5f);

#ifdef __ANDROID__
	font_ = new ncFont("/sdcard/ncine/trebuchet32_256_4444.pvr", "/sdcard/ncine/trebuchet32_256.fnt");
#else
	font_ = new ncFont("fonts/trebuchet32_256.png", "fonts/trebuchet32_256.fnt");
#endif
	textNode_ = new ncTextNode(&rootNode, font_);
	textNode_->setScale(0.85f);
	textNode_->setPosition(ncApplication::width() * 0.1f, ncApplication::height() * 0.35f);

	for (int i = 0; i < NumJoysticks; i++)
	{
		if (ncApplication::inputManager().isJoyPresent(i))
		{
			LOGI_X("Joystick %d (%s) - %d axes, %d buttons", i,
				ncApplication::inputManager().joyName(i), ncApplication::inputManager().joyNumAxes(i), ncApplication::inputManager().joyNumButtons(i));
		}
	}
}

void MyEventHandler::onFrameStart()
{
	int firstJoy = 0;
	for (int i = 0; i < NumJoysticks; i++)
	{
		if (ncApplication::inputManager().isJoyPresent(i))
		{
			firstJoy = i;
			break;
		}
	}

	memset(joyString_, 0, NumChars);
	for (int i = 0; i < NumJoysticks; i++)
	{
		if (ncApplication::inputManager().isJoyPresent(i))
		{
			sprintf(&joyString_[strlen(joyString_)], "Joystick %d: %s (%d axes, %d buttons)\n", i,
				ncApplication::inputManager().joyName(i), ncApplication::inputManager().joyNumAxes(i), ncApplication::inputManager().joyNumButtons(i));
		}
	}
	strncat(joyString_, "Axes:", 5);
	for (int i = 0; i < NumAxes; i++)
	{
		axisValues_[i] = ncApplication::inputManager().joyAxisNormValue(firstJoy, i);
		sprintf(&joyString_[strlen(joyString_)], " %.2f", axisValues_[i]);
	}
	strncat(joyString_, "\nButtons:", 9);
	for (int i = 0; i < NumButtons; i++)
	{
		buttonStates_[i] = ncApplication::inputManager().isJoyButtonPressed(firstJoy, i);
		sprintf(&joyString_[strlen(joyString_)], " %u", buttonStates_[i]);
	}
	textNode_->setString(joyString_);


	float xOffset1 = axisValues_[0] * 0.1f;
	float yOffset1 = -axisValues_[1] * 0.1f;
	float scale1 = 1.0f + 0.5f * axisValues_[2];
	sprites_[0]->setPosition(ncApplication::width() * (0.25f + xOffset1), ncApplication::height() * (0.5f + yOffset1));
	sprites_[0]->setScale(scale1);

	float xOffset2 = axisValues_[3] * 0.1f;
	float yOffset2 = -axisValues_[4] * 0.1f;
	float scale2 = 1.0f + 0.5f * axisValues_[5];
	sprites_[1]->setPosition(ncApplication::width() * (0.75f + xOffset2), ncApplication::height() * (0.5f + yOffset2));
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
