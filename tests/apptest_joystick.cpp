#include "apptest_joystick.h"
#include <nctl/algorithms.h>
#include <ncine/Application.h>
#include <nctl/String.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/TextNode.h>
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const char *TextureFile = "texture4_ETC2.ktx";
const char *FontTextureFile = "DroidSans32_256_ETC2.ktx";
#else
const char *TextureFile = "texture4.png";
const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

enum SpriteNames
{
	AXIS_LEFT,
	AXIS_RIGHT,
	BUTTON_X,
	BUTTON_Y,
	BUTTON_A,
	BUTTON_B,
	BUTTON_L1,
	BUTTON_R1,
	AXIS_L2,
	AXIS_R2,
	BUTTON_BACK,
	BUTTON_GUIDE,
	BUTTON_START,
	DPAD_LEFT,
	DPAD_UP,
	DPAD_DOWN,
	DPAD_RIGHT
};

const float triggerPosX = 0.12f;
const float stickMovement = 0.02f;

const float stickPressedSize = 0.45f;
const float stickUnpressedSize = 0.4f;
const float buttonPressedSize = 0.18f;
const float buttonUnpressedSize = 0.15f;
const float startPressedSize = 0.16f;
const float startUnpressedSize = 0.13f;
const float guidePressedSize = 0.33f;
const float guideUnpressedSize = 0.3f;

const char *hatStateToString(unsigned char hatState)
{
	switch (hatState)
	{
		case nc::HatState::CENTERED: return "centered";
		case nc::HatState::UP: return "up";
		case nc::HatState::RIGHT: return "right";
		case nc::HatState::DOWN: return "down";
		case nc::HatState::LEFT: return "left";
		case nc::HatState::RIGHT_UP: return "right-up";
		case nc::HatState::RIGHT_DOWN: return "right-down";
		case nc::HatState::LEFT_UP: return "left-up";
		case nc::HatState::LEFT_DOWN: return "left-down";
		default: return "unknown";
	}
}

