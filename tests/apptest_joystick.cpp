#include "apptest_joystick.h"
#include "Application.h"
#include "ServiceLocator.h"
#include "ncString.h"
#include "Texture.h"
#include "Sprite.h"
#include "Font.h"
#include "TextNode.h"
#include "IFile.h" // for dataPath()
#include "apptest_datapath.h"
#include "apptest_joymapping.h"

namespace {

#ifdef __ANDROID__
	const char *TextureFile = "texture4.webp";
	const char *FontTextureFile = "DroidSans32_256_8888.ktx";
#else
	const char *TextureFile = "texture4.png";
	const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

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
	sprites_[0] = new nc::Sprite(&rootNode, texture_, nc::theApplication().width() * 0.25f, nc::theApplication().height() * 0.5f);
	sprites_[1] = new nc::Sprite(&rootNode, texture_, nc::theApplication().width() * 0.75f, nc::theApplication().height() * 0.5f);
	sprites_[0]->setScale(0.5f);
	sprites_[1]->setScale(0.5f);

	font_ = new nc::Font((nc::IFile::dataPath() + "fonts/" + FontTextureFile).data(),
	                     (nc::IFile::dataPath() + "fonts/" + FontFntFile).data());
	textNode_ = new nc::TextNode(&rootNode, font_);
	textNode_->setScale(0.85f);
	textNode_->setPosition(nc::theApplication().width() * 0.1f, nc::theApplication().height() * 0.38f);

	for (int i = 0; i < MaxNumJoysticks; i++)
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
	int firstJoy = 0;
	for (int i = 0; i < MaxNumJoysticks; i++)
	{
		if (nc::theApplication().inputManager().isJoyPresent(i))
		{
			firstJoy = i;
			break;
		}
	}

	joyString_->clear();
	for (int i = 0; i < MaxNumJoysticks; i++)
	{
		if (nc::theApplication().inputManager().isJoyPresent(i))
		{
			joyString_->formatAppend("Joystick %d: %s (%d axes, %d buttons)\n", i,
			                         nc::theApplication().inputManager().joyName(i),
			                         nc::theApplication().inputManager().joyNumAxes(i),
			                         nc::theApplication().inputManager().joyNumButtons(i));
		}
	}

	if (nc::theApplication().inputManager().isJoyPresent(firstJoy))
	{
		int numAxes = nc::theApplication().inputManager().joyNumAxes(firstJoy);
		int numButtons = nc::theApplication().inputManager().joyNumButtons(firstJoy);

		*joyString_ += "Axes:";
		for (int i = 0; i < numAxes; i++)
		{
			axisValues_[i] = nc::theApplication().inputManager().joyAxisNormValue(firstJoy, i);
			joyString_->formatAppend(" %.2f", axisValues_[i]);
		}
		*joyString_ += "\nButtons:";
		for (int i = 0; i < numButtons; i++)
		{
			buttonStates_[i] = nc::theApplication().inputManager().isJoyButtonPressed(firstJoy, i);
			joyString_->formatAppend(" %u", buttonStates_[i]);
		}
	}
	textNode_->setString(*joyString_);


	nc::Vector2f joyLeftStick(mappedAxisNormValue (firstJoy, AXIS_LX), mappedAxisNormValue (firstJoy, AXIS_LY));
	nc::Vector2f joyRightStick(mappedAxisNormValue (firstJoy, AXIS_RX), mappedAxisNormValue (firstJoy, AXIS_RY));
	float joyLeftTrigger = mappedAxisNormValue(firstJoy, AXIS_LTRIGGER);
	float joyRightTrigger = mappedAxisNormValue(firstJoy, AXIS_RTRIGGER);
	deadZoneNormalize(joyLeftStick, LeftStickDeadZone);
	deadZoneNormalize(joyRightStick, RightStickDeadZone);

	sprites_[0]->setPosition(nc::theApplication().width() * (0.25f + joyLeftStick.x * 0.1f), nc::theApplication().height() * (0.5f - joyLeftStick.y * 0.1f));
	sprites_[0]->setScale(1.0f + joyLeftTrigger * 0.5f);
	sprites_[1]->setPosition(nc::theApplication().width() * (0.75f + joyRightStick.x * 0.1f), nc::theApplication().height() * (0.5f - joyRightStick.y * 0.1f));
	sprites_[1]->setScale(1.0f + joyRightTrigger * 0.5f);
}

void MyEventHandler::onShutdown()
{
	delete textNode_;
	delete font_;
	delete sprites_[0];
	delete sprites_[1];
	delete texture_;
	delete joyString_;
}

#ifndef __ANDROID__
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
#endif
