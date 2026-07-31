#include <SDL3/SDL.h>
#include <nctl/CString.h>

#include "common_macros.h"
#include "SdlInputManager.h"
#include "SdlGfxDevice.h"
#include "IInputEventHandler.h"
#include "Application.h"
#include "JoyMapping.h"

#ifdef WITH_IMGUI
	#include "ImGuiSdlInput.h"
#endif

#ifdef WITH_NUKLEAR
	#include "NuklearSdlInput.h"
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const int IInputManager::MaxNumJoysticks = 16;
const unsigned short int IInputManager::MaxVibrationValue = 0xFFFF;

TouchEvent SdlInputManager::touchEvent_;
SdlMouseState SdlInputManager::mouseState_;
MouseEvent SdlInputManager::mouseEvent_;
SdlScrollEvent SdlInputManager::scrollEvent_;
SdlKeyboardState SdlInputManager::keyboardState_;
KeyboardEvent SdlInputManager::keyboardEvent_;
TextInputEvent SdlInputManager::textInputEvent_;

SDL_Joystick *SdlInputManager::sdlJoysticks_[MaxNumJoysticks];
nctl::StaticArray<SdlJoystickState, SdlInputManager::MaxNumJoysticks> SdlInputManager::joystickStates_(nctl::StaticArrayMode::EXTEND_SIZE);
JoyButtonEvent SdlInputManager::joyButtonEvent_;
JoyHatEvent SdlInputManager::joyHatEvent_;
JoyAxisEvent SdlInputManager::joyAxisEvent_;
JoyConnectionEvent SdlInputManager::joyConnectionEvent_;

int SdlInputManager::joystickIDCount_ = 0;
SDL_JoystickID *SdlInputManager::joystickIDs_ = nullptr;
int SdlInputManager::textInputRefCount_ = 0;
char SdlInputManager::joyGuidString_[33];

namespace {

	int getJoystickIndex(const SDL_JoystickID *joystickIDs, int joystickIDCount, SDL_JoystickID joystickID)
	{
		int joystickIndex = -1;
		if (joystickID != 0)
		{
			for (int i = 0; i < joystickIDCount; i++)
			{
				if (joystickIDs[i] == joystickID)
				{
					joystickIndex = i;
					break;
				}
			}
		}

		return joystickIndex;
	}

	MouseButton sdlToNcineMouseButton(int button)
	{
		if (button == SDL_BUTTON_LEFT)
			return MouseButton::LEFT;
		else if (button == SDL_BUTTON_MIDDLE)
			return MouseButton::MIDDLE;
		else if (button == SDL_BUTTON_RIGHT)
			return MouseButton::RIGHT;
		else if (button == SDL_BUTTON_X1)
			return MouseButton::FOURTH;
		else if (button == SDL_BUTTON_X2)
			return MouseButton::FIFTH;
		else
			return MouseButton::LEFT;
	}

	int ncineToSdlMouseButtonMask(MouseButton button)
	{
		switch (button)
		{
			case MouseButton::LEFT: return SDL_BUTTON_LMASK;
			case MouseButton::MIDDLE: return SDL_BUTTON_MMASK;
			case MouseButton::RIGHT: return SDL_BUTTON_RMASK;
			case MouseButton::FOURTH: return SDL_BUTTON_X1MASK;
			case MouseButton::FIFTH: return SDL_BUTTON_X2MASK;
			default: return SDL_BUTTON_LMASK;
		}
	}

}

///////////////////////////////////////////////////////////
// SdlMouseState
///////////////////////////////////////////////////////////

SdlMouseState::SdlMouseState()
    : currentStateIndex_(0), buttons_{0, 0}
{
}

bool SdlMouseState::isButtonDown(MouseButton button) const
{
	const int sdlButtonMask = ncineToSdlMouseButtonMask(button);
	return (buttons_[currentStateIndex_] & sdlButtonMask) != 0;
}

bool SdlMouseState::isButtonPressed(MouseButton button) const
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	const int sdlButtonMask = ncineToSdlMouseButtonMask(button);
	return ((buttons_[currentStateIndex_] & sdlButtonMask) != 0 && (buttons_[prevStateIndex] & sdlButtonMask) == 0);
}

bool SdlMouseState::isButtonReleased(MouseButton button) const
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	const int sdlButtonMask = ncineToSdlMouseButtonMask(button);
	return ((buttons_[currentStateIndex_] & sdlButtonMask) == 0 && (buttons_[prevStateIndex] & sdlButtonMask) != 0);
}