const char *dpadButtonsToString(const nc::JoyMappedState &joyState)
{
	if (joyState.isButtonPressed(nc::ButtonName::DPAD_UP))
	{
		if (joyState.isButtonPressed(nc::ButtonName::DPAD_RIGHT))
			return "right-up";
		else if (joyState.isButtonPressed(nc::ButtonName::DPAD_LEFT))
			return "left-up";
		return "up";
	}
	else if (joyState.isButtonPressed(nc::ButtonName::DPAD_DOWN))
	{
		if (joyState.isButtonPressed(nc::ButtonName::DPAD_RIGHT))
			return "right-down";
		else if (joyState.isButtonPressed(nc::ButtonName::DPAD_LEFT))
			return "left-down";
		return "down";
	}
	else if (joyState.isButtonPressed(nc::ButtonName::DPAD_RIGHT))
		return "right";
	else if (joyState.isButtonPressed(nc::ButtonName::DPAD_LEFT))
		return "left";

	return "centered";
}

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

	joyString_ = nctl::makeUnique<nctl::String>(unsigned(MaxNumChars));

	texture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", TextureFile)).data());

	const float appWidth = nc::theApplication().width();
	const float appHeight = nc::theApplication().height();

	joyNode_ = nctl::makeUnique<nc::SceneNode>(&rootNode, appWidth * 0.5f, appHeight * 0.65f);
	joyNode_->setScale(1.33f);
	for (unsigned int i = 0; i < NumSprites; i++)
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(joyNode_.get(), texture_.get()));

	sprites_[AXIS_LEFT]->setPosition(appWidth * -triggerPosX, 0.0f); // Left stick
	sprites_[AXIS_RIGHT]->setPosition(appWidth * triggerPosX * 0.5f, -appWidth * 0.075f); // Right stick
	sprites_[AXIS_LEFT]->setScale(stickUnpressedSize);
	sprites_[AXIS_RIGHT]->setScale(stickUnpressedSize);

	sprites_[BUTTON_X]->setPosition(appWidth * (triggerPosX - 0.02f), appWidth * 0.0f);
	sprites_[BUTTON_Y]->setPosition(appWidth * triggerPosX, appWidth * 0.02f);
	sprites_[BUTTON_A]->setPosition(appWidth * triggerPosX, appWidth * -0.02f);
	sprites_[BUTTON_B]->setPosition(appWidth * (triggerPosX + 0.02f), appWidth * 0.0f);
	sprites_[BUTTON_X]->setScale(buttonUnpressedSize);
	sprites_[BUTTON_Y]->setScale(buttonUnpressedSize);
	sprites_[BUTTON_A]->setScale(buttonUnpressedSize);
	sprites_[BUTTON_B]->setScale(buttonUnpressedSize);

	sprites_[BUTTON_L1]->setPosition(appWidth * -triggerPosX, appWidth * 0.07f); // Left bumper
	sprites_[BUTTON_R1]->setPosition(appWidth * triggerPosX, appWidth * 0.07f); // Right bumper
	sprites_[AXIS_L2]->setPosition(appWidth * -triggerPosX, appWidth * 0.1f); // Left trigger
	sprites_[AXIS_R2]->setPosition(appWidth * triggerPosX, appWidth * 0.1f); // Right trigger
	sprites_[BUTTON_L1]->setScale(buttonUnpressedSize);
	sprites_[BUTTON_R1]->setScale(buttonUnpressedSize);
	sprites_[AXIS_L2]->setScale(buttonUnpressedSize);
	sprites_[AXIS_R2]->setScale(buttonUnpressedSize);

	sprites_[BUTTON_BACK]->setPosition(appWidth * -0.03f, 0.0f);
	sprites_[BUTTON_GUIDE]->setPosition(0.0f, 0.0f);
	sprites_[BUTTON_START]->setPosition(appWidth * 0.03f, 0.0f);
	sprites_[BUTTON_BACK]->setScale(startUnpressedSize);
	sprites_[BUTTON_GUIDE]->setScale(guideUnpressedSize);
	sprites_[BUTTON_START]->setScale(startUnpressedSize);

	sprites_[DPAD_LEFT]->setPosition(appWidth * (-triggerPosX * 0.25f - 0.02f), -appWidth * 0.075f);
	sprites_[DPAD_UP]->setPosition(appWidth * -triggerPosX * 0.25f, -appWidth * (0.075f - 0.02f));
	sprites_[DPAD_DOWN]->setPosition(appWidth * -triggerPosX * 0.25f, -appWidth * (0.075f + 0.02f));
	sprites_[DPAD_RIGHT]->setPosition(appWidth * (-triggerPosX * 0.25f + 0.02f), -appWidth * 0.075f);
	sprites_[DPAD_LEFT]->setScale(buttonUnpressedSize);
	sprites_[DPAD_UP]->setScale(buttonUnpressedSize);
	sprites_[DPAD_DOWN]->setScale(buttonUnpressedSize);
	sprites_[DPAD_RIGHT]->setScale(buttonUnpressedSize);

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());
	textNode_ = nctl::makeUnique<nc::TextNode>(&rootNode, font_.get(), unsigned(MaxNumChars));
	textNode_->setScale(0.85f);

	for (int i = 0; i < nc::IInputManager::MaxNumJoysticks; i++)
	{
		if (nc::theApplication().inputManager().isJoyPresent(i))
		{
			LOGI_X("Joystick %d (%s) - %d axes, %d buttons, %d hats", i,
			       nc::theApplication().inputManager().joyName(i),
			       nc::theApplication().inputManager().joyNumAxes(i),
			       nc::theApplication().inputManager().joyNumButtons(i),
			       nc::theApplication().inputManager().joyNumHats(i));
		}
	}
}

