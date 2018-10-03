#include <cstring> // for memset() and memcpy()
#include <cmath> // for fabsf()
#include "GlfwInputManager.h"
#include "IInputEventHandler.h"
#include "Application.h"
#include "JoyMapping.h"

#ifdef WITH_IMGUI
	#include "ImGuiGlfwInput.h"
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const int IInputManager::MaxNumJoysticks = GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1 + 1;

bool GlfwInputManager::windowHasFocus_ = true;
GlfwMouseState GlfwInputManager::mouseState_;
GlfwMouseEvent GlfwInputManager::mouseEvent_;
GlfwScrollEvent GlfwInputManager::scrollEvent_;
GlfwKeyboardState GlfwInputManager::keyboardState_;
KeyboardEvent GlfwInputManager::keyboardEvent_;

GlfwJoystickState GlfwInputManager::nullJoystickState_;
nctl::StaticArray<GlfwJoystickState, GlfwInputManager::MaxNumJoysticks> GlfwInputManager::joystickStates_;
JoyButtonEvent GlfwInputManager::joyButtonEvent_;
JoyAxisEvent GlfwInputManager::joyAxisEvent_;
JoyConnectionEvent GlfwInputManager::joyConnectionEvent_;
const float GlfwInputManager::JoystickEventsSimulator::AxisEventTolerance = 0.001f;
GlfwInputManager::JoystickEventsSimulator GlfwInputManager::joyEventsSimulator_;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GlfwInputManager::GlfwInputManager()
{
	glfwSetWindowCloseCallback(GlfwGfxDevice::windowHandle(), windowCloseCallback);
	glfwSetWindowSizeCallback(GlfwGfxDevice::windowHandle(), windowSizeCallback);
	glfwSetFramebufferSizeCallback(GlfwGfxDevice::windowHandle(), framebufferSizeCallback);
	glfwSetKeyCallback(GlfwGfxDevice::windowHandle(), keyCallback);
	glfwSetCharCallback(GlfwGfxDevice::windowHandle(), charCallback);
	glfwSetCursorPosCallback(GlfwGfxDevice::windowHandle(), cursorPosCallback);
	glfwSetMouseButtonCallback(GlfwGfxDevice::windowHandle(), mouseButtonCallback);
	glfwSetScrollCallback(GlfwGfxDevice::windowHandle(), scrollCallback);
	glfwSetJoystickCallback(joystickCallback);

	joyMapping_.init(this);

#ifdef WITH_IMGUI
	ImGuiGlfwInput::init(GlfwGfxDevice::windowHandle());
#endif
}

GlfwInputManager::~GlfwInputManager()
{
#ifdef WITH_IMGUI
	ImGuiGlfwInput::shutdown();
#endif
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool GlfwJoystickState::isButtonPressed(int buttonId) const
{
	bool isPressed = false;
	if (buttonId >= 0 && buttonId < numButtons_)
		isPressed = (buttons_[buttonId] != GLFW_RELEASE);
	return isPressed;
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

bool GlfwInputManager::hasFocus()
{
	const bool glfwFocused = (glfwGetWindowAttrib(GlfwGfxDevice::windowHandle(), GLFW_FOCUSED) != 0);

	// A focus event has occurred (either gain or loss)
	if (windowHasFocus_ != glfwFocused)
		windowHasFocus_ = glfwFocused;

	return windowHasFocus_;
}

void GlfwInputManager::updateJoystickStates()
{
	for (unsigned int joyId = 0; joyId < MaxNumJoysticks; joyId++)
	{
		if (glfwJoystickPresent(GLFW_JOYSTICK_1 + joyId))
		{
			joystickStates_[joyId].buttons_ = glfwGetJoystickButtons(joyId, &joystickStates_[joyId].numButtons_);
			joystickStates_[joyId].axesValues_ = glfwGetJoystickAxes(joyId, &joystickStates_[joyId].numAxes_);

			joyEventsSimulator_.simulateButtonsEvents(joyId, joystickStates_[joyId].numButtons_, joystickStates_[joyId].buttons_);
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
#if GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 3
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

		mouseCursorMode_ = mode;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GlfwInputManager::windowCloseCallback(GLFWwindow *window)
{
	ncine::theApplication().quit();
}

void GlfwInputManager::windowSizeCallback(GLFWwindow *window, int width, int height)
{
	GlfwGfxDevice &gfxDevice = static_cast<GlfwGfxDevice &>(theApplication().gfxDevice());
	gfxDevice.width_ = width;
	gfxDevice.height_ = height;
}

void GlfwInputManager::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	theApplication().gfxDevice().setViewport(width, height);
}

void GlfwInputManager::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
#ifdef WITH_IMGUI
	ImGuiGlfwInput::keyCallback(window, key, scancode, action, mods);
#endif

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

void GlfwInputManager::charCallback(GLFWwindow* window, unsigned int c)
{
#ifdef WITH_IMGUI
	ImGuiGlfwInput::charCallback(window, c);
#endif
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
#ifdef WITH_IMGUI
	ImGuiGlfwInput::mouseButtonCallback(window, button, action, mods);
#endif

	if (inputEventHandler_ == nullptr)
		return;

	double xCursor, yCursor;
	glfwGetCursorPos(window, &xCursor, &yCursor);
	mouseEvent_.x = static_cast<int>(xCursor);
	mouseEvent_.y = theApplication().heightInt() - static_cast<int>(yCursor);
	mouseEvent_.button_ = button;

	if (action == GLFW_PRESS)
		inputEventHandler_->onMouseButtonPressed(mouseEvent_);
	else if (action == GLFW_RELEASE)
		inputEventHandler_->onMouseButtonReleased(mouseEvent_);
}

void GlfwInputManager::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
#ifdef WITH_IMGUI
	ImGuiGlfwInput::scrollCallback(window, xoffset, yoffset);
#endif

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
		glfwGetJoystickButtons(joy, &numButtons);
		glfwGetJoystickAxes(joy, &numAxes);
		updateJoystickStates();

	#if GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 3
		const char *guid = glfwGetJoystickGUID(joy);
	#else
		const char *guid = nullptr;
	#endif

		LOGI_X("Joystick %d \"%s\" (%s) has been connected - %d axes, %d buttons",
		       joyId, glfwGetJoystickName(joy), guid, numAxes, numButtons);
		if (inputEventHandler_ != nullptr)
		{
			joyMapping_.onJoyConnected(joyConnectionEvent_);
			inputEventHandler_->onJoyConnected(joyConnectionEvent_);
		}
	}
	else if (event == GLFW_DISCONNECTED)
	{
		joyEventsSimulator_.resetJoystickState(joyId);
		LOGI_X("Joystick %d has been disconnected", joyId);
		if (inputEventHandler_ != nullptr)
		{
			inputEventHandler_->onJoyDisconnected(joyConnectionEvent_);
			joyMapping_.onJoyDisconnected(joyConnectionEvent_);
		}
	}
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
