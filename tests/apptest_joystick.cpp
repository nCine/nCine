#include "apptest_joystick.h"
#include "Application.h"
#include "ServiceLocator.h"
#include "ncString.h"
#include "Texture.h"
#include "Sprite.h"
#include "Font.h"
#include "TextNode.h"
#include "IFile.h" // for dataPath()

namespace {

#ifdef __ANDROID__
const char* TextureFile = "texture4.webp";
const char* FontTextureFile = "DroidSans32_256_8888.ktx";
#else
const char* TextureFile = "texture4.png";
const char* FontTextureFile = "DroidSans32_256.png";
#endif
const char* FontFntFile = "DroidSans32_256.fnt";

}

nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	joyString_ = new nc::String(NumChars);
	texture_ = new nc::Texture((nc::IFile::dataPath() + "textures/" + TextureFile).data());
	sprites_[0] = new nc::Sprite(&rootNode, texture_, nc::theApplication().width() * 0.25f, nc::theApplication().height() * 0.5f);
	sprites_[1] = new nc::Sprite(&rootNode, texture_, nc::theApplication().width() * 0.75f, nc::theApplication().height() * 0.5f);
	sprites_[0]->setScale(0.5f);
	sprites_[1]->setScale(0.5f);

	font_ = new nc::Font((nc::IFile::dataPath() + "fonts/" + FontTextureFile).data(),
						 (nc::IFile::dataPath() + "fonts/" + FontFntFile).data());
	textNode_ = new nc::TextNode(&rootNode, font_);
	textNode_->setScale(0.85f);
	textNode_->setPosition(nc::theApplication().width() * 0.1f, nc::theApplication().height() * 0.35f);

	for (int i = 0; i < NumJoysticks; i++)
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
	for (int i = 0; i < NumJoysticks; i++)
	{
		if (nc::theApplication().inputManager().isJoyPresent(i))
		{
			firstJoy = i;
			break;
		}
	}

	joyString_->clear();
	for (int i = 0; i < NumJoysticks; i++)
	{
		if (nc::theApplication().inputManager().isJoyPresent(i))
		{
			joyString_->format("Joystick %d: %s (%d axes, %d buttons)\n", i,
				nc::theApplication().inputManager().joyName(i),
				nc::theApplication().inputManager().joyNumAxes(i),
				nc::theApplication().inputManager().joyNumButtons(i));
		}
	}
	*joyString_ += "Axes:";
	for (int i = 0; i < NumAxes; i++)
	{
		axisValues_[i] = nc::theApplication().inputManager().joyAxisNormValue(firstJoy, i);
		joyString_->formatAppend(" %.2f", axisValues_[i]);
	}
	*joyString_ += "\nButtons:";
	for (int i = 0; i < NumButtons; i++)
	{
		buttonStates_[i] = nc::theApplication().inputManager().isJoyButtonPressed(firstJoy, i);
		joyString_->formatAppend(" %u", buttonStates_[i]);
	}
	textNode_->setString(*joyString_);


	float xOffset1 = axisValues_[0] * 0.1f;
	float yOffset1 = -axisValues_[1] * 0.1f;
	float scale1 = 1.0f + 0.5f * axisValues_[2];
	sprites_[0]->setPosition(nc::theApplication().width() * (0.25f + xOffset1), nc::theApplication().height() * (0.5f + yOffset1));
	sprites_[0]->setScale(scale1);

	float xOffset2 = axisValues_[3] * 0.1f;
	float yOffset2 = -axisValues_[4] * 0.1f;
	float scale2 = 1.0f + 0.5f * axisValues_[5];
	sprites_[1]->setPosition(nc::theApplication().width() * (0.75f + xOffset2), nc::theApplication().height() * (0.5f + yOffset2));
	sprites_[1]->setScale(scale2);
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
