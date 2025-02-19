#include <cstring> // for memset() and memcpy()
#include <cmath> // for fabsf()
#include <nctl/CString.h>
#include "GlfwInputManager.h"
#include "IInputEventHandler.h"
#include "IAppEventHandler.h"
#include "Application.h"
#include "JoyMapping.h"

#ifdef WITH_IMGUI
	#include "ImGuiGlfwInput.h"
#endif

#ifdef WITH_NUKLEAR
	#include "NuklearGlfwInput.h"
#endif

#define GLFW_VERSION_COMBINED (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 + GLFW_VERSION_REVISION)

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const int IInputManager::MaxNumJoysticks = GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1 + 1;

bool GlfwInputManager::windowHasFocus_ = true;
GlfwMouseState GlfwInputManager::mouseState_;
MouseEvent GlfwInputManager::mouseEvent_;
GlfwScrollEvent GlfwInputManager::scrollEvent_;
GlfwKeyboardState GlfwInputManager::keyboardState_;
KeyboardEvent GlfwInputManager::keyboardEvent_;
TextInputEvent GlfwInputManager::textInputEvent_;

GlfwJoystickState GlfwInputManager::nullJoystickState_;
nctl::StaticArray<GlfwJoystickState, GlfwInputManager::MaxNumJoysticks> GlfwInputManager::joystickStates_(nctl::StaticArrayMode::EXTEND_SIZE);
JoyButtonEvent GlfwInputManager::joyButtonEvent_;
JoyHatEvent GlfwInputManager::joyHatEvent_;
JoyAxisEvent GlfwInputManager::joyAxisEvent_;
JoyConnectionEvent GlfwInputManager::joyConnectionEvent_;
const float GlfwInputManager::JoystickEventsSimulator::AxisEventTolerance = 0.001f;
GlfwInputManager::JoystickEventsSimulator GlfwInputManager::joyEventsSimulator_;

int GlfwInputManager::preScalingWidth_ = 0;
int GlfwInputManager::preScalingHeight_ = 0;
unsigned long int GlfwInputManager::lastFrameWindowSizeChanged_ = 0;

namespace {

	MouseButton glfwToNcineMouseButton(int button)
	{
		switch (button)
		{
			case GLFW_MOUSE_BUTTON_LEFT: return MouseButton::LEFT;
			case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::MIDDLE;
			case GLFW_MOUSE_BUTTON_RIGHT: return MouseButton::RIGHT;
			case GLFW_MOUSE_BUTTON_4: return MouseButton::FOURTH;
			case GLFW_MOUSE_BUTTON_5: return MouseButton::FIFTH;
			default: return MouseButton::LEFT;
		}
	}

	int ncineToGlfwMouseButton(MouseButton button)
	{
		switch (button)
		{
			case MouseButton::LEFT: return GLFW_MOUSE_BUTTON_LEFT;
			case MouseButton::MIDDLE: return GLFW_MOUSE_BUTTON_MIDDLE;
			case MouseButton::RIGHT: return GLFW_MOUSE_BUTTON_RIGHT;
			case MouseButton::FOURTH: return GLFW_MOUSE_BUTTON_4;
			case MouseButton::FIFTH: return GLFW_MOUSE_BUTTON_5;
			default: return GLFW_MOUSE_BUTTON_LEFT;
		}
	}

}

///////////////////////////////////////////////////////////
// GlfwMouseState
///////////////////////////////////////////////////////////

GlfwMouseState::GlfwMouseState()
{
	memset(prevButtonState_, GLFW_RELEASE, MouseState::NumButtons * sizeof(unsigned char));
}

bool GlfwMouseState::isButtonDown(MouseButton button) const
{
	const int glfwButton = ncineToGlfwMouseButton(button);
	return glfwGetMouseButton(GlfwGfxDevice::windowHandle(), glfwButton) == GLFW_PRESS;
}

