#include <SDL/SDL.h> // for SDL_WasInit()
#include <SDL/SDL_joystick.h>

#include "SdlInputManager.h"
#include "IInputEventHandler.h"
#include "ServiceLocator.h"
#include "FileLogger.h"
#include "Application.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

IInputEventHandler* IInputManager::inputEventHandler_ = NULL;
SdlMouseState SdlInputManager::mouseState_;
SdlMouseEvent SdlInputManager::mouseEvent_;
SdlKeyboardState SdlInputManager::keyboardState_;
KeyboardEvent	SdlInputManager::keyboardEvent_;
short int IInputManager::MaxAxisValue = 32767;
SDL_Joystick* SdlInputManager::sdlJoysticks_[MaxNumJoysticks];
JoyButtonEvent SdlInputManager::joyButtonEvent_;
JoyAxisEvent SdlInputManager::joyAxisEvent_;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// The constructor takes care of opening available joysticks
SdlInputManager::SdlInputManager()
{
	if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
	{
		LOGF("SDL video subsystem is not initialized");
		exit(-1);
	}

	// Initializing the joystick subsystem
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	// Enabling joystick event processing
	SDL_JoystickEventState(SDL_ENABLE);

	memset(sdlJoysticks_, 0, sizeof(SDL_Joystick *) * MaxNumJoysticks);

	// Opening attached joysticks
	int numJoysticks = SDL_NumJoysticks();
	if (numJoysticks > 0)
	{
		for (int i = 0; i < numJoysticks; i++)
		{
			sdlJoysticks_[i] = SDL_JoystickOpen(i);
			if (sdlJoysticks_[i])
			{
				SDL_Joystick *sdlJoy = sdlJoysticks_[i];
				LOGI_X("Joystick %d: %s - %d hats, %d axes, %d buttons, %d balls",
					 i, SDL_JoystickName(i), SDL_JoystickNumHats(sdlJoy), SDL_JoystickNumAxes(sdlJoy), SDL_JoystickNumButtons(sdlJoy), SDL_JoystickNumBalls(sdlJoy));
			}
		}
	}
}

