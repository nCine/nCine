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

	m_pTexture = new ncTexture("textures/texture3.png");
	m_pSprites[0] = new ncSprite(&rRootNode, m_pTexture, ncApplication::Width()*0.25f, ncApplication::Height()*0.5f);
	m_pSprites[1] = new ncSprite(&rRootNode, m_pTexture, ncApplication::Width()*0.75f, ncApplication::Height()*0.5f);
	m_pSprites[0]->SetScale(0.5f);
	m_pSprites[1]->SetScale(0.5f);

	m_pFont = new ncFont("fonts/trebuchet32_256.png", "fonts/trebuchet32_256.fnt");
	m_pTextNode = new ncTextNode(&rRootNode, m_pFont);
	m_pTextNode->SetScale(0.85f);
	m_pTextNode->SetPosition(ncApplication::Width()*0.1f, ncApplication::Height()*0.35f);

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"Joystick 0 - %d axes, %d buttons",
		ncApplication::InputManager().JoyNumAxes(0), ncApplication::InputManager().JoyNumButtons(0));
}

void MyEventHandler::OnFrameStart()
{
	memset(m_vJoyString, 0, numChars);
	strncat(m_vJoyString, "Axes:", 5);
	for (int i = 0; i < numAxes; i++)
	{
		m_iAxisValues[i] = ncApplication::InputManager().JoyAxisValue(0, i);
		sprintf(&m_vJoyString[strlen(m_vJoyString)], " %d", m_iAxisValues[i]);
	}
	strncat(m_vJoyString, "\nButtons:", 9);
	for (int i = 0; i < numButtons; i++)
	{
		m_ubButtonStates[i] = ncApplication::InputManager().isJoyButtonPressed(0, i);
		sprintf(&m_vJoyString[strlen(m_vJoyString)], " %u", m_ubButtonStates[i]);
	}
	m_pTextNode->SetString(m_vJoyString);


	float fMaxAxisValue = ncApplication::InputManager().s_iMaxAxisValue;
	float fXOffset1 = (m_iAxisValues[0]/fMaxAxisValue)*0.1f;
	float fYOffset1 = -(m_iAxisValues[1]/fMaxAxisValue)*0.1f;
	float fScale1 = 1.0f + 0.5f*(m_iAxisValues[2]/fMaxAxisValue);
	m_pSprites[0]->SetPosition(ncApplication::Width() * (0.25f + fXOffset1), ncApplication::Height() * (0.5f + fYOffset1));
	m_pSprites[0]->SetScale(fScale1);

	float fXOffset2 = (m_iAxisValues[3]/fMaxAxisValue)*0.1f;
	float fYOffset2 = -(m_iAxisValues[4]/fMaxAxisValue)*0.1f;
	float fScale2 = 1.0f + 0.5f*(m_iAxisValues[5]/fMaxAxisValue);
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
		ncApplication::Quit();
	else if (event.sym == NCKEY_SPACE)
		ncApplication::TogglePause();
}
#endif