bool GlfwMouseState::isButtonPressed(MouseButton button) const
{
	const unsigned int buttonIndex = static_cast<unsigned int>(button);
	const int glfwButton = ncineToGlfwMouseButton(button);
	return (glfwGetMouseButton(GlfwGfxDevice::windowHandle(), glfwButton) == GLFW_PRESS && prevButtonState_[buttonIndex] == GLFW_RELEASE);
}

bool GlfwMouseState::isButtonReleased(MouseButton button) const
{
	const unsigned int buttonIndex = static_cast<unsigned int>(button);
	const int glfwButton = ncineToGlfwMouseButton(button);
	return (glfwGetMouseButton(GlfwGfxDevice::windowHandle(), glfwButton) == GLFW_RELEASE && prevButtonState_[buttonIndex] == GLFW_PRESS);
}

void GlfwMouseState::copyButtonStateToPrev()
{
	for (unsigned int i = 0; i < MouseState::NumButtons; i++)
	{
		const int glfwButton = ncineToGlfwMouseButton(static_cast<MouseButton>(i));
		prevButtonState_[i] = glfwGetMouseButton(GlfwGfxDevice::windowHandle(), glfwButton);
	}
}

///////////////////////////////////////////////////////////
// GlfwKeyboardState
///////////////////////////////////////////////////////////

GlfwKeyboardState::GlfwKeyboardState()
{
	memset(prevKeyState_, GLFW_RELEASE, NumKeys * sizeof(unsigned char));
}

bool GlfwKeyboardState::isKeyDown(KeySym key) const
{
	const int glfwKey = GlfwKeys::enumToKeySymValue(key);
	if (glfwKey == GLFW_KEY_UNKNOWN)
		return false;
	else
		return glfwGetKey(GlfwGfxDevice::windowHandle(), glfwKey) == GLFW_PRESS;
}

bool GlfwKeyboardState::isKeyPressed(KeySym key) const
{
	const unsigned int keyIndex = static_cast<unsigned int>(key);
	const int glfwKey = GlfwKeys::enumToKeySymValue(key);
	if (glfwKey == GLFW_KEY_UNKNOWN)
		return false;
	else
		return (glfwGetKey(GlfwGfxDevice::windowHandle(), glfwKey) == GLFW_PRESS && prevKeyState_[keyIndex] == GLFW_RELEASE);
}

bool GlfwKeyboardState::isKeyReleased(KeySym key) const
{
	const unsigned int keyIndex = static_cast<unsigned int>(key);
	const int glfwKey = GlfwKeys::enumToKeySymValue(key);
	if (glfwKey == GLFW_KEY_UNKNOWN)
		return false;
	else
		return (glfwGetKey(GlfwGfxDevice::windowHandle(), glfwKey) == GLFW_RELEASE && prevKeyState_[keyIndex] == GLFW_PRESS);
}

void GlfwKeyboardState::copyKeyStateToPrev()
{
	for (unsigned int i = 0; i < NumKeys; i++)
	{
		const int glfwKey = GlfwKeys::enumToKeySymValue(static_cast<KeySym>(i));
		if (glfwKey != GLFW_KEY_UNKNOWN)
			prevKeyState_[i] = glfwGetKey(GlfwGfxDevice::windowHandle(), glfwKey);
	}
}

///////////////////////////////////////////////////////////
// GlfwJoystickState
///////////////////////////////////////////////////////////

GlfwJoystickState::GlfwJoystickState()
    : numButtons_(0), numHats_(0), numAxes_(0), buttons_(nullptr), hats_(nullptr), axesValues_(nullptr)
{
	memset(prevButtonState_, GLFW_RELEASE, numButtons_ * sizeof(unsigned char));
}

bool GlfwJoystickState::isButtonDown(int buttonId) const
{
	ASSERT(buttonId < static_cast<int>(MaxNumButtons));
	bool isDown = false;
	if (buttonId >= 0 && buttonId < numButtons_ && buttonId < static_cast<int>(MaxNumButtons))
		isDown = (buttons_[buttonId] != GLFW_RELEASE);
	return isDown;
}

