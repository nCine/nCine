#include "apptest_joystick.h"
#include "Application.h"
#include "ncString.h"
#include "Texture.h"
#include "Sprite.h"
#include "TextNode.h"
#include "IFile.h" // for dataPath()
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const char *TextureFile = "texture4.webp";
const char *FontTextureFile = "DroidSans32_256.webp";
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

	joyString_ = new nc::String(MaxNumChars);
	texture_ = new nc::Texture((nc::IFile::dataPath() + "textures/" + TextureFile).data());

	const float appWidth = nc::theApplication().width();
	const float appHeight = nc::theApplication().height();

	joyNode_ = new nc::SceneNode(&rootNode, appWidth * 0.5f, appHeight * 0.65f);
	joyNode_->setScale(1.33f);
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		sprites_[i] = new nc::Sprite(joyNode_, texture_);
	}
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

	font_ = new nc::Font((nc::IFile::dataPath() + "fonts/" + FontTextureFile).data(),
	                     (nc::IFile::dataPath() + "fonts/" + FontFntFile).data());
	textNode_ = new nc::TextNode(&rootNode, font_, MaxNumChars);
	textNode_->setScale(0.85f);

	for (int i = 0; i < nc::IInputManager::MaxNumJoysticks; i++)
	{
		if (nc::theApplication().inputManager().isJoyPresent(i))
		{
			LOGI_X("Joystick %d (%s) - %d axes, %d buttons", i,
			       nc::theApplication().inputManager().joyName(i),
			       nc::theApplication().inputManager().joyNumAxes(i),
			       nc::theApplication().inputManager().joyNumButtons(i));
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
				bool isMapped = nc::theApplication().inputManager().isJoyMapped(i);

				joyString_->formatAppend("Joystick %d is %s: \"%s\" (%d axes, %d buttons)\n", i, isMapped ? "mapped" : "unmapped", joyName, numAxes, numButtons);
			}
		}

		if (nc::theApplication().inputManager().isJoyPresent(firstJoy))
		{
			int numAxes = nc::theApplication().inputManager().joyNumAxes(firstJoy);
			int numButtons = nc::theApplication().inputManager().joyNumButtons(firstJoy);
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
		}
	}
	else
	{
		joyString_->formatAppend("No joysticks connected");
	}
	textNode_->setString(*joyString_);
	textNode_->setPosition(appWidth * 0.1f + textNode_->width() * 0.5f, appHeight * 0.38f - textNode_->height() * 0.5f);

	const nc::JoyMappedState &joyMappedState = nc::theApplication().inputManager().joyMappedState(firstJoy);
	nc::Vector2f joyVectorLeft(joyMappedState.axisValue(nc::AXIS_LX), joyMappedState.axisValue(nc::AXIS_LY));
	nc::Vector2f joyVectorRight(joyMappedState.axisValue(nc::AXIS_RX), joyMappedState.axisValue(nc::AXIS_RY));
	nc::theApplication().inputManager().deadZoneNormalize(joyVectorLeft, nc::IInputManager::LeftStickDeadZone);
	nc::theApplication().inputManager().deadZoneNormalize(joyVectorRight, nc::IInputManager::RightStickDeadZone);
	bool lPressed = joyMappedState.isButtonPressed(nc::BUTTON_LSTICK);
	bool rPressed = joyMappedState.isButtonPressed(nc::BUTTON_RSTICK);

	sprites_[0]->setPosition(appWidth * (-triggerPosX + joyVectorLeft.x * stickMovement), appWidth * -joyVectorLeft.y * stickMovement);
	sprites_[1]->setPosition(appWidth * (triggerPosX * 0.5f + joyVectorRight.x * stickMovement), appWidth * (-0.075f - joyVectorRight.y * stickMovement));
	sprites_[0]->setScale(lPressed ? stickPressedSize : stickUnpressedSize);
	sprites_[1]->setScale(rPressed ? stickPressedSize : stickUnpressedSize);

	bool bxPressed = joyMappedState.isButtonPressed(nc::BUTTON_X);
	bool byPressed = joyMappedState.isButtonPressed(nc::BUTTON_Y);
	bool baPressed = joyMappedState.isButtonPressed(nc::BUTTON_A);
	bool bbPressed = joyMappedState.isButtonPressed(nc::BUTTON_B);
	sprites_[2]->setScale(bxPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[3]->setScale(byPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[4]->setScale(baPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[5]->setScale(bbPressed ? buttonPressedSize : buttonUnpressedSize);

	bool lsPressed = joyMappedState.isButtonPressed(nc::BUTTON_LBUMPER);
	bool rsPressed = joyMappedState.isButtonPressed(nc::BUTTON_RBUMPER);
	sprites_[6]->setScale(lsPressed ? buttonPressedSize : buttonUnpressedSize);
	sprites_[7]->setScale(rsPressed ? buttonPressedSize : buttonUnpressedSize);

	float joyLeftTrigger = joyMappedState.axisValue(nc::AXIS_LTRIGGER);
	float joyRightTrigger = joyMappedState.axisValue(nc::AXIS_RTRIGGER);
	sprites_[8]->setScale(buttonUnpressedSize + (joyLeftTrigger * 0.1f));
	sprites_[9]->setScale(buttonUnpressedSize + (joyRightTrigger * 0.1f));

	bool backPressed = joyMappedState.isButtonPressed(nc::BUTTON_BACK);
	bool guidePressed = joyMappedState.isButtonPressed(nc::BUTTON_GUIDE);
	bool startPressed = joyMappedState.isButtonPressed(nc::BUTTON_START);
	sprites_[10]->setScale(backPressed ? startPressedSize : startUnpressedSize);
	sprites_[11]->setScale(guidePressed ? guidePressedSize : guideUnpressedSize);
	sprites_[12]->setScale(startPressed ? startPressedSize : startUnpressedSize);
}

void MyEventHandler::onShutdown()
{
	delete textNode_;
	delete font_;
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		delete sprites_[i];
	}
	delete joyNode_;
	delete texture_;
	delete joyString_;
}

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
