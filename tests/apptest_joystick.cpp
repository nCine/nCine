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

ncIAppEventHandler* create_apphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::OnInit()
{
	ncIInputManager::SetHandler(this);
	ncSceneNode &rRootNode = ncApplication::RootNode();

#ifdef __ANDROID__
	m_pTexture = new ncTexture("/sdcard/ncine/checker_256_ETC1_MIP.ktx");
#else
	m_pTexture = new ncTexture("textures/texture3.png");
#endif
	m_pSprites[0] = new ncSprite(&rRootNode, m_pTexture, ncApplication::Width() * 0.25f, ncApplication::Height() * 0.5f);
	m_pSprites[1] = new ncSprite(&rRootNode, m_pTexture, ncApplication::Width() * 0.75f, ncApplication::Height() * 0.5f);
	m_pSprites[0]->SetScale(0.5f);
	m_pSprites[1]->SetScale(0.5f);

#ifdef __ANDROID__
	m_pFont = new ncFont("/sdcard/ncine/trebuchet32_256_4444.pvr", "/sdcard/ncine/trebuchet32_256.fnt");
#else
	m_pFont = new ncFont("fonts/trebuchet32_256.png", "fonts/trebuchet32_256.fnt");
#endif
	m_pTextNode = new ncTextNode(&rRootNode, m_pFont);
	m_pTextNode->SetScale(0.85f);
	m_pTextNode->SetPosition(ncApplication::Width() * 0.1f, ncApplication::Height() * 0.35f);

	for (int i = 0; i < numJoysticks; i++)
	{
		if (ncApplication::InputManager().isJoyPresent(i))
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"Joystick %d (%s) - %d axes, %d buttons", i,
				ncApplication::InputManager().JoyName(i), ncApplication::InputManager().JoyNumAxes(i), ncApplication::InputManager().JoyNumButtons(i));
		}
	}
}

void MyEventHandler::OnFrameStart()
{
	int iFirstJoy = 0;
	for (int i = 0; i < numJoysticks; i++)
	{
		if (ncApplication::InputManager().isJoyPresent(i))
		{
			iFirstJoy = i;
			break;
		}
	}

	memset(m_vJoyString, 0, numChars);
	for (int i = 0; i < numJoysticks; i++)
	{
		if (ncApplication::InputManager().isJoyPresent(i))
		{
			sprintf(&m_vJoyString[strlen(m_vJoyString)], "Joystick %d: %s (%d axes, %d buttons)\n", i,
				ncApplication::InputManager().JoyName(i), ncApplication::InputManager().JoyNumAxes(i), ncApplication::InputManager().JoyNumButtons(i));
		}
	}
	strncat(m_vJoyString, "Axes:", 5);
	for (int i = 0; i < numAxes; i++)
	{
		m_fAxisValues[i] = ncApplication::InputManager().JoyAxisNormValue(iFirstJoy, i);
		sprintf(&m_vJoyString[strlen(m_vJoyString)], " %.2f", m_fAxisValues[i]);
	}
	strncat(m_vJoyString, "\nButtons:", 9);
	for (int i = 0; i < numButtons; i++)
	{
		m_ubButtonStates[i] = ncApplication::InputManager().isJoyButtonPressed(iFirstJoy, i);
		sprintf(&m_vJoyString[strlen(m_vJoyString)], " %u", m_ubButtonStates[i]);
	}
	m_pTextNode->SetString(m_vJoyString);


	float fXOffset1 = m_fAxisValues[0] * 0.1f;
	float fYOffset1 = -m_fAxisValues[1] * 0.1f;
	float fScale1 = 1.0f + 0.5f * m_fAxisValues[2];
	m_pSprites[0]->SetPosition(ncApplication::Width() * (0.25f + fXOffset1), ncApplication::Height() * (0.5f + fYOffset1));
	m_pSprites[0]->SetScale(fScale1);

	float fXOffset2 = m_fAxisValues[3] * 0.1f;
	float fYOffset2 = -m_fAxisValues[4] * 0.1f;
	float fScale2 = 1.0f + 0.5f * m_fAxisValues[5];
	m_pSprites[1]->SetPosition(ncApplication::Width() * (0.75f + fXOffset2), ncApplication::Height() * (0.5f + fYOffset2));
	m_pSprites[1]->SetScale(fScale2);
}

void MyEventHandler::OnShutdown()
{
	delete m_pTextNode;
	delete m_pFont;
	delete m_pSprites[0];
	delete m_pSprites[1];
	delete m_pTexture;
}

#ifndef __ANDROID__
void MyEventHandler::OnKeyReleased(const ncKeyboardEvent &event)
{
	if (event.sym == NCKEY_ESCAPE || event.sym == NCKEY_Q)
	{
		ncApplication::Quit();
	}
	else if (event.sym == NCKEY_SPACE)
	{
		ncApplication::TogglePause();
	}
}
#endif
