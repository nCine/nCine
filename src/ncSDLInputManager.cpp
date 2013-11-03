#include "ncSDLInputManager.h"
#include "ncIInputEventHandler.h"
#include "ncServiceLocator.h"
#include "ncFileLogger.h"
#include "ncApplication.h"
#include <SDL/SDL.h> // for SDL_WasInit()
#include <SDL/SDL_joystick.h>

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

ncIInputEventHandler* ncIInputManager::s_pInputEventHandler = NULL;
ncSDLMouseState ncSDLInputManager::s_mouseState;
ncSDLMouseEvent ncSDLInputManager::s_mouseEvent;
ncSDLKeyboardState ncSDLInputManager::s_keyboardState;
ncKeyboardEvent	ncSDLInputManager::s_keyboardEvent;
short int ncIInputManager::s_iMaxAxisValue = 32767;
SDL_Joystick* ncSDLInputManager::s_pJoysticks[s_uMaxNumJoysticks];
ncJoyButtonEvent ncSDLInputManager::s_joyButtonEvent;
ncJoyAxisEvent ncSDLInputManager::s_joyAxisEvent;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// The constructor takes care of opening available joysticks
ncSDLInputManager::ncSDLInputManager()
{
	if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncSDLInputManager::ncSDLInputManager - SDL video subsystem is not initialized");
		exit(-1);
	}

	// Initializing the joystick subsystem
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	// Enabling joystick event processing
	SDL_JoystickEventState(SDL_ENABLE);

	memset(s_pJoysticks, 0, sizeof(SDL_Joystick *) * s_uMaxNumJoysticks);

	// Opening attached joysticks
	int iNumJoysticks = SDL_NumJoysticks();
	if(iNumJoysticks > 0)
	{
		for(int i = 0; i < iNumJoysticks; i++)
		{
			s_pJoysticks[i] = SDL_JoystickOpen(i);
			if (s_pJoysticks[i])
			{
				SDL_Joystick *pJoy = s_pJoysticks[i];
				ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncSDLInputManager::ncSDLInputManager - Joystick %d: %s - %d hats, %d axes, %d buttons, %d balls",
					 i, SDL_JoystickName(i), SDL_JoystickNumHats(pJoy), SDL_JoystickNumAxes(pJoy), SDL_JoystickNumButtons(pJoy), SDL_JoystickNumBalls(pJoy));
			}
		}
	}
}

