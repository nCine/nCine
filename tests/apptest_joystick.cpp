#include "apptest_joystick.h"
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

	sprites_[0]->setPosition(appWidth * -triggerPosX, 0.0f); // Left stick
	sprites_[1]->setPosition(appWidth * triggerPosX * 0.5f, -appWidth * 0.075f); // Right stick
	sprites_[0]->setScale(stickUnpressedSize);
	sprites_[1]->setScale(stickUnpressedSize);

	sprites_[2]->setPosition(appWidth * (triggerPosX - 0.02f), appWidth * 0.0f); // X button
	sprites_[3]->setPosition(appWidth * triggerPosX, appWidth * 0.02f); // Y button
	sprites_[4]->setPosition(appWidth * triggerPosX, appWidth * -0.02f); // A button
	sprites_[5]->setPosition(appWidth * (triggerPosX + 0.02f), appWidth * 0.0f); // B button
	sprites_[2]->setScale(buttonUnpressedSize);
	sprites_[3]->setScale(buttonUnpressedSize);
	sprites_[4]->setScale(buttonUnpressedSize);
	sprites_[5]->setScale(buttonUnpressedSize);

	sprites_[6]->setPosition(appWidth * -triggerPosX, appWidth * 0.07f); // Left shoulder
	sprites_[7]->setPosition(appWidth * triggerPosX, appWidth * 0.07f); // Right shoulder
	sprites_[8]->setPosition(appWidth * -triggerPosX, appWidth * 0.1f); // Left trigger
	sprites_[9]->setPosition(appWidth * triggerPosX, appWidth * 0.1f); // Right trigger
	sprites_[6]->setScale(buttonUnpressedSize);
	sprites_[7]->setScale(buttonUnpressedSize);
	sprites_[8]->setScale(buttonUnpressedSize);
	sprites_[9]->setScale(buttonUnpressedSize);

	sprites_[10]->setPosition(appWidth * -0.03f, 0.0f); // Back button
	sprites_[11]->setPosition(0.0f, 0.0f); // Guide button
	sprites_[12]->setPosition(appWidth * 0.03f, 0.0f); // Start button
	sprites_[10]->setScale(startUnpressedSize);
	sprites_[11]->setScale(guideUnpressedSize);
	sprites_[12]->setScale(startUnpressedSize);

	sprites_[13]->setPosition(appWidth * (-triggerPosX * 0.25f - 0.02f), -appWidth * 0.075f); // D-Pad Left
	sprites_[14]->setPosition(appWidth * -triggerPosX * 0.25f, -appWidth * (0.075f - 0.02f)); // D-Pad Up
	sprites_[15]->setPosition(appWidth * -triggerPosX * 0.25f, -appWidth * (0.075f + 0.02f)); // D-Pad Down
	sprites_[16]->setPosition(appWidth * (-triggerPosX * 0.25f + 0.02f), -appWidth * 0.075f); // D-Pad Right
	sprites_[13]->setScale(buttonUnpressedSize);
	sprites_[14]->setScale(buttonUnpressedSize);
	sprites_[15]->setScale(buttonUnpressedSize);
	sprites_[16]->setScale(buttonUnpressedSize);

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
				int numAxes = nc::theApplication().inputManager().joyNumAxes(i);
				int numButtons = nc::theApplication().inputManager().joyNumButtons(i);
				int numHats = nc::theApplication().inputManager().joyNumHats(i);
				bool isMapped = nc::theApplication().inputManager().isJoyMapped(i);

				joyString_->formatAppend("Joystick %d is %s: \"%s\" (%d axes, %d buttons, %d hats)\n", i, isMapped ? "mapped" : "unmapped", joyName, numAxes, numButtons, numHats);
			}
		}

		if (nc::theApplication().inputManager().isJoyPresent(firstJoy))
		{
			int numAxes = nc::theApplication().inputManager().joyNumAxes(firstJoy);
			int numButtons = nc::theApplication().inputManager().joyNumButtons(firstJoy);
			int numHats = nc::theApplication().inputManager().joyNumHats(firstJoy);
			const nc::JoystickState &joyState = nc::theApplication().inputManager().joystickState(firstJoy);

			*joyString_ += "Axes:";
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
		}
	}
	else
		joyString_->formatAppend("No joysticks connected");

	textNode_->setString(*joyString_);
	textNode_->setPosition(appWidth * 0.1f + textNode_->width() * 0.5f, appHeight * 0.38f - textNode_->height() * 0.5f);

	const nc::JoyMappedState &joyMappedState = nc::theApplication().inputManager().joyMappedState(firstJoy);
	nc::Vector2f joyVectorLeft(joyMappedState.axisValue(nc::AxisName::LX), joyMappedState.axisValue(nc::AxisName::LY));
	nc::Vector2f joyVectorRight(joyMappedState.axisValue(nc::AxisName::RX), joyMappedState.axisValue(nc::AxisName::RY));
	nc::theApplication().inputManager().deadZoneNormalize(joyVectorLeft, nc::IInputManager::LeftStickDeadZone);
	nc::theApplication().inputManager().deadZoneNormalize(joyVectorRight, nc::IInputManager::RightStickDeadZone);
	const bool lPressed = joyMappedState.isButtonPressed(nc::ButtonName::LSTICK);
	const bool rPressed = joyMappedState.isButtonPressed(nc::ButtonName::RSTICK);

	sprites_[0]->setPosition(appWidth * (-triggerPosX + joyVectorLeft.x * stickMovement), appWidth * -joyVectorLeft.y * stickMovement);
	sprites_[1]->setPosition(appWidth * (triggerPosX * 0.5f + joyVectorRight.x * stickMovement), appWidth * (-0.075f - joyVectorRight.y * stickMovement));
	sprites_[0]->setScale(lPressed ? stickPressedSize : stickUnpressedSize);
	sprites_[1]->setScale(rPressed ? stickPressedSize : stickUnpressedSize);

	const bool bxPressed = joyMappedState.isButtonPressed(nc::ButtonName::X);
	const bool byPressed = joyMappedState.isButtonPressed(nc::ButtonName::Y);
	const bool baPressed = joyMappedState.isButtonPressed(nc::ButtonName::A);
	const bool bbPressed = joyMappedState.isButtonPressed(nc::ButtonName::B);
	sprites_[2]->setScale(bxPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[3]->setScale(byPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[4]->setScale(baPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[5]->setScale(bbPressed ? buttonPressedSize : buttonUnpressedSize);

	const bool lsPressed = joyMappedState.isButtonPressed(nc::ButtonName::LBUMPER);
	const bool rsPressed = joyMappedState.isButtonPressed(nc::ButtonName::RBUMPER);
	sprites_[6]->setScale(lsPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[7]->setScale(rsPressed ? buttonPressedSize : buttonUnpressedSize);

	const float joyLeftTrigger = joyMappedState.axisValue(nc::AxisName::LTRIGGER);
	const float joyRightTrigger = joyMappedState.axisValue(nc::AxisName::RTRIGGER);
	sprites_[8]->setScale(buttonUnpressedSize + (joyLeftTrigger * 0.1f));
	sprites_[9]->setScale(buttonUnpressedSize + (joyRightTrigger * 0.1f));

	const bool backPressed = joyMappedState.isButtonPressed(nc::ButtonName::BACK);
	const bool guidePressed = joyMappedState.isButtonPressed(nc::ButtonName::GUIDE);
	const bool startPressed = joyMappedState.isButtonPressed(nc::ButtonName::START);
	sprites_[10]->setScale(backPressed ? startPressedSize : startUnpressedSize);
	sprites_[11]->setScale(guidePressed ? guidePressedSize : guideUnpressedSize);
	sprites_[12]->setScale(startPressed ? startPressedSize : startUnpressedSize);

	const bool dpadLeftPressed = joyMappedState.isButtonPressed(nc::ButtonName::DPAD_LEFT);
	const bool dpadUpPressed = joyMappedState.isButtonPressed(nc::ButtonName::DPAD_UP);
	const bool dpadDownPressed = joyMappedState.isButtonPressed(nc::ButtonName::DPAD_DOWN);
	const bool dpadRighgtPressed = joyMappedState.isButtonPressed(nc::ButtonName::DPAD_RIGHT);
	sprites_[13]->setScale(dpadLeftPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[14]->setScale(dpadUpPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[15]->setScale(dpadDownPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[16]->setScale(dpadRighgtPressed ? buttonPressedSize : buttonUnpressedSize);
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
	else if (event.sym == nc::KeySym::SPACE)
	{
		const bool isSuspended = nc::theApplication().isSuspended();
		nc::theApplication().setSuspended(!isSuspended);
	}
}
