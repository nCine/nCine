#include <cstring> // for memset()
#include <SDL.h>
#include <nctl/CString.h>

#include "common_macros.h"
#include "SdlInputManager.h"
#include "IInputEventHandler.h"
#include "FileLogger.h"
#include "Application.h"
#include "JoyMapping.h"

#ifdef WITH_IMGUI
	#include "SdlGfxDevice.h"
	#include "ImGuiSdlInput.h"
#endif

#ifdef WITH_NUKLEAR
	#include "SdlGfxDevice.h"
	#include "NuklearSdlInput.h"
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const int IInputManager::MaxNumJoysticks = 16;

SDL_Window *SdlInputManager::windowHandle_ = nullptr;

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

char SdlInputManager::joyGuidString_[33];

namespace {

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

	memset(prevKeyState_, 0, keyStateArrayLength_);
}

bool SdlKeyboardState::isKeyDown(KeySym key) const
{
	const int sdlKey = SdlKeys::enumToScancode(key);
	if (sdlKey == SDL_SCANCODE_UNKNOWN)
		return false;
	else
		return keyState_[sdlKey] != 0;
}

bool SdlKeyboardState::isKeyPressed(KeySym key) const
{
	const int sdlKey = SdlKeys::enumToScancode(key);
	if (sdlKey == SDL_SCANCODE_UNKNOWN)
		return false;
	else
		return (keyState_[sdlKey] != 0 && prevKeyState_[sdlKey] == 0);
}

bool SdlKeyboardState::isKeyReleased(KeySym key) const
{
	const int sdlKey = SdlKeys::enumToScancode(key);
	if (sdlKey == SDL_SCANCODE_UNKNOWN)
		return false;
	else
		return (keyState_[sdlKey] == 0 && prevKeyState_[sdlKey] != 0);
}

void SdlKeyboardState::copyKeyStateToPrev()
{
	memcpy(prevKeyState_, keyState_, keyStateArrayLength_ * sizeof(unsigned char));
}

///////////////////////////////////////////////////////////
// SdlJoystickState
///////////////////////////////////////////////////////////

SdlJoystickState::SdlJoystickState()
    : sdlJoystick_(nullptr)
{
	memset(prevButtonState_, 0, MaxNumButtons * sizeof(unsigned char));
}

bool SdlJoystickState::isButtonDown(int buttonId) const
{
	bool isDown = false;
	if (sdlJoystick_ != nullptr)
		isDown = SDL_JoystickGetButton(sdlJoystick_, buttonId) != 0;
	return isDown;
}

bool SdlJoystickState::isButtonPressed(int buttonId) const
{
	ASSERT(buttonId < static_cast<int>(MaxNumButtons));
	bool isPressed = false;
	if (buttonId >= 0 && buttonId < static_cast<int>(MaxNumButtons))
		isPressed = (SDL_JoystickGetButton(sdlJoystick_, buttonId) != 0 && prevButtonState_[buttonId] == 0);
	return isPressed;
}

bool SdlJoystickState::isButtonReleased(int buttonId) const
{
	ASSERT(buttonId < static_cast<int>(MaxNumButtons));
	bool isReleased = false;
	if (buttonId >= 0 && buttonId < static_cast<int>(MaxNumButtons))
		isReleased = (SDL_JoystickGetButton(sdlJoystick_, buttonId) == 0 && prevButtonState_[buttonId] != 0);
	return isReleased;
}

unsigned char SdlJoystickState::hatState(int hatId) const
{
	unsigned char hatState = 0;
	if (sdlJoystick_ != nullptr)
		hatState = SDL_JoystickGetHat(sdlJoystick_, hatId);

	return hatState;
}

short int SdlJoystickState::axisValue(int axisId) const
{
	short int axisValue = 0;
	if (sdlJoystick_ != nullptr)
		axisValue = SDL_JoystickGetAxis(sdlJoystick_, axisId);
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
		prevButtonState_[i] = SDL_JoystickGetButton(sdlJoystick_, i);
}