void SdlMouseState::copyButtonStateToPrev()
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	buttons_[prevStateIndex] = buttons_[currentStateIndex_];
	currentStateIndex_ = prevStateIndex;
}

///////////////////////////////////////////////////////////
// SdlKeyboardState
///////////////////////////////////////////////////////////

SdlKeyboardState::SdlKeyboardState()
    : keyStateArrayLength_(0), keyState_(nullptr)
{
	keyState_ = SDL_GetKeyboardState(&keyStateArrayLength_);
	FATAL_ASSERT(keyStateArrayLength_ <= MaxKeyStateArrayLength);

	for (unsigned int i = 0; i < keyStateArrayLength_; i++)
		prevKeyState_[i] = false;
}

bool SdlKeyboardState::isKeyDown(KeySym key) const
{
	const int sdlKey = SdlKeys::enumToScancode(key);
	if (sdlKey == SDL_SCANCODE_UNKNOWN)
		return false;
	else
		return keyState_[sdlKey];
}

bool SdlKeyboardState::isKeyPressed(KeySym key) const
{
	const int sdlKey = SdlKeys::enumToScancode(key);
	if (sdlKey == SDL_SCANCODE_UNKNOWN)
		return false;
	else
		return (keyState_[sdlKey] == true && prevKeyState_[sdlKey] == false);
}

bool SdlKeyboardState::isKeyReleased(KeySym key) const
{
	const int sdlKey = SdlKeys::enumToScancode(key);
	if (sdlKey == SDL_SCANCODE_UNKNOWN)
		return false;
	else
		return (keyState_[sdlKey] == false && prevKeyState_[sdlKey] == true);
}

void SdlKeyboardState::copyKeyStateToPrev()
{
	for (unsigned int i = 0; i < keyStateArrayLength_; i++)
		prevKeyState_[i] = keyState_[i];
}

///////////////////////////////////////////////////////////
// SdlJoystickState
///////////////////////////////////////////////////////////

SdlJoystickState::SdlJoystickState()
    : sdlJoystick_(nullptr)
{
	for (unsigned int i = 0; i < MaxNumButtons; i++)
		prevButtonState_[i] = false;
}

bool SdlJoystickState::isButtonDown(int buttonId) const
{
	bool isDown = false;
	if (sdlJoystick_ != nullptr)
		isDown = SDL_GetJoystickButton(sdlJoystick_, buttonId);
	return isDown;
}

bool SdlJoystickState::isButtonPressed(int buttonId) const
{
	ASSERT(buttonId < static_cast<int>(MaxNumButtons));
	bool isPressed = false;
	if (buttonId >= 0 && buttonId < static_cast<int>(MaxNumButtons))
		isPressed = (SDL_GetJoystickButton(sdlJoystick_, buttonId) == true && prevButtonState_[buttonId] == false);
	return isPressed;
}

bool SdlJoystickState::isButtonReleased(int buttonId) const
{
	ASSERT(buttonId < static_cast<int>(MaxNumButtons));
	bool isReleased = false;
	if (buttonId >= 0 && buttonId < static_cast<int>(MaxNumButtons))
		isReleased = (SDL_GetJoystickButton(sdlJoystick_, buttonId) == false && prevButtonState_[buttonId] == true);
	return isReleased;
}

unsigned char SdlJoystickState::hatState(int hatId) const
{
	unsigned char hatState = 0;
	if (sdlJoystick_ != nullptr)
		hatState = SDL_GetJoystickHat(sdlJoystick_, hatId);

	return hatState;
}

short int SdlJoystickState::axisValue(int axisId) const
{
	short int axisValue = 0;
	if (sdlJoystick_ != nullptr)
		axisValue = SDL_GetJoystickAxis(sdlJoystick_, axisId);
	return axisValue;
}

float SdlJoystickState::axisNormValue(int axisId) const
{
	// If the joystick is not present the returned value is zero
	const float value = axisValue(axisId) / float(IInputManager::MaxAxisValue);

	return value;
}

void SdlJoystickState::copyButtonStateToPrev()
{
	for (unsigned int i = 0; i < MaxNumButtons; i++)
		prevButtonState_[i] = SDL_GetJoystickButton(sdlJoystick_, i);
}