bool GlfwJoystickState::isButtonPressed(int buttonId) const
{
	ASSERT(buttonId < static_cast<int>(MaxNumButtons));
	bool isPressed = false;
	if (buttonId >= 0 && buttonId < numButtons_ && buttonId < static_cast<int>(MaxNumButtons))
		isPressed = (buttons_[buttonId] != GLFW_RELEASE && prevButtonState_[buttonId] == GLFW_RELEASE);
	return isPressed;
}

bool GlfwJoystickState::isButtonReleased(int buttonId) const
{
	ASSERT(buttonId < static_cast<int>(MaxNumButtons));
	bool isReleased = false;
	if (buttonId >= 0 && buttonId < numButtons_ && buttonId < static_cast<int>(MaxNumButtons))
		isReleased = (buttons_[buttonId] == GLFW_RELEASE && prevButtonState_[buttonId] != GLFW_RELEASE);
	return isReleased;
}

unsigned char GlfwJoystickState::hatState(int hatId) const
{
	unsigned char hatState = HatState::CENTERED;
	if (hatId >= 0 && hatId < numHats_)
		hatState = hats_[hatId];
	return hatState;
}

short int GlfwJoystickState::axisValue(int axisId) const
{
	// If the joystick is not present the returned value is zero
	const short int axisValue = static_cast<short int>(axisNormValue(axisId) * IInputManager::MaxAxisValue);

	return axisValue;
}

float GlfwJoystickState::axisNormValue(int axisId) const
{
	float axisValue = 0.0f;
	if (axisId >= 0 && axisId < numAxes_)
		axisValue = axesValues_[axisId];
	return axisValue;
}

void GlfwJoystickState::copyButtonStateToPrev()
{
	memcpy(prevButtonState_, buttons_, numButtons_);
}

