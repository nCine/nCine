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

bool performVibration = false;

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
	if (joyState.isButtonDown(nc::ButtonName::DPAD_UP))
	{
		if (joyState.isButtonDown(nc::ButtonName::DPAD_RIGHT))
			return "right-up";
		else if (joyState.isButtonDown(nc::ButtonName::DPAD_LEFT))
			return "left-up";
		return "up";
	}
	else if (joyState.isButtonDown(nc::ButtonName::DPAD_DOWN))
	{
		if (joyState.isButtonDown(nc::ButtonName::DPAD_RIGHT))
			return "right-down";
		else if (joyState.isButtonDown(nc::ButtonName::DPAD_LEFT))
			return "left-down";
		return "down";
	}
	else if (joyState.isButtonDown(nc::ButtonName::DPAD_RIGHT))
		return "right";
	else if (joyState.isButtonDown(nc::ButtonName::DPAD_LEFT))
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
	const nc::IInputManager &inputManager = nc::theApplication().inputManager();

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
		if (inputManager.isJoyPresent(i))
		{
			LOGI_X("Joystick %d (%s) - %d axes, %d buttons, %d hats", i,
			       inputManager.joyName(i),
			       inputManager.joyNumAxes(i),
			       inputManager.joyNumButtons(i),
			       inputManager.joyNumHats(i));
		}
	}
}