/// The destructor releases every opened joystick
SdlInputManager::~SdlInputManager()
{
	// Close a joystick if opened
	for (unsigned int i = 0; i < MaxNumJoysticks; i++)
	{
		if (isJoyPresent(i))
		{
			SDL_JoystickClose(sdlJoysticks_[i]);
			sdlJoysticks_[i] = NULL;
		}

	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void SdlInputManager::parseEvent(const SDL_Event &event)
{
	if (inputEventHandler_ == NULL)
	{
		return;
	}

	// Filling static event structures
	switch (event.type)
	{
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			keyboardEvent_.scancode = event.key.keysym.scancode;
			keyboardEvent_.sym = keySymValueToEnum(event.key.keysym.sym);
			keyboardEvent_.mod = keyModValueToEnum(event.key.keysym.mod);
			keyboardEvent_.unicode = event.key.keysym.unicode;
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			mouseEvent_.x = event.button.x;
			mouseEvent_.y = theApplication().height() - event.button.y;
			mouseEvent_.button_ = event.button.button;
			break;
		case SDL_MOUSEMOTION:
			mouseState_.x = event.motion.x;
			mouseState_.y = theApplication().height() - event.motion.y;
			mouseState_.buttons_ = event.motion.state;
			break;
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			joyButtonEvent_.joyId = event.jbutton.which;
			joyButtonEvent_.buttonId = event.jbutton.button;
			break;
		case SDL_JOYAXISMOTION:
			joyAxisEvent_.joyId = event.jaxis.which;
			joyAxisEvent_.axisId = event.jaxis.axis;
			joyAxisEvent_.value = event.jaxis.value;
			joyAxisEvent_.normValue = joyAxisEvent_.value / float(MaxAxisValue);
			break;
		case SDL_JOYHATMOTION:
			joyAxisEvent_.joyId = event.jhat.which;
			joyAxisEvent_.axisId = SDL_JoystickNumAxes(sdlJoysticks_[event.jhat.which]) + event.jhat.hat;
			break;
		default:
			break;
	}

	// Calling the event handler method
	switch (event.type)
	{
		case SDL_KEYDOWN:
			inputEventHandler_->onKeyPressed(keyboardEvent_);
			break;
		case SDL_KEYUP:
			inputEventHandler_->onKeyReleased(keyboardEvent_);
			break;
		case SDL_MOUSEBUTTONDOWN:
			inputEventHandler_->onMouseButtonPressed(mouseEvent_);
			break;
		case SDL_MOUSEBUTTONUP:
			inputEventHandler_->onMouseButtonReleased(mouseEvent_);
			break;
		case SDL_MOUSEMOTION:
			inputEventHandler_->onMouseMoved(mouseState_);
			break;
		case SDL_JOYBUTTONDOWN:
			inputEventHandler_->onJoyButtonPressed(joyButtonEvent_);
			break;
		case SDL_JOYBUTTONUP:
			inputEventHandler_->onJoyButtonReleased(joyButtonEvent_);
			break;
		case SDL_JOYAXISMOTION:
			inputEventHandler_->onJoyAxisMoved(joyAxisEvent_);
			break;
		case SDL_JOYHATMOTION:
			// HACK: Always splitting a hat event into two axis ones,
			// even if the value of one of the two axes doesn't change
			joyAxisEvent_.value = hatEnumToAxisValue(event.jhat.value, false);
			joyAxisEvent_.normValue = joyAxisEvent_.value / float(MaxAxisValue);
			inputEventHandler_->onJoyAxisMoved(joyAxisEvent_);
			joyAxisEvent_.axisId++;
			joyAxisEvent_.value = hatEnumToAxisValue(event.jhat.value, true);
			joyAxisEvent_.normValue = joyAxisEvent_.value / float(MaxAxisValue);
			inputEventHandler_->onJoyAxisMoved(joyAxisEvent_);
			break;
		default:
			break;
	}
}

bool SdlInputManager::isJoyPresent(int joyId) const
{
	if (joyId >= 0 && joyId < int(MaxNumJoysticks) && SDL_JoystickOpened(joyId) && sdlJoysticks_[joyId])
	{
		return true;
	}
	else
	{
		return false;
	}
}

const char *SdlInputManager::joyName(int joyId) const
{
	if (isJoyPresent(joyId))
	{
		return SDL_JoystickName(joyId);
	}
	else
	{
		return '\0';
	}
}

int SdlInputManager::joyNumButtons(int joyId) const
{
	int numButtons = -1;

	if (isJoyPresent(joyId))
	{
		numButtons = SDL_JoystickNumButtons(sdlJoysticks_[joyId]);
	}

	return numButtons;
}

int SdlInputManager::joyNumAxes(int joyId) const
{
	int numAxes = -1;

	if (isJoyPresent(joyId))
	{
		numAxes = SDL_JoystickNumAxes(sdlJoysticks_[joyId]) + (SDL_JoystickNumHats(sdlJoysticks_[joyId]) * 2);
	}

	return numAxes;
}

bool SdlInputManager::isJoyButtonPressed(int joyId, int buttonId) const
{
	if (isJoyPresent(joyId))
	{
		return SDL_JoystickGetButton(sdlJoysticks_[joyId], buttonId);
	}
	else
	{
		return false;
	}
}

short int SdlInputManager::joyAxisValue(int joyId, int axisId) const
{
	short int axisValue = 0;

	if (isJoyPresent(joyId))
	{
		int numAxes = SDL_JoystickNumAxes(sdlJoysticks_[joyId]);
		if (axisId < numAxes) // axisId is an analog axis
		{
			axisValue = SDL_JoystickGetAxis(sdlJoysticks_[joyId], axisId);
		}
		else // axisId is a digital d-pad
		{
			int hatId = (axisId - numAxes) / 2;
			unsigned char hatState = SDL_JoystickGetHat(sdlJoysticks_[joyId], hatId);
			bool upDownAxis = ((axisId - numAxes) % 2); // odd axis is left-right, even axis is down-up

			axisValue = hatEnumToAxisValue(hatState, upDownAxis);
		}
	}

	return axisValue;
}

float SdlInputManager::joyAxisNormValue(int joyId, int axisId) const
{
	// If the joystick is not present the returned value is zero
	float axisValue = joyAxisValue(joyId, axisId) / float(MaxAxisValue);

	return axisValue;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////

short int SdlInputManager::hatEnumToAxisValue(unsigned char hatState, bool upDownAxis)
{
	short int axisValue = 0;

	if (upDownAxis == false) // odd axis is left-right
	{
		if (hatState == SDL_HAT_LEFT || hatState == SDL_HAT_LEFTUP || hatState == SDL_HAT_LEFTDOWN)
		{
			axisValue = -MaxAxisValue;
		}
		else if (hatState == SDL_HAT_RIGHT || hatState == SDL_HAT_RIGHTDOWN || hatState == SDL_HAT_RIGHTUP)
		{
			axisValue = MaxAxisValue;
		}
	}
	else // even axis is down-up
	{
		if (hatState == SDL_HAT_DOWN || hatState == SDL_HAT_RIGHTDOWN || hatState == SDL_HAT_LEFTDOWN)
		{
			axisValue = -MaxAxisValue;
		}
		else if (hatState == SDL_HAT_UP || hatState == SDL_HAT_RIGHTUP || hatState == SDL_HAT_LEFTUP)
		{
			axisValue = MaxAxisValue;
		}
	}

	return axisValue;
}

}