void GlfwJoystickState::resetPrevButtonState()
{
	memset(prevButtonState_, GLFW_RELEASE, numButtons_);
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GlfwInputManager::GlfwInputManager()
{
	GlfwGfxDevice &gfxDevice = static_cast<GlfwGfxDevice &>(theApplication().gfxDevice());
	preScalingWidth_ = gfxDevice.width_;
	preScalingHeight_ = gfxDevice.height_;

	glfwSetMonitorCallback(monitorCallback);
	glfwSetWindowCloseCallback(GlfwGfxDevice::windowHandle(), windowCloseCallback);
#if GLFW_VERSION_COMBINED >= 3300
	glfwSetWindowContentScaleCallback(GlfwGfxDevice::windowHandle(), windowContentScaleCallback);
#endif
	glfwSetWindowSizeCallback(GlfwGfxDevice::windowHandle(), windowSizeCallback);
	glfwSetFramebufferSizeCallback(GlfwGfxDevice::windowHandle(), framebufferSizeCallback);
	glfwSetKeyCallback(GlfwGfxDevice::windowHandle(), keyCallback);
	glfwSetCharCallback(GlfwGfxDevice::windowHandle(), charCallback);
	glfwSetCursorPosCallback(GlfwGfxDevice::windowHandle(), cursorPosCallback);
	glfwSetMouseButtonCallback(GlfwGfxDevice::windowHandle(), mouseButtonCallback);
	glfwSetScrollCallback(GlfwGfxDevice::windowHandle(), scrollCallback);
	glfwSetJoystickCallback(joystickCallback);
	glfwSetDropCallback(GlfwGfxDevice::windowHandle(), dropCallback);

	joyMapping_.init(this);

#ifdef WITH_IMGUI
	ImGuiGlfwInput::init(GlfwGfxDevice::windowHandle(), true);
#endif

#ifdef WITH_NUKLEAR
	NuklearGlfwInput::init(GlfwGfxDevice::windowHandle(), true);
#endif
}

GlfwInputManager::~GlfwInputManager()
{
#ifdef WITH_NUKLEAR
	NuklearGlfwInput::shutdown();
#endif

#ifdef WITH_IMGUI
	ImGuiGlfwInput::shutdown();
#endif
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool GlfwInputManager::hasFocus()
{
	const bool glfwFocused = (glfwGetWindowAttrib(GlfwGfxDevice::windowHandle(), GLFW_FOCUSED) != 0);

	// A focus event has occurred (either gain or loss)
	if (windowHasFocus_ != glfwFocused)
		windowHasFocus_ = glfwFocused;

	return windowHasFocus_;
}

void GlfwInputManager::copyButtonStatesToPrev()
{
	mouseState_.copyButtonStateToPrev();
	keyboardState_.copyKeyStateToPrev();

	for (unsigned int joyId = 0; joyId < MaxNumJoysticks; joyId++)
		joystickStates_[joyId].copyButtonStateToPrev();
	joyMapping_.copyButtonStateToPrev();
}

void GlfwInputManager::updateJoystickStates()
{
	for (unsigned int joyId = 0; joyId < MaxNumJoysticks; joyId++)
	{
		if (glfwJoystickPresent(GLFW_JOYSTICK_1 + joyId))
		{
			joystickStates_[joyId].buttons_ = glfwGetJoystickButtons(joyId, &joystickStates_[joyId].numButtons_);
#if GLFW_VERSION_COMBINED >= 3300
			joystickStates_[joyId].hats_ = glfwGetJoystickHats(joyId, &joystickStates_[joyId].numHats_);
#else
			joystickStates_[joyId].hats_ = 0;
#endif
			joystickStates_[joyId].axesValues_ = glfwGetJoystickAxes(joyId, &joystickStates_[joyId].numAxes_);

			joyEventsSimulator_.simulateButtonsEvents(joyId, joystickStates_[joyId].numButtons_, joystickStates_[joyId].buttons_);
			joyEventsSimulator_.simulateHatsEvents(joyId, joystickStates_[joyId].numHats_, joystickStates_[joyId].hats_);
			joyEventsSimulator_.simulateAxesEvents(joyId, joystickStates_[joyId].numAxes_, joystickStates_[joyId].axesValues_);
		}
	}
}

bool GlfwInputManager::isJoyPresent(int joyId) const
{
	ASSERT(joyId >= 0);
	ASSERT_MSG_X(GLFW_JOYSTICK_1 + joyId <= GLFW_JOYSTICK_LAST,
	             "joyId is %d and the maximum is %d", joyId, GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1);
	return (glfwJoystickPresent(GLFW_JOYSTICK_1 + joyId) != 0);
}

const char *GlfwInputManager::joyName(int joyId) const
{
	if (isJoyPresent(joyId))
		return glfwGetJoystickName(joyId);
	else
		return nullptr;
}

const char *GlfwInputManager::joyGuid(int joyId) const
{
#ifdef __EMSCRIPTEN__
	return "default";
#elif GLFW_VERSION_COMBINED >= 3300
	if (isJoyPresent(joyId))
		return glfwGetJoystickGUID(joyId);
	else
		return nullptr;
#else
	return nullptr;
#endif
}

int GlfwInputManager::joyNumButtons(int joyId) const
{
	int numButtons = -1;

	if (isJoyPresent(joyId))
		glfwGetJoystickButtons(GLFW_JOYSTICK_1 + joyId, &numButtons);

	return numButtons;
}

int GlfwInputManager::joyNumHats(int joyId) const
{
	int numHats = -1;

	if (isJoyPresent(joyId))
#if GLFW_VERSION_COMBINED >= 3300
		glfwGetJoystickHats(GLFW_JOYSTICK_1 + joyId, &numHats);
#else
		numHats = 0;
#endif

	return numHats;
}

int GlfwInputManager::joyNumAxes(int joyId) const
{
	int numAxes = -1;

	if (isJoyPresent(joyId))
		glfwGetJoystickAxes(GLFW_JOYSTICK_1 + joyId, &numAxes);

	return numAxes;
}

const JoystickState &GlfwInputManager::joystickState(int joyId) const
{
	if (isJoyPresent(joyId))
		return joystickStates_[joyId];
	else
		return nullJoystickState_;
}

void GlfwInputManager::setMouseCursorMode(MouseCursorMode mode)
{
	if (mode != mouseCursorMode_)
	{
		switch (mode)
		{
			case MouseCursorMode::NORMAL: glfwSetInputMode(GlfwGfxDevice::windowHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL); break;
			case MouseCursorMode::HIDDEN: glfwSetInputMode(GlfwGfxDevice::windowHandle(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN); break;
			case MouseCursorMode::DISABLED: glfwSetInputMode(GlfwGfxDevice::windowHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED); break;
		}

#if GLFW_VERSION_COMBINED >= 3300
		// Enable raw mouse motion (if supported) when disabling the cursor
		const bool enableRawMouseMotion = (mode == MouseCursorMode::DISABLED && glfwRawMouseMotionSupported() == GLFW_TRUE);
		glfwSetInputMode(GlfwGfxDevice::windowHandle(), GLFW_RAW_MOUSE_MOTION, enableRawMouseMotion ? GLFW_TRUE : GLFW_FALSE);
#endif

		// Handling ImGui cursor changes
		IInputManager::setMouseCursorMode(mode);

		mouseCursorMode_ = mode;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GlfwInputManager::monitorCallback(GLFWmonitor *monitor, int event)
{
	GlfwGfxDevice &gfxDevice = static_cast<GlfwGfxDevice &>(theApplication().gfxDevice());
	gfxDevice.updateMonitors();
}

void GlfwInputManager::windowCloseCallback(GLFWwindow *window)
{
	bool shouldQuit = true;
	if (inputEventHandler_)
		shouldQuit = inputEventHandler_->onQuitRequest();

	if (shouldQuit)
		theApplication().quit();
	else
		glfwSetWindowShouldClose(window, GLFW_FALSE);
}

void GlfwInputManager::windowContentScaleCallback(GLFWwindow *window, float xscale, float yscale)
{
	GlfwGfxDevice &gfxDevice = static_cast<GlfwGfxDevice &>(theApplication().gfxDevice());

	// Revert the window size change if it happened the same frame its content scale also changed
	if (lastFrameWindowSizeChanged_ == theApplication().numFrames())
	{
		gfxDevice.width_ = preScalingWidth_;
		gfxDevice.height_ = preScalingHeight_;
	}

	gfxDevice.updateMonitorScaling(gfxDevice.windowMonitorIndex());
	const float scalingFactor = (xscale + yscale) * 0.5f;
	theApplication().appEventHandler_->onChangeScalingFactor(scalingFactor);
}

void GlfwInputManager::windowSizeCallback(GLFWwindow *window, int width, int height)
{
	GlfwGfxDevice &gfxDevice = static_cast<GlfwGfxDevice &>(theApplication().gfxDevice());

	// Save previous resolution for if a content scale event is coming just after a window size one
	preScalingWidth_ = gfxDevice.width_;
	preScalingHeight_ = gfxDevice.height_;
	lastFrameWindowSizeChanged_ = theApplication().numFrames();

	gfxDevice.width_ = width;
	gfxDevice.height_ = height;
	gfxDevice.isFullScreen_ = (glfwGetWindowMonitor(window) != nullptr);
}

void GlfwInputManager::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	GlfwGfxDevice &gfxDevice = static_cast<GlfwGfxDevice &>(theApplication().gfxDevice());
	gfxDevice.drawableWidth_ = width;
	gfxDevice.drawableHeight_ = height;

	theApplication().resizeScreenViewport(width, height);
}

void GlfwInputManager::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (inputEventHandler_ == nullptr)
		return;

	keyboardEvent_.scancode = scancode;
	keyboardEvent_.sym = GlfwKeys::keySymValueToEnum(key);
	keyboardEvent_.mod = GlfwKeys::keyModMaskToEnumMask(mods);

	if (action == GLFW_PRESS)
		inputEventHandler_->onKeyPressed(keyboardEvent_);
	else if (action == GLFW_RELEASE)
		inputEventHandler_->onKeyReleased(keyboardEvent_);
}

void GlfwInputManager::charCallback(GLFWwindow *window, unsigned int c)
{
	if (inputEventHandler_ == nullptr)
		return;

	// Current GLFW version does not return an UTF-8 string (https://github.com/glfw/glfw/issues/837)
	nctl::Utf8::codePointToUtf8(c, textInputEvent_.text, nullptr);
	inputEventHandler_->onTextInput(textInputEvent_);
}

void GlfwInputManager::cursorPosCallback(GLFWwindow *window, double x, double y)
{
	if (inputEventHandler_ == nullptr)
		return;

	mouseState_.x = static_cast<int>(x);
	mouseState_.y = theApplication().heightInt() - static_cast<int>(y);
	inputEventHandler_->onMouseMoved(mouseState_);
}

void GlfwInputManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (inputEventHandler_ == nullptr)
		return;

	double xCursor, yCursor;
	glfwGetCursorPos(window, &xCursor, &yCursor);
	mouseEvent_.x = static_cast<int>(xCursor);
	mouseEvent_.y = theApplication().heightInt() - static_cast<int>(yCursor);
	mouseEvent_.button = glfwToNcineMouseButton(button);

	if (action == GLFW_PRESS)
		inputEventHandler_->onMouseButtonPressed(mouseEvent_);
	else if (action == GLFW_RELEASE)
		inputEventHandler_->onMouseButtonReleased(mouseEvent_);
}

void GlfwInputManager::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	if (inputEventHandler_ == nullptr)
		return;

	scrollEvent_.x = static_cast<float>(xoffset);
	scrollEvent_.y = static_cast<float>(yoffset);
	inputEventHandler_->onScrollInput(scrollEvent_);
}

void GlfwInputManager::joystickCallback(int joy, int event)
{
	const int joyId = joy - GLFW_JOYSTICK_1;
	joyConnectionEvent_.joyId = joyId;

	if (event == GLFW_CONNECTED)
	{
		int numButtons = -1;
		int numAxes = -1;
		int numHats = -1;
		glfwGetJoystickButtons(joy, &numButtons);
#ifdef __EMSCRIPTEN__
		numHats = 0;
		const char *guid = "default";
#elif GLFW_VERSION_COMBINED >= 3300
		glfwGetJoystickHats(joy, &numHats);
		const char *guid = glfwGetJoystickGUID(joy);
#else
		numHats = 0;
		const char *guid = nullptr;
#endif
		glfwGetJoystickAxes(joy, &numAxes);
		updateJoystickStates();

		LOGI_X("Joystick %d \"%s\" (GUID: \"%s\") has been connected - %d axes, %d buttons, %d hats",
		       joyId, glfwGetJoystickName(joy), guid, numAxes, numButtons, numHats);
		if (inputEventHandler_ != nullptr)
		{
			joyMapping_.onJoyConnected(joyConnectionEvent_);
			inputEventHandler_->onJoyConnected(joyConnectionEvent_);
		}
	}
	else if (event == GLFW_DISCONNECTED)
	{
		joystickStates_[joyId].resetPrevButtonState();
		joyEventsSimulator_.resetJoystickState(joyId);
		LOGI_X("Joystick %d has been disconnected", joyId);
		if (inputEventHandler_ != nullptr)
		{
			inputEventHandler_->onJoyDisconnected(joyConnectionEvent_);
			joyMapping_.onJoyDisconnected(joyConnectionEvent_);
		}
	}
}

void GlfwInputManager::dropCallback(GLFWwindow *window, int count, const char **paths)
{
	if (inputEventHandler_ == nullptr)
		return;

	int destIndex = 0;
	for (int srcIndex = 0; srcIndex < count; srcIndex++)
	{
		// Skip long paths instead of truncating them
		if (nctl::strnlen(paths[srcIndex], DropEvent::MaxPathLength) > DropEvent::MaxPathLength - 1)
			continue;

		nctl::strncpy(dropEvent_.paths[destIndex], paths[srcIndex], DropEvent::MaxPathLength);
		destIndex++;

		if (destIndex >= DropEvent::MaxNumPaths)
			break;
	}
	dropEvent_.numPaths = destIndex;

	if (dropEvent_.numPaths > 0)
		inputEventHandler_->onFilesDropped(dropEvent_);
}

GlfwInputManager::JoystickEventsSimulator::JoystickEventsSimulator()
{
	memset(buttonsState_, 0, sizeof(unsigned char) * MaxNumButtons * MaxNumJoysticks);
	memset(axesValuesState_, 0, sizeof(float) * MaxNumAxes * MaxNumJoysticks);
}

void GlfwInputManager::JoystickEventsSimulator::resetJoystickState(int joyId)
{
	memset(buttonsState_[joyId], 0, sizeof(unsigned char) * MaxNumButtons);
	memset(axesValuesState_[joyId], 0, sizeof(float) * MaxNumAxes);
}

void GlfwInputManager::JoystickEventsSimulator::simulateButtonsEvents(int joyId, int numButtons, const unsigned char *buttons)
{
	for (int buttonId = 0; buttonId < numButtons; buttonId++)
	{
		if (inputEventHandler_ != nullptr && buttonsState_[joyId][buttonId] != buttons[buttonId])
		{
			joyButtonEvent_.joyId = joyId;
			joyButtonEvent_.buttonId = buttonId;
			if (joystickStates_[joyId].buttons_[buttonId] == GLFW_PRESS)
			{
				joyMapping_.onJoyButtonPressed(joyButtonEvent_);
				inputEventHandler_->onJoyButtonPressed(joyButtonEvent_);
			}
			else if (joystickStates_[joyId].buttons_[buttonId] == GLFW_RELEASE)
			{
				joyMapping_.onJoyButtonReleased(joyButtonEvent_);
				inputEventHandler_->onJoyButtonReleased(joyButtonEvent_);
			}
		}
	}

	if (numButtons > 0)
		memcpy(buttonsState_[joyId], buttons, sizeof(unsigned char) * numButtons);
}

void GlfwInputManager::JoystickEventsSimulator::simulateHatsEvents(int joyId, int numHats, const unsigned char *hats)
{
	for (int hatId = 0; hatId < numHats; hatId++)
	{
		if (inputEventHandler_ != nullptr && hatsState_[joyId][hatId] != hats[hatId])
		{
			joyHatEvent_.joyId = joyId;
			joyHatEvent_.hatId = hatId;
			joyHatEvent_.hatState = hats[hatId];

			joyMapping_.onJoyHatMoved(joyHatEvent_);
			inputEventHandler_->onJoyHatMoved(joyHatEvent_);
		}
	}

	if (numHats > 0)
		memcpy(hatsState_[joyId], hats, sizeof(unsigned char) * numHats);
}

void GlfwInputManager::JoystickEventsSimulator::simulateAxesEvents(int joyId, int numAxes, const float *axesValues)
{
	for (int axisId = 0; axisId < numAxes; axisId++)
	{
		if (inputEventHandler_ != nullptr && fabsf(axesValuesState_[joyId][axisId] - axesValues[axisId]) > AxisEventTolerance)
		{
			joyAxisEvent_.joyId = joyId;
			joyAxisEvent_.axisId = axisId;
			joyAxisEvent_.value = static_cast<short int>(axesValues[axisId] * MaxAxisValue);
			joyAxisEvent_.normValue = axesValues[axisId];
			joyMapping_.onJoyAxisMoved(joyAxisEvent_);
			inputEventHandler_->onJoyAxisMoved(joyAxisEvent_);
		}
	}

	if (numAxes > 0)
		memcpy(axesValuesState_[joyId], axesValues, sizeof(float) * numAxes);
}

}