void MyEventHandler::onFrameStart()
{
	nc::IInputManager &inputManager = nc::theApplication().inputManager();

	const float appWidth = nc::theApplication().width();
	const float appHeight = nc::theApplication().height();

	int firstJoy = -1;
	for (int i = 0; i < nc::IInputManager::MaxNumJoysticks; i++)
	{
		if (inputManager.isJoyPresent(i))
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
			if (inputManager.isJoyPresent(i))
			{
				const char *joyName = inputManager.joyName(i);
				const char *joyGuid = inputManager.joyGuid(i);
				const int numAxes = inputManager.joyNumAxes(i);
				const int numButtons = inputManager.joyNumButtons(i);
				const int numHats = inputManager.joyNumHats(i);
				const bool isMapped = inputManager.isJoyMapped(i);
				const bool hasVibration = inputManager.hasJoyVibration(i);

				joyString_->formatAppend("Joystick %d is %s: \"%s\" (%d axes, %d buttons, %d hats, %s vibration)\n",
				                         i, isMapped ? "mapped" : "unmapped", joyName, numAxes, numButtons, numHats, hasVibration ? "has" : "no");
				joyString_->formatAppend("GUID: \"%s\"\n", joyGuid);
			}
		}

		if (inputManager.isJoyPresent(firstJoy))
		{
			const int numAxes = nctl::min(inputManager.joyNumAxes(firstJoy), static_cast<int>(MaxNumAxes));
			const int numButtons = nctl::min(inputManager.joyNumButtons(firstJoy), static_cast<int>(MaxNumButtons));
			const int numHats = nctl::min(inputManager.joyNumHats(firstJoy), static_cast<int>(MaxNumHats));
			const nc::JoystickState &joyState = inputManager.joystickState(firstJoy);

			*joyString_ += "\nAxes:";
			for (int i = 0; i < numAxes; i++)
			{
				axisValues_[i] = joyState.axisNormValue(i);
				joyString_->formatAppend("  %d: (%.2f)", i, axisValues_[i]);
			}
			*joyString_ += "\nButtons:";
			for (int i = 0; i < numButtons; i++)
			{
				buttonStates_[i] = joyState.isButtonDown(i);
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

		if (inputManager.isJoyMapped(firstJoy))
		{
			const nc::JoyMappedState &joyState = inputManager.joyMappedState(firstJoy);

			*joyString_ += "\nAxes:";
			joyString_->formatAppend("  LX: (%.2f), LY: (%.2f), RX: (%.2f), RY: (%.2f), LTrigger: (%.2f), RTrigger: (%.2f)",
			                         joyState.axisValue(nc::AxisName::LX), joyState.axisValue(nc::AxisName::LY),
			                         joyState.axisValue(nc::AxisName::RX), joyState.axisValue(nc::AxisName::RY),
			                         joyState.axisValue(nc::AxisName::RTRIGGER), joyState.axisValue(nc::AxisName::LTRIGGER));

			*joyString_ += "\nButtons:";
			joyString_->formatAppend("  A: (%u), B: (%u), X: (%u), Y: (%u), Back: (%u), Guide: (%u), Start: (%u), Misc1: (%u)",
			                         joyState.isButtonDown(nc::ButtonName::A), joyState.isButtonDown(nc::ButtonName::B),
			                         joyState.isButtonDown(nc::ButtonName::X), joyState.isButtonDown(nc::ButtonName::Y),
			                         joyState.isButtonDown(nc::ButtonName::BACK), joyState.isButtonDown(nc::ButtonName::GUIDE),
			                         joyState.isButtonDown(nc::ButtonName::START), joyState.isButtonDown(nc::ButtonName::MISC1));
			joyString_->formatAppend("\n                LStick: (%u), RStick: (%u), LBumper: (%u), RBumper: (%u), D-Pad: (%s)",
			                         joyState.isButtonDown(nc::ButtonName::LSTICK), joyState.isButtonDown(nc::ButtonName::RSTICK),
			                         joyState.isButtonDown(nc::ButtonName::LBUMPER), joyState.isButtonDown(nc::ButtonName::RBUMPER),
			                         dpadButtonsToString(joyState));
		}
	}
	else
		joyString_->formatAppend("No joysticks connected");

	textNode_->setString(*joyString_);
	textNode_->setPosition(appWidth * 0.05f + textNode_->width() * 0.5f, appHeight * 0.38f - textNode_->height() * 0.5f);

	const nc::JoyMappedState &joyMappedState = inputManager.joyMappedState(firstJoy);
	nc::Vector2f joyVectorLeft(joyMappedState.axisValue(nc::AxisName::LX), joyMappedState.axisValue(nc::AxisName::LY));
	nc::Vector2f joyVectorRight(joyMappedState.axisValue(nc::AxisName::RX), joyMappedState.axisValue(nc::AxisName::RY));
	inputManager.deadZoneNormalize(joyVectorLeft, nc::IInputManager::LeftStickDeadZone);
	inputManager.deadZoneNormalize(joyVectorRight, nc::IInputManager::RightStickDeadZone);
	const bool lIsDown = joyMappedState.isButtonDown(nc::ButtonName::LSTICK);
	const bool rIsDown = joyMappedState.isButtonDown(nc::ButtonName::RSTICK);

	sprites_[AXIS_LEFT]->setPosition(appWidth * (-triggerPosX + joyVectorLeft.x * stickMovement), appWidth * -joyVectorLeft.y * stickMovement);
	sprites_[AXIS_RIGHT]->setPosition(appWidth * (triggerPosX * 0.5f + joyVectorRight.x * stickMovement), appWidth * (-0.075f - joyVectorRight.y * stickMovement));
	sprites_[AXIS_LEFT]->setScale(lIsDown ? stickPressedSize : stickUnpressedSize);
	sprites_[AXIS_RIGHT]->setScale(rIsDown ? stickPressedSize : stickUnpressedSize);

	const bool bxIsDown = joyMappedState.isButtonDown(nc::ButtonName::X);
	const bool byIsDown = joyMappedState.isButtonDown(nc::ButtonName::Y);
	const bool baIsDown = joyMappedState.isButtonDown(nc::ButtonName::A);
	const bool bbIsDown = joyMappedState.isButtonDown(nc::ButtonName::B);
	sprites_[BUTTON_X]->setScale(bxIsDown ? buttonPressedSize : buttonUnpressedSize);
	sprites_[BUTTON_Y]->setScale(byIsDown ? buttonPressedSize : buttonUnpressedSize);
	sprites_[BUTTON_A]->setScale(baIsDown ? buttonPressedSize : buttonUnpressedSize);
	sprites_[BUTTON_B]->setScale(bbIsDown ? buttonPressedSize : buttonUnpressedSize);

	const bool lsIsDown = joyMappedState.isButtonDown(nc::ButtonName::LBUMPER);
	const bool rsIsdown = joyMappedState.isButtonDown(nc::ButtonName::RBUMPER);
	sprites_[BUTTON_L1]->setScale(lsIsDown ? buttonPressedSize : buttonUnpressedSize);
	sprites_[BUTTON_R1]->setScale(rsIsdown ? buttonPressedSize : buttonUnpressedSize);

	const float joyLeftTrigger = joyMappedState.axisValue(nc::AxisName::LTRIGGER);
	const float joyRightTrigger = joyMappedState.axisValue(nc::AxisName::RTRIGGER);
	sprites_[AXIS_L2]->setScale(buttonUnpressedSize + joyLeftTrigger * 0.1f);
	sprites_[AXIS_R2]->setScale(buttonUnpressedSize + joyRightTrigger * 0.1f);

	const bool backIsDown = joyMappedState.isButtonDown(nc::ButtonName::BACK);
	const bool guideIsDown = joyMappedState.isButtonDown(nc::ButtonName::GUIDE);
	const bool startIsDown = joyMappedState.isButtonDown(nc::ButtonName::START);
	sprites_[BUTTON_BACK]->setScale(backIsDown ? startPressedSize : startUnpressedSize);
	sprites_[BUTTON_GUIDE]->setScale(guideIsDown ? guidePressedSize : guideUnpressedSize);
	sprites_[BUTTON_START]->setScale(startIsDown ? startPressedSize : startUnpressedSize);

	const bool dpadLeftIsDown = joyMappedState.isButtonDown(nc::ButtonName::DPAD_LEFT);
	const bool dpadUpIsDown = joyMappedState.isButtonDown(nc::ButtonName::DPAD_UP);
	const bool dpadDownIsDown = joyMappedState.isButtonDown(nc::ButtonName::DPAD_DOWN);
	const bool dpadRighgtIsDown = joyMappedState.isButtonDown(nc::ButtonName::DPAD_RIGHT);
	sprites_[DPAD_LEFT]->setScale(dpadLeftIsDown ? buttonPressedSize : buttonUnpressedSize);
	sprites_[DPAD_UP]->setScale(dpadUpIsDown ? buttonPressedSize : buttonUnpressedSize);
	sprites_[DPAD_DOWN]->setScale(dpadDownIsDown ? buttonPressedSize : buttonUnpressedSize);
	sprites_[DPAD_RIGHT]->setScale(dpadRighgtIsDown ? buttonPressedSize : buttonUnpressedSize);

	const bool hasVibration = inputManager.hasJoyVibration(firstJoy);
	if (joyMappedState.isButtonPressed(nc::ButtonName::BACK))
	{
		performVibration = !performVibration;
		if (hasVibration && performVibration == false)
			inputManager.joyVibrate(firstJoy, 0.0f, 0.0f, 0);
	}

	if (hasVibration && performVibration)
	{
		float intensities[2] = { 0.0f, 0.0f };
		if (joyLeftTrigger > nc::IInputManager::TriggerDeadZone)
			intensities[0] = joyLeftTrigger;
		if (joyRightTrigger > nc::IInputManager::TriggerDeadZone)
			intensities[1] = joyRightTrigger;

		const int frameTimeMs = static_cast<int>(nc::theApplication().frameTime() * 1000.0f);
		inputManager.joyVibrate(firstJoy, intensities[0], intensities[1], frameTimeMs);
	}
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}