/// The destructor releases every opened joystick
ncSDLInputManager::~ncSDLInputManager()
{
	// Close a joystick if opened
	for(unsigned int i = 0; i < s_uMaxNumJoysticks; i++)
	{
		if (isJoyPresent(i))
		{
			SDL_JoystickClose(s_pJoysticks[i]);
			s_pJoysticks[i] = NULL;
		}

	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncSDLInputManager::ParseEvent(const SDL_Event &event)
{
	if (s_pInputEventHandler == NULL)
		return;

	// Filling static event structures
	switch (event.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			s_keyboardEvent.scancode = event.key.keysym.scancode;
			s_keyboardEvent.sym = ncKeySym(event.key.keysym.sym);
			s_keyboardEvent.mod = event.key.keysym.mod;
			s_keyboardEvent.unicode = event.key.keysym.unicode;
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			s_mouseEvent.x = event.button.x;
			s_mouseEvent.y = ncApplication::Height()-event.button.y;
			s_mouseEvent.m_ubButton = event.button.button;
			break;
		case SDL_MOUSEMOTION:
			s_mouseState.x = event.motion.x;
			s_mouseState.y = ncApplication::Height()-event.motion.y;
			s_mouseState.m_ubButtons = event.motion.state;
			break;
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			s_joyButtonEvent.joyId = event.jbutton.which;
			s_joyButtonEvent.buttonId = event.jbutton.button;
			break;
		case SDL_JOYAXISMOTION:
			s_joyAxisEvent.joyId = event.jaxis.which;
			s_joyAxisEvent.axisId = event.jaxis.axis;
			s_joyAxisEvent.value = event.jaxis.value;
			break;
		case SDL_JOYHATMOTION:
			s_joyAxisEvent.joyId = event.jhat.which;
			s_joyAxisEvent.axisId = SDL_JoystickNumAxes(s_pJoysticks[event.jhat.which]) + event.jhat.hat;
			break;
		default:
			break;
	}

	// Calling the event handler method
	switch (event.type) {
		case SDL_KEYDOWN:
			s_pInputEventHandler->OnKeyPressed(s_keyboardEvent);
			break;
		case SDL_KEYUP:
			s_pInputEventHandler->OnKeyReleased(s_keyboardEvent);
			break;
		case SDL_MOUSEBUTTONDOWN:
			s_pInputEventHandler->OnMouseButtonPressed(s_mouseEvent);
			break;
		case SDL_MOUSEBUTTONUP:
			s_pInputEventHandler->OnMouseButtonReleased(s_mouseEvent);
			break;
		case SDL_MOUSEMOTION:
			s_pInputEventHandler->OnMouseMoved(s_mouseState);
			break;
		case SDL_JOYBUTTONDOWN:
			s_pInputEventHandler->OnJoyButtonPressed(s_joyButtonEvent);
			break;
		case SDL_JOYBUTTONUP:
			s_pInputEventHandler->OnJoyButtonReleased(s_joyButtonEvent);
			break;
		case SDL_JOYAXISMOTION:
			s_pInputEventHandler->OnJoyAxisMoved(s_joyAxisEvent);
			break;
		case SDL_JOYHATMOTION:
			// HACK: Always splitting a hat event into two axis ones,
			// even if the value of one of the two axes doesn't change
			s_joyAxisEvent.value = HatEnumToAxisValue(event.jhat.value, false);
			s_pInputEventHandler->OnJoyAxisMoved(s_joyAxisEvent);
			s_joyAxisEvent.axisId++;
			s_joyAxisEvent.value = HatEnumToAxisValue(event.jhat.value, true);
			s_pInputEventHandler->OnJoyAxisMoved(s_joyAxisEvent);
			break;
		default:
			break;
	}
}

bool ncSDLInputManager::isJoyPresent(int iJoyId) const
{
	if(SDL_JoystickOpened(iJoyId) && s_pJoysticks[iJoyId])
		return true;
	else
		return false;
}

int ncSDLInputManager::JoyNumButtons(int iJoyId) const
{
	int iNumButtons = -1;

	if (isJoyPresent(iJoyId))
		iNumButtons = SDL_JoystickNumButtons(s_pJoysticks[iJoyId]);

	return iNumButtons;
}

int ncSDLInputManager::JoyNumAxes(int iJoyId) const
{
	int iNumAxes = -1;

	if (isJoyPresent(iJoyId))
		iNumAxes = SDL_JoystickNumAxes(s_pJoysticks[iJoyId]) + (SDL_JoystickNumHats(s_pJoysticks[iJoyId]) * 2);

	return iNumAxes;
}

bool ncSDLInputManager::isJoyButtonPressed(int iJoyId, int iButtonId) const
{
	if (isJoyPresent(iJoyId))
		return SDL_JoystickGetButton(s_pJoysticks[iJoyId], iButtonId);
	else
		return false;
}

short int ncSDLInputManager::JoyAxisValue(int iJoyId, int iAxisId) const
{
	short int iRetValue = 0;

	if (isJoyPresent(iJoyId))
	{
		int iNumAxes = SDL_JoystickNumAxes(s_pJoysticks[iJoyId]);
		if (iAxisId < iNumAxes) // iAxisId is an analog axis
			iRetValue = SDL_JoystickGetAxis(s_pJoysticks[iJoyId], iAxisId);
		else // iAxisId is a digital d-pad
		{
			int iHatId = (iAxisId-iNumAxes) / 2;
			unsigned char ubHatState = SDL_JoystickGetHat(s_pJoysticks[iJoyId], iHatId);
			bool bUpDownAxis = ((iAxisId-iNumAxes) % 2); // odd axis is left-right, even axis is down-up

			iRetValue = HatEnumToAxisValue(ubHatState, bUpDownAxis);
		}
	}

	return iRetValue;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////

short int ncSDLInputManager::HatEnumToAxisValue(unsigned char ubHatState, bool bUpDownAxis)
{
	short int iRetValue = 0;

	if (bUpDownAxis == false) // odd axis is left-right
	{
		if (ubHatState == SDL_HAT_LEFT || ubHatState == SDL_HAT_LEFTUP || ubHatState == SDL_HAT_LEFTDOWN)
			iRetValue = -s_iMaxAxisValue;
		else if (ubHatState == SDL_HAT_RIGHT || ubHatState == SDL_HAT_RIGHTDOWN || ubHatState == SDL_HAT_RIGHTUP)
			iRetValue = s_iMaxAxisValue;
	}
	else // even axis is down-up
	{
		if (ubHatState == SDL_HAT_DOWN || ubHatState == SDL_HAT_RIGHTDOWN || ubHatState == SDL_HAT_LEFTDOWN)
			iRetValue = -s_iMaxAxisValue;
		else if (ubHatState == SDL_HAT_UP || ubHatState == SDL_HAT_RIGHTUP || ubHatState == SDL_HAT_LEFTUP)
			iRetValue = s_iMaxAxisValue;
	}

	return iRetValue;
}