void SdlJoystickState::resetPrevButtonState()
{
	for (unsigned int i = 0; i < MaxNumButtons; i++)
		prevButtonState_[i] = false;
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

SdlInputManager::SdlInputManager()
{
	const SDL_InitFlags flags = SDL_WasInit(SDL_INIT_VIDEO);
	FATAL_ASSERT_MSG((flags & SDL_INIT_VIDEO) != 0, "SDL video subsystem is not initialized");

	// Initializing the joystick subsystem
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	// Enabling joystick event processing
	SDL_SetJoystickEventsEnabled(true);

	memset(sdlJoysticks_, 0, sizeof(SDL_Joystick *) * MaxNumJoysticks);

	// Opening attached joysticks
	joystickIDs_ = SDL_GetJoysticks(&joystickIDCount_);
	for (int i = 0; i < joystickIDCount_; i++)
	{
		sdlJoysticks_[i] = SDL_OpenJoystick(joystickIDs_[i]);
		if (sdlJoysticks_[i])
		{
			SDL_Joystick *sdlJoy = sdlJoysticks_[i];
			LOGI_X("Joystick %d \"%s\" - %d hats, %d axes, %d buttons, %d balls",
			       i, SDL_GetJoystickName(sdlJoy), SDL_GetNumJoystickHats(sdlJoy), SDL_GetNumJoystickAxes(sdlJoy), SDL_GetNumJoystickButtons(sdlJoy), SDL_GetNumJoystickBalls(sdlJoy));
		}
	}

	joyMapping_.init(this);

#ifdef WITH_IMGUI
	ImGuiSdlInput::init(SdlGfxDevice::windowHandle());
#endif

#ifdef WITH_NUKLEAR
	NuklearSdlInput::init(SdlGfxDevice::windowHandle());
#endif
}

SdlInputManager::~SdlInputManager()
{
#ifdef WITH_NUKLEAR
	NuklearSdlInput::shutdown();
#endif

#ifdef WITH_IMGUI
	ImGuiSdlInput::shutdown();
#endif

	// Close a joystick if opened
	for (unsigned int i = 0; i < MaxNumJoysticks; i++)
	{
		if (isJoyPresent(i))
		{
			SDL_CloseJoystick(sdlJoysticks_[i]);
			sdlJoysticks_[i] = nullptr;
		}
	}

	SDL_free(joystickIDs_);
	joystickIDs_ = nullptr;
	joystickIDCount_ = 0;

	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool SdlInputManager::shouldQuitOnRequest()
{
	bool shouldQuit = true;

	if (inputEventHandler_)
		shouldQuit = inputEventHandler_->onQuitRequest();

	return shouldQuit;
}

void SdlInputManager::acquireTextInput(SDL_Window *window)
{
	if (textInputRefCount_ == 0)
		SDL_StartTextInput(window);
	textInputRefCount_++;
}

void SdlInputManager::releaseTextInput(SDL_Window *window)
{
	ASSERT(textInputRefCount_ > 0);
	textInputRefCount_--;
	if (textInputRefCount_ == 0)
		SDL_StopTextInput(window);
}

void SdlInputManager::copyButtonStatesToPrev()
{
	mouseState_.copyButtonStateToPrev();
	keyboardState_.copyKeyStateToPrev();

	for (unsigned int joyId = 0; joyId < MaxNumJoysticks; joyId++)
		joystickStates_[joyId].copyButtonStateToPrev();
	joyMapping_.copyButtonStateToPrev();
}

void SdlInputManager::parseEvent(const SDL_Event &event)
{
#ifdef WITH_IMGUI
	ImGuiSdlInput::processEvent(&event);
#endif

#ifdef WITH_NUKLEAR
	NuklearSdlInput::processEvent(&event);
#endif

	if (inputEventHandler_ == nullptr)
		return;

	if (event.type == SDL_EVENT_JOYSTICK_ADDED || event.type == SDL_EVENT_JOYSTICK_REMOVED)
	{
		handleJoyDeviceEvent(event);
		return;
	}

	// Filling static event structures
	switch (event.type)
	{
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
			keyboardEvent_.scancode = event.key.scancode;
			keyboardEvent_.sym = SdlKeys::keySymValueToEnum(event.key.key);
			keyboardEvent_.mod = SdlKeys::keyModMaskToEnumMask(event.key.mod);
			break;
		case SDL_EVENT_TEXT_INPUT:
			nctl::strncpy(textInputEvent_.text, event.text.text, 4);
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
			mouseEvent_.x = event.button.x;
			mouseEvent_.y = theApplication().heightInt() - event.button.y;
			mouseEvent_.button = sdlToNcineMouseButton(event.button.button);
			break;
		case SDL_EVENT_MOUSE_MOTION:
			if (mouseCursorMode_ != MouseCursorMode::DISABLED)
			{
				mouseState_.x = event.motion.x;
				mouseState_.y = theApplication().heightInt() - event.motion.y;
			}
			else
			{
				mouseState_.x += event.motion.xrel;
				mouseState_.y -= event.motion.yrel;
			}
			mouseState_.buttons_[mouseState_.currentStateIndex_] = event.motion.state;
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			scrollEvent_.x = static_cast<float>(event.wheel.x);
			scrollEvent_.y = static_cast<float>(event.wheel.y);
			break;
		case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
		case SDL_EVENT_JOYSTICK_BUTTON_UP:
			joyButtonEvent_.joyId = getJoystickIndex(joystickIDs_, joystickIDCount_, event.jbutton.which);
			joyButtonEvent_.buttonId = event.jbutton.button;
			break;
		case SDL_EVENT_JOYSTICK_AXIS_MOTION:
			joyAxisEvent_.joyId = getJoystickIndex(joystickIDs_, joystickIDCount_, event.jaxis.which);
			joyAxisEvent_.axisId = event.jaxis.axis;
			joyAxisEvent_.value = event.jaxis.value;
			joyAxisEvent_.normValue = joyAxisEvent_.value / float(MaxAxisValue);
			break;
		case SDL_EVENT_JOYSTICK_HAT_MOTION:
			joyHatEvent_.joyId = getJoystickIndex(joystickIDs_, joystickIDCount_, event.jhat.which);
			joyHatEvent_.hatId = event.jhat.hat;
			joyHatEvent_.hatState = event.jhat.value;
			break;
		case SDL_EVENT_FINGER_DOWN:
		case SDL_EVENT_FINGER_MOTION:
		case SDL_EVENT_FINGER_UP:
		{
			int fingerCount = 0;
			SDL_Finger **fingers = SDL_GetTouchFingers(event.tfinger.touchID, &fingerCount);
			touchEvent_.count = fingerCount;
			for (int i = 0; i < fingerCount; i++)
			{
				SDL_Finger *finger = fingers[i];
				TouchEvent::Pointer &pointer = touchEvent_.pointers[i];
				pointer.id = static_cast<int>(finger->id);
				pointer.x = theApplication().width() * finger->x;
				pointer.y = theApplication().height() * (1.0f - finger->y);
				pointer.pressure = finger->pressure;
			}
		}
			break;
		case SDL_EVENT_DROP_BEGIN:
			dropEvent_.numPaths = 0;
			break;
		case SDL_EVENT_DROP_TEXT:
		case SDL_EVENT_DROP_FILE:
			// Skip long paths instead of truncating them
			if (dropEvent_.numPaths < DropEvent::MaxNumPaths &&
			    nctl::strnlen(event.drop.data, DropEvent::MaxPathLength) < DropEvent::MaxPathLength - 1)
			{
				nctl::strncpy(dropEvent_.paths[dropEvent_.numPaths], event.drop.data, DropEvent::MaxPathLength);
				dropEvent_.numPaths++;
			}
			break;
		default:
			break;
	}

	// Calling the event handler method
	switch (event.type)
	{
		case SDL_EVENT_KEY_DOWN:
			inputEventHandler_->onKeyPressed(keyboardEvent_);
			break;
		case SDL_EVENT_KEY_UP:
			inputEventHandler_->onKeyReleased(keyboardEvent_);
			break;
		case SDL_EVENT_TEXT_INPUT:
			inputEventHandler_->onTextInput(textInputEvent_);
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			inputEventHandler_->onMouseButtonPressed(mouseEvent_);
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			inputEventHandler_->onMouseButtonReleased(mouseEvent_);
			break;
		case SDL_EVENT_MOUSE_MOTION:
			inputEventHandler_->onMouseMoved(mouseState_);
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			inputEventHandler_->onScrollInput(scrollEvent_);
			break;
		case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
			joyMapping_.onJoyButtonPressed(joyButtonEvent_);
			inputEventHandler_->onJoyButtonPressed(joyButtonEvent_);
			break;
		case SDL_EVENT_JOYSTICK_BUTTON_UP:
			joyMapping_.onJoyButtonReleased(joyButtonEvent_);
			inputEventHandler_->onJoyButtonReleased(joyButtonEvent_);
			break;
		case SDL_EVENT_JOYSTICK_AXIS_MOTION:
			joyMapping_.onJoyAxisMoved(joyAxisEvent_);
			inputEventHandler_->onJoyAxisMoved(joyAxisEvent_);
			break;
		case SDL_EVENT_JOYSTICK_HAT_MOTION:
			joyMapping_.onJoyHatMoved(joyHatEvent_);
			inputEventHandler_->onJoyHatMoved(joyHatEvent_);
			break;
		case SDL_EVENT_FINGER_DOWN:
			if (touchEvent_.count == 0)
				inputEventHandler_->onTouchDown(touchEvent_);
			else
				inputEventHandler_->onPointerDown(touchEvent_);
			break;
		case SDL_EVENT_FINGER_MOTION:
			inputEventHandler_->onTouchMove(touchEvent_);
			break;
		case SDL_EVENT_FINGER_UP:
			if (touchEvent_.count == 0)
				inputEventHandler_->onTouchUp(touchEvent_);
			else
				inputEventHandler_->onPointerUp(touchEvent_);
			break;
		case SDL_EVENT_DROP_COMPLETE:
			if (dropEvent_.numPaths > 0)
				inputEventHandler_->onFilesDropped(dropEvent_);
			break;
		default:
			break;
	}
}

const MouseState &SdlInputManager::mouseState() const
{
	float mouseX = 0.0f;
	float mouseY = 0.0f;
	mouseState_.buttons_[mouseState_.currentStateIndex_] = SDL_GetMouseState(&mouseX, &mouseY);
	mouseState_.x = static_cast<int>(mouseX);
	mouseState_.y = static_cast<int>(mouseY);
	return mouseState_;
}

bool SdlInputManager::isJoyPresent(int joyId) const
{
	ASSERT(joyId >= 0);
	ASSERT_MSG_X(joyId < int(MaxNumJoysticks), "joyId is %d and the maximum is %u", joyId, MaxNumJoysticks - 1);
	if (joyId < 0 || joyId >= int(MaxNumJoysticks))
		return false;

	return (sdlJoysticks_[joyId] && SDL_JoystickConnected(sdlJoysticks_[joyId]));
}

const char *SdlInputManager::joyName(int joyId) const
{
	if (isJoyPresent(joyId))
		return SDL_GetJoystickName(sdlJoysticks_[joyId]);
	else
		return nullptr;
}

const char *SdlInputManager::joyGuid(int joyId) const
{
	if (isJoyPresent(joyId))
	{
#ifndef __EMSCRIPTEN__
		const SDL_GUID joystickGuid = SDL_GetJoystickGUID(sdlJoysticks_[joyId]);
		SDL_GUIDToString(joystickGuid, joyGuidString_, 33);
#else
		memset(joyGuidString_, 0, 33);
		nctl::strncpy(joyGuidString_, "default", 7);
#endif
		return joyGuidString_;
	}
	else
		return nullptr;
}

int SdlInputManager::joyNumButtons(int joyId) const
{
	int numButtons = -1;

	if (isJoyPresent(joyId))
		numButtons = SDL_GetNumJoystickButtons(sdlJoysticks_[joyId]);

	return numButtons;
}

int SdlInputManager::joyNumHats(int joyId) const
{
	int numHats = -1;

	if (isJoyPresent(joyId))
		numHats = SDL_GetNumJoystickHats(sdlJoysticks_[joyId]);

	return numHats;
}

int SdlInputManager::joyNumAxes(int joyId) const
{
	int numAxes = -1;

	if (isJoyPresent(joyId))
		numAxes = SDL_GetNumJoystickAxes(sdlJoysticks_[joyId]);

	return numAxes;
}

const JoystickState &SdlInputManager::joystickState(int joyId) const
{
	joystickStates_[joyId].sdlJoystick_ = nullptr;

	if (isJoyPresent(joyId))
		joystickStates_[joyId].sdlJoystick_ = sdlJoysticks_[joyId];

	return joystickStates_[joyId];
}

bool SdlInputManager::hasJoyVibration(int joyId) const
{
	bool hasVibration = false;

	if (isJoyPresent(joyId))
	{
		const SDL_PropertiesID props = SDL_GetJoystickProperties(sdlJoysticks_[joyId]);
		if (props != 0)
			hasVibration = SDL_GetBooleanProperty(props, SDL_PROP_JOYSTICK_CAP_RUMBLE_BOOLEAN, false);
	}

	return hasVibration;
}

void SdlInputManager::joyVibrate(int joyId, float lowFreqIntensity, float highFreqIntensity, unsigned int duration) const
{
	if (isJoyPresent(joyId))
	{
		// Clamp intensity between 0.0f and 1.0f
		lowFreqIntensity = (lowFreqIntensity < 0.0f) ? 0.0f : lowFreqIntensity;
		lowFreqIntensity = (lowFreqIntensity > 1.0f) ? 1.0f : lowFreqIntensity;
		highFreqIntensity = (highFreqIntensity < 0.0f) ? 0.0f : highFreqIntensity;
		highFreqIntensity = (highFreqIntensity > 1.0f) ? 1.0f : highFreqIntensity;

		const unsigned short int leftIntensity = static_cast<unsigned short int>(lowFreqIntensity * MaxVibrationValue);
		const unsigned short int rightIntensity = static_cast<unsigned short int>(highFreqIntensity * MaxVibrationValue);
		SDL_RumbleJoystick(sdlJoysticks_[joyId], leftIntensity, rightIntensity, duration);
	}
}

void SdlInputManager::setMouseCursorMode(MouseCursorMode mode)
{
	if (mode != mouseCursorMode_)
	{
		bool changeMode = true;
		switch (mode)
		{
			case MouseCursorMode::NORMAL:
				SDL_ShowCursor();
				SDL_SetWindowRelativeMouseMode(SdlGfxDevice::windowHandle(), false);
				break;
			case MouseCursorMode::HIDDEN:
				SDL_HideCursor();
				SDL_SetWindowRelativeMouseMode(SdlGfxDevice::windowHandle(), false);
				break;
			case MouseCursorMode::DISABLED:
				const int supported = SDL_SetWindowRelativeMouseMode(SdlGfxDevice::windowHandle(), true);
				changeMode = (supported == 0);
				break;
		}

		if (changeMode)
		{
			// Handling ImGui cursor changes
			IInputManager::setMouseCursorMode(mode);

			mouseCursorMode_ = mode;
		}
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////

void SdlInputManager::handleJoyDeviceEvent(const SDL_Event &event)
{
	const SDL_JoystickID joystickID = event.jdevice.which;

	if (event.type == SDL_EVENT_JOYSTICK_ADDED)
	{
		SDL_free(joystickIDs_);
		joystickIDs_ = SDL_GetJoysticks(&joystickIDCount_);

		const int deviceIndex = getJoystickIndex(joystickIDs_, joystickIDCount_, joystickID);
		if (deviceIndex < 0)
			return;

		joyConnectionEvent_.joyId = deviceIndex;
		sdlJoysticks_[deviceIndex] = SDL_OpenJoystick(joystickID);

		SDL_Joystick *joy = sdlJoysticks_[deviceIndex];

#ifndef __EMSCRIPTEN__
		const SDL_GUID joystickGuid = SDL_GetJoystickGUID(joy);
		SDL_GUIDToString(joystickGuid, joyGuidString_, 33);
#else
		memset(joyGuidString_, 0, 33);
		nctl::strncpy(joyGuidString_, "default", 7);
#endif

		LOGI_X("Joystick %d \"%s\" (GUID: \"%s\") has been connected - %d hats, %d axes, %d buttons, %d balls",
		       deviceIndex, SDL_GetJoystickName(joy), joyGuidString_, SDL_GetNumJoystickHats(joy), SDL_GetNumJoystickAxes(joy), SDL_GetNumJoystickButtons(joy), SDL_GetNumJoystickBalls(joy));
		joyMapping_.onJoyConnected(joyConnectionEvent_);
		inputEventHandler_->onJoyConnected(joyConnectionEvent_);
	}
	else if (event.type == SDL_EVENT_JOYSTICK_REMOVED)
	{
		const int deviceIndex = getJoystickIndex(joystickIDs_, joystickIDCount_, joystickID);
		if (deviceIndex < 0)
			return;

		joyConnectionEvent_.joyId = deviceIndex;
		SDL_CloseJoystick(sdlJoysticks_[deviceIndex]);
		sdlJoysticks_[deviceIndex] = nullptr;

		// Compacting the array of SDL joystick pointers
		for (int i = deviceIndex; i < MaxNumJoysticks - 1; i++)
			sdlJoysticks_[i] = sdlJoysticks_[i + 1];
		sdlJoysticks_[MaxNumJoysticks - 1] = nullptr;

		SDL_free(joystickIDs_);
		joystickIDs_ = SDL_GetJoysticks(&joystickIDCount_);

		LOGI_X("Joystick %d has been disconnected", deviceIndex);
		inputEventHandler_->onJoyDisconnected(joyConnectionEvent_);
		joyMapping_.onJoyDisconnected(joyConnectionEvent_);
	}
}

}