void MyEventHandler::onFrameStart()
{
	const float appWidth = nc::theApplication().width();
	const float appHeight = nc::theApplication().height();

	int firstJoy = -1;
	for (int i = 0; i < nc::IInputManager::MaxNumJoysticks; i++)
	{
		if (nc::theApplication().inputManager().isJoyPresent(i))
		{
			firstJoy = i;
			break;
		}
	}

	joyString_->clear();
	if (firstJoy >= 0)
	{
		for (int i = 0; i < nc::IInputManager::MaxNumJoysticks; i++)
		{
			if (nc::theApplication().inputManager().isJoyPresent(i))
			{
				const char *joyName = nc::theApplication().inputManager().joyName(i);
				const char *joyGuid = nc::theApplication().inputManager().joyGuid(i);
				const int numAxes = nc::theApplication().inputManager().joyNumAxes(i);
				const int numButtons = nc::theApplication().inputManager().joyNumButtons(i);
				const int numHats = nc::theApplication().inputManager().joyNumHats(i);
				const bool isMapped = nc::theApplication().inputManager().isJoyMapped(i);

				joyString_->formatAppend("Joystick %d is %s: \"%s\" (%d axes, %d buttons, %d hats)\n",
				                         i, isMapped ? "mapped" : "unmapped", joyName, numAxes, numButtons, numHats);
				joyString_->formatAppend("GUID: \"%s\"\n", joyGuid);
			}
		}

		if (nc::theApplication().inputManager().isJoyPresent(firstJoy))
		{
			const int numAxes = nctl::min(nc::theApplication().inputManager().joyNumAxes(firstJoy), static_cast<int>(MaxNumAxes));
			const int numButtons = nctl::min(nc::theApplication().inputManager().joyNumButtons(firstJoy), static_cast<int>(MaxNumButtons));
			const int numHats = nctl::min(nc::theApplication().inputManager().joyNumHats(firstJoy), static_cast<int>(MaxNumHats));
			const nc::JoystickState &joyState = nc::theApplication().inputManager().joystickState(firstJoy);

			*joyString_ += "\nAxes:";
			for (int i = 0; i < numAxes; i++)
			{
				axisValues_[i] = joyState.axisNormValue(i);
				joyString_->formatAppend("  %d: (%.2f)", i, axisValues_[i]);
			}
			*joyString_ += "\nButtons:";
			for (int i = 0; i < numButtons; i++)
			{
				buttonStates_[i] = joyState.isButtonPressed(i);
				joyString_->formatAppend("  %d: (%u)", i, buttonStates_[i]);
			}
			*joyString_ += "\nHats:";
			for (int i = 0; i < numHats; i++)
			{
				hatStates_[i] = joyState.hatState(i);
				joyString_->formatAppend("  %d: (%s)", i, hatStateToString(hatStates_[i]));
			}
			*joyString_ += "\n";
		}

		if (nc::theApplication().inputManager().isJoyMapped(firstJoy))
		{
			const nc::JoyMappedState &joyState = nc::theApplication().inputManager().joyMappedState(firstJoy);

			*joyString_ += "\nAxes:";
			joyString_->formatAppend("  LX: (%.2f), LY: (%.2f), RX: (%.2f), RY: (%.2f), LTrigger: (%.2f), RTrigger: (%.2f)",
			                         joyState.axisValue(nc::AxisName::LX), joyState.axisValue(nc::AxisName::LY),
			                         joyState.axisValue(nc::AxisName::RX), joyState.axisValue(nc::AxisName::RY),
			                         joyState.axisValue(nc::AxisName::RTRIGGER), joyState.axisValue(nc::AxisName::LTRIGGER));

			*joyString_ += "\nButtons:";
			joyString_->formatAppend("  A: (%u), B: (%u), X: (%u), Y: (%u), Back: (%u), Guide: (%u), Start: (%u), Misc1: (%u)",
			                         joyState.isButtonPressed(nc::ButtonName::A), joyState.isButtonPressed(nc::ButtonName::B),
			                         joyState.isButtonPressed(nc::ButtonName::X), joyState.isButtonPressed(nc::ButtonName::Y),
			                         joyState.isButtonPressed(nc::ButtonName::BACK), joyState.isButtonPressed(nc::ButtonName::GUIDE),
			                         joyState.isButtonPressed(nc::ButtonName::START), joyState.isButtonPressed(nc::ButtonName::MISC1));
			joyString_->formatAppend("\n                LStick: (%u), RStick: (%u), LBumper: (%u), RBumper: (%u), D-Pad: (%s)",
			                         joyState.isButtonPressed(nc::ButtonName::LSTICK), joyState.isButtonPressed(nc::ButtonName::RSTICK),
			                         joyState.isButtonPressed(nc::ButtonName::LBUMPER), joyState.isButtonPressed(nc::ButtonName::RBUMPER),
			                         dpadButtonsToString(joyState));
		}
	}
	else
		joyString_->formatAppend("No joysticks connected");

	textNode_->setString(*joyString_);
	textNode_->setPosition(appWidth * 0.05f + textNode_->width() * 0.5f, appHeight * 0.38f - textNode_->height() * 0.5f);

	const nc::JoyMappedState &joyMappedState = nc::theApplication().inputManager().joyMappedState(firstJoy);
	nc::Vector2f joyVectorLeft(joyMappedState.axisValue(nc::AxisName::LX), joyMappedState.axisValue(nc::AxisName::LY));
	nc::Vector2f joyVectorRight(joyMappedState.axisValue(nc::AxisName::RX), joyMappedState.axisValue(nc::AxisName::RY));
	nc::theApplication().inputManager().deadZoneNormalize(joyVectorLeft, nc::IInputManager::LeftStickDeadZone);
	nc::theApplication().inputManager().deadZoneNormalize(joyVectorRight, nc::IInputManager::RightStickDeadZone);
	const bool lPressed = joyMappedState.isButtonPressed(nc::ButtonName::LSTICK);
	const bool rPressed = joyMappedState.isButtonPressed(nc::ButtonName::RSTICK);

	sprites_[AXIS_LEFT]->setPosition(appWidth * (-triggerPosX + joyVectorLeft.x * stickMovement), appWidth * -joyVectorLeft.y * stickMovement);
	sprites_[AXIS_RIGHT]->setPosition(appWidth * (triggerPosX * 0.5f + joyVectorRight.x * stickMovement), appWidth * (-0.075f - joyVectorRight.y * stickMovement));
	sprites_[AXIS_LEFT]->setScale(lPressed ? stickPressedSize : stickUnpressedSize);
	sprites_[AXIS_RIGHT]->setScale(rPressed ? stickPressedSize : stickUnpressedSize);

	const bool bxPressed = joyMappedState.isButtonPressed(nc::ButtonName::X);
	const bool byPressed = joyMappedState.isButtonPressed(nc::ButtonName::Y);
	const bool baPressed = joyMappedState.isButtonPressed(nc::ButtonName::A);
	const bool bbPressed = joyMappedState.isButtonPressed(nc::ButtonName::B);
	sprites_[BUTTON_X]->setScale(bxPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[BUTTON_Y]->setScale(byPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[BUTTON_A]->setScale(baPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[BUTTON_B]->setScale(bbPressed ? buttonPressedSize : buttonUnpressedSize);

	const bool lsPressed = joyMappedState.isButtonPressed(nc::ButtonName::LBUMPER);
	const bool rsPressed = joyMappedState.isButtonPressed(nc::ButtonName::RBUMPER);
	sprites_[BUTTON_L1]->setScale(lsPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[BUTTON_R1]->setScale(rsPressed ? buttonPressedSize : buttonUnpressedSize);

	const float joyLeftTrigger = joyMappedState.axisValue(nc::AxisName::LTRIGGER);
	const float joyRightTrigger = joyMappedState.axisValue(nc::AxisName::RTRIGGER);
	sprites_[AXIS_L2]->setScale(buttonUnpressedSize + joyLeftTrigger * 0.1f);
	sprites_[AXIS_R2]->setScale(buttonUnpressedSize + joyRightTrigger * 0.1f);

	const bool backPressed = joyMappedState.isButtonPressed(nc::ButtonName::BACK);
	const bool guidePressed = joyMappedState.isButtonPressed(nc::ButtonName::GUIDE);
	const bool startPressed = joyMappedState.isButtonPressed(nc::ButtonName::START);
	sprites_[BUTTON_BACK]->setScale(backPressed ? startPressedSize : startUnpressedSize);
	sprites_[BUTTON_GUIDE]->setScale(guidePressed ? guidePressedSize : guideUnpressedSize);
	sprites_[BUTTON_START]->setScale(startPressed ? startPressedSize : startUnpressedSize);

	const bool dpadLeftPressed = joyMappedState.isButtonPressed(nc::ButtonName::DPAD_LEFT);
	const bool dpadUpPressed = joyMappedState.isButtonPressed(nc::ButtonName::DPAD_UP);
	const bool dpadDownPressed = joyMappedState.isButtonPressed(nc::ButtonName::DPAD_DOWN);
	const bool dpadRighgtPressed = joyMappedState.isButtonPressed(nc::ButtonName::DPAD_RIGHT);
	sprites_[DPAD_LEFT]->setScale(dpadLeftPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[DPAD_UP]->setScale(dpadUpPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[DPAD_DOWN]->setScale(dpadDownPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[DPAD_RIGHT]->setScale(dpadRighgtPressed ? buttonPressedSize : buttonUnpressedSize);
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}