void SdlJoystickState::resetPrevButtonState()
{
	memset(prevButtonState_, 0, MaxNumButtons);
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

SdlInputManager::SdlInputManager()
{
	const unsigned int ret = SDL_WasInit(SDL_INIT_VIDEO);
	FATAL_ASSERT_MSG(ret != 0, "SDL video subsystem is not initialized");

	// Initializing the joystick subsystem
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	// Enabling joystick event processing
	SDL_JoystickEventState(SDL_ENABLE);

	memset(sdlJoysticks_, 0, sizeof(SDL_Joystick *) * MaxNumJoysticks);

	// Opening attached joysticks
	const int numJoysticks = SDL_NumJoysticks();
	for (int i = 0; i < numJoysticks; i++)
	{
		sdlJoysticks_[i] = SDL_JoystickOpen(i);
		if (sdlJoysticks_[i])
		{
			SDL_Joystick *sdlJoy = sdlJoysticks_[i];
			LOGI_X("Joystick %d \"%s\" - %d hats, %d axes, %d buttons, %d balls",
			       i, SDL_JoystickName(sdlJoy), SDL_JoystickNumHats(sdlJoy), SDL_JoystickNumAxes(sdlJoy), SDL_JoystickNumButtons(sdlJoy), SDL_JoystickNumBalls(sdlJoy));
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
			SDL_JoystickClose(sdlJoysticks_[i]);
			sdlJoysticks_[i] = nullptr;
		}
	}
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

	if (event.type == SDL_JOYDEVICEADDED || event.type == SDL_JOYDEVICEREMOVED)
	{
		handleJoyDeviceEvent(event);
		return;
	}

	// Filling static event structures
	switch (event.type)
	{
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			keyboardEvent_.scancode = event.key.keysym.scancode;
			keyboardEvent_.sym = SdlKeys::keySymValueToEnum(event.key.keysym.sym);
			keyboardEvent_.mod = SdlKeys::keyModMaskToEnumMask(event.key.keysym.mod);
			break;
		case SDL_TEXTINPUT:
			nctl::strncpy(textInputEvent_.text, event.text.text, 4);
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			mouseEvent_.x = event.button.x;
			mouseEvent_.y = theApplication().heightInt() - event.button.y;
			mouseEvent_.button = sdlToNcineMouseButton(event.button.button);
			break;
		case SDL_MOUSEMOTION:
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
		case SDL_MOUSEWHEEL:
			scrollEvent_.x = static_cast<float>(event.wheel.x);
			scrollEvent_.y = static_cast<float>(event.wheel.y);
			break;
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			joyButtonEvent_.joyId = joyInstanceIdToDeviceIndex(event.jbutton.which);
			joyButtonEvent_.buttonId = event.jbutton.button;
			break;
		case SDL_JOYAXISMOTION:
			joyAxisEvent_.joyId = joyInstanceIdToDeviceIndex(event.jaxis.which);
			joyAxisEvent_.axisId = event.jaxis.axis;
			joyAxisEvent_.value = event.jaxis.value;
			joyAxisEvent_.normValue = joyAxisEvent_.value / float(MaxAxisValue);
			break;
		case SDL_JOYHATMOTION:
			joyHatEvent_.joyId = joyInstanceIdToDeviceIndex(event.jhat.which);
			joyHatEvent_.hatId = event.jhat.hat;
			joyHatEvent_.hatState = event.jhat.value;
			break;
		case SDL_FINGERDOWN:
		case SDL_FINGERMOTION:
		case SDL_FINGERUP:
			touchEvent_.count = SDL_GetNumTouchFingers(event.tfinger.touchId);
			for (unsigned int i = 0; i < touchEvent_.count; i++)
			{
				SDL_Finger *finger = SDL_GetTouchFinger(event.tfinger.touchId, i);
				TouchEvent::Pointer &pointer = touchEvent_.pointers[i];
				pointer.id = static_cast<int>(finger->id);
				pointer.x = theApplication().width() * finger->x;
				pointer.y = theApplication().height() * (1.0f - finger->y);
				pointer.pressure = finger->pressure;
			}
			break;
		case SDL_DROPBEGIN:
			dropEvent_.numPaths = 0;
			break;
		case SDL_DROPTEXT:
		case SDL_DROPFILE:
			// Skip long paths instead of truncating them
			if (dropEvent_.numPaths < DropEvent::MaxNumPaths &&
			    nctl::strnlen(event.drop.file, DropEvent::MaxPathLength) < DropEvent::MaxPathLength - 1)
			{
				nctl::strncpy(dropEvent_.paths[dropEvent_.numPaths], event.drop.file, DropEvent::MaxPathLength);
				dropEvent_.numPaths++;
			}
			SDL_free(event.drop.file);
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
		case SDL_TEXTINPUT:
			inputEventHandler_->onTextInput(textInputEvent_);
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
		case SDL_MOUSEWHEEL:
			inputEventHandler_->onScrollInput(scrollEvent_);
			break;
		case SDL_JOYBUTTONDOWN:
			joyMapping_.onJoyButtonPressed(joyButtonEvent_);
			inputEventHandler_->onJoyButtonPressed(joyButtonEvent_);
			break;
		case SDL_JOYBUTTONUP:
			joyMapping_.onJoyButtonReleased(joyButtonEvent_);
			inputEventHandler_->onJoyButtonReleased(joyButtonEvent_);
			break;
		case SDL_JOYAXISMOTION:
			joyMapping_.onJoyAxisMoved(joyAxisEvent_);
			inputEventHandler_->onJoyAxisMoved(joyAxisEvent_);
			break;
		case SDL_JOYHATMOTION:
			joyMapping_.onJoyHatMoved(joyHatEvent_);
			inputEventHandler_->onJoyHatMoved(joyHatEvent_);
			break;
		case SDL_FINGERDOWN:
			if (touchEvent_.count == 0)
				inputEventHandler_->onTouchDown(touchEvent_);
			else
				inputEventHandler_->onPointerDown(touchEvent_);
			break;
		case SDL_FINGERMOTION:
			inputEventHandler_->onTouchMove(touchEvent_);
			break;
		case SDL_FINGERUP:
			if (touchEvent_.count == 0)
				inputEventHandler_->onTouchUp(touchEvent_);
			else
				inputEventHandler_->onPointerUp(touchEvent_);
			break;
		case SDL_DROPCOMPLETE:
			if (dropEvent_.numPaths > 0)
				inputEventHandler_->onFilesDropped(dropEvent_);
			break;
		default:
			break;
	}
}

bool SdlInputManager::isJoyPresent(int joyId) const
{
	ASSERT(joyId >= 0);
	ASSERT_MSG_X(joyId < int(MaxNumJoysticks), "joyId is %d and the maximum is %u", joyId, MaxNumJoysticks - 1);

	if (sdlJoysticks_[joyId] && SDL_JoystickGetAttached(sdlJoysticks_[joyId]))
		return true;
	else
		return false;
}

const char *SdlInputManager::joyName(int joyId) const
{
	if (isJoyPresent(joyId))
		return SDL_JoystickName(sdlJoysticks_[joyId]);
	else
		return nullptr;
}

const char *SdlInputManager::joyGuid(int joyId) const
{
	if (isJoyPresent(joyId))
	{
#ifndef __EMSCRIPTEN__
		const SDL_JoystickGUID joystickGuid = SDL_JoystickGetGUID(sdlJoysticks_[joyId]);
		SDL_JoystickGetGUIDString(joystickGuid, joyGuidString_, 33);
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
		numButtons = SDL_JoystickNumButtons(sdlJoysticks_[joyId]);

	return numButtons;
}

int SdlInputManager::joyNumHats(int joyId) const
{
	int numHats = -1;

	if (isJoyPresent(joyId))
		numHats = SDL_JoystickNumHats(sdlJoysticks_[joyId]);

	return numHats;
}

int SdlInputManager::joyNumAxes(int joyId) const
{
	int numAxes = -1;

	if (isJoyPresent(joyId))
		numAxes = SDL_JoystickNumAxes(sdlJoysticks_[joyId]);

	return numAxes;
}

const JoystickState &SdlInputManager::joystickState(int joyId) const
{
	joystickStates_[joyId].sdlJoystick_ = nullptr;

	if (isJoyPresent(joyId))
		joystickStates_[joyId].sdlJoystick_ = sdlJoysticks_[joyId];

	return joystickStates_[joyId];
}

void SdlInputManager::setMouseCursorMode(MouseCursorMode mode)
{
	if (mode != mouseCursorMode_)
	{
		bool changeMode = true;
		switch (mode)
		{
			case MouseCursorMode::NORMAL:
				SDL_ShowCursor(SDL_ENABLE);
				SDL_SetRelativeMouseMode(SDL_FALSE);
				break;
			case MouseCursorMode::HIDDEN:
				SDL_ShowCursor(SDL_DISABLE);
				SDL_SetRelativeMouseMode(SDL_FALSE);
				break;
			case MouseCursorMode::DISABLED:
				const int supported = SDL_SetRelativeMouseMode(SDL_TRUE);
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
	if (event.type == SDL_JOYDEVICEADDED)
	{
		const int deviceIndex = event.jdevice.which;
		joyConnectionEvent_.joyId = deviceIndex;
		sdlJoysticks_[deviceIndex] = SDL_JoystickOpen(deviceIndex);

		SDL_Joystick *joy = sdlJoysticks_[deviceIndex];

#ifndef __EMSCRIPTEN__
		const SDL_JoystickGUID joystickGuid = SDL_JoystickGetGUID(joy);
		SDL_JoystickGetGUIDString(joystickGuid, joyGuidString_, 33);
#else
		memset(joyGuidString_, 0, 33);
		nctl::strncpy(joyGuidString_, "default", 7);
#endif

		LOGI_X("Joystick %d \"%s\" (GUID: \"%s\") has been connected - %d hats, %d axes, %d buttons, %d balls",
		       deviceIndex, SDL_JoystickName(joy), joyGuidString_, SDL_JoystickNumHats(joy), SDL_JoystickNumAxes(joy), SDL_JoystickNumButtons(joy), SDL_JoystickNumBalls(joy));
		joyMapping_.onJoyConnected(joyConnectionEvent_);
		inputEventHandler_->onJoyConnected(joyConnectionEvent_);
	}
	else if (event.type == SDL_JOYDEVICEREMOVED)
	{
		const int deviceIndex = joyInstanceIdToDeviceIndex(event.jdevice.which);
		if (deviceIndex == -1)
			return;

		joyConnectionEvent_.joyId = deviceIndex;
		SDL_JoystickClose(sdlJoysticks_[deviceIndex]);
		sdlJoysticks_[deviceIndex] = nullptr;

		// Compacting the array of SDL joystick pointers
		for (int i = deviceIndex; i < MaxNumJoysticks - 1; i++)
			sdlJoysticks_[i] = sdlJoysticks_[i + 1];
		sdlJoysticks_[MaxNumJoysticks - 1] = nullptr;

		LOGI_X("Joystick %d has been disconnected", deviceIndex);
		inputEventHandler_->onJoyDisconnected(joyConnectionEvent_);
		joyMapping_.onJoyDisconnected(joyConnectionEvent_);
	}
}

int SdlInputManager::joyInstanceIdToDeviceIndex(SDL_JoystickID instanceId)
{
	int deviceIndex = -1;
	for (int i = 0; i < MaxNumJoysticks; i++)
	{
		SDL_JoystickID id = SDL_JoystickInstanceID(sdlJoysticks_[i]);
		if (instanceId == id)
		{
			deviceIndex = i;
			break;
		}
	}
	return deviceIndex;
}

}
