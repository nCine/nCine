#include "GlfwInputManager.h"
#include "IInputEventHandler.h"
#include "Application.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

IInputEventHandler* IInputManager::inputEventHandler_ = NULL;
bool GlfwInputManager::windowHasFocus_ = true;
GlfwMouseState GlfwInputManager::mouseState_;
GlfwMouseEvent GlfwInputManager::mouseEvent_;
GlfwKeyboardState GlfwInputManager::keyboardState_;
KeyboardEvent	GlfwInputManager::keyboardEvent_;
short int IInputManager::MaxAxisValue = 32767;
StaticArray<GlfwJoystickState, GlfwInputManager::MaxNumJoysticks> GlfwInputManager::joystickStates_;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GlfwInputManager::GlfwInputManager()
{
	glfwSetWindowCloseCallback(GlfwGfxDevice::windowHandle(), windowCloseCallback);
	glfwSetKeyCallback(GlfwGfxDevice::windowHandle(), keyCallback);
	glfwSetCursorPosCallback(GlfwGfxDevice::windowHandle(), cursorPosCallback);
	glfwSetMouseButtonCallback(GlfwGfxDevice::windowHandle(), mouseButtonCallback);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GlfwInputManager::windowCloseCallback(GLFWwindow *window)
{
	ncine::theApplication().quit();
}

void GlfwInputManager::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (inputEventHandler_ == NULL)
	{
		return;
	}

	keyboardEvent_.scancode = scancode;
	keyboardEvent_.sym = KeySym(key);
	keyboardEvent_.mod = mods;

	if (action == GLFW_PRESS)
	{
		inputEventHandler_->onKeyPressed(keyboardEvent_);
	}
	else if (action == GLFW_RELEASE)
	{
		inputEventHandler_->onKeyReleased(keyboardEvent_);
	}
}

void GlfwInputManager::cursorPosCallback(GLFWwindow *window, double x, double y)
{
	if (inputEventHandler_ == NULL)
	{
		return;
	}

	mouseState_.x = int(x);
	mouseState_.y = theApplication().height() - int(y);
	inputEventHandler_->onMouseMoved(mouseState_);
}

void GlfwInputManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (inputEventHandler_ == NULL)
	{
		return;
	}

	double xCursor, yCursor;
	glfwGetCursorPos(window, &xCursor, &yCursor);
	mouseEvent_.x = int(xCursor); mouseEvent_.y = int(yCursor);
	mouseEvent_.y = theApplication().height() - mouseEvent_.y;
	mouseEvent_.button_ = button;

	if (action == GLFW_PRESS)
	{
		inputEventHandler_->onMouseButtonPressed(mouseEvent_);
	}
	else if (action == GLFW_RELEASE)
	{
		inputEventHandler_->onMouseButtonReleased(mouseEvent_);
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Detects window focus gain/loss events
bool GlfwInputManager::hasFocus()
{
	// A focus event has occurred (either gain or loss)
	if (windowHasFocus_ != glfwGetWindowAttrib(GlfwGfxDevice::windowHandle(), GLFW_FOCUSED))
	{
		windowHasFocus_ = glfwGetWindowAttrib(GlfwGfxDevice::windowHandle(), GLFW_FOCUSED);
	}

	return windowHasFocus_;
}

/// Updates joystick state structures
void GlfwInputManager::updateJoystickStates()
{
	for (unsigned int i = 0; i < MaxNumJoysticks; i++)
	{
		if (glfwJoystickPresent(GLFW_JOYSTICK_1 + i))
		{
			joystickStates_[i].buttons_ = glfwGetJoystickButtons(i, &joystickStates_[i].numButtons_);
			joystickStates_[i].axisValues_ = glfwGetJoystickAxes(i, &joystickStates_[i].numAxes_);
		}
	}
}

bool GlfwInputManager::isJoyPresent(int joyId) const
{
	if (joyId >= 0 && GLFW_JOYSTICK_1 + joyId <= GLFW_JOYSTICK_LAST)
	{
		return glfwJoystickPresent(GLFW_JOYSTICK_1 + joyId);
	}
	else
	{
		return false;
	}
}

const char *GlfwInputManager::joyName(int joyId) const
{
	if (isJoyPresent(joyId))
	{
		return glfwGetJoystickName(joyId);
	}
	else
	{
		return '\0';
	}
}

int GlfwInputManager::joyNumButtons(int joyId) const
{
	int numButtons = -1;

	if (isJoyPresent(joyId))
	{
		glfwGetJoystickButtons(GLFW_JOYSTICK_1 + joyId, &numButtons);
	}

	return numButtons;
}

int GlfwInputManager::joyNumAxes(int joyId) const
{
	int numAxes = -1;

	if (isJoyPresent(joyId))
	{
		glfwGetJoystickAxes(GLFW_JOYSTICK_1 + joyId, &numAxes);
	}

	return numAxes;
}

bool GlfwInputManager::isJoyButtonPressed(int joyId, int buttonId) const
{
	if (isJoyPresent(joyId) && buttonId >= 0 && buttonId < joyNumButtons(joyId))
	{
		return joystickStates_[joyId].buttons_[buttonId];
	}
	else
	{
		return false;
	}
}

short int GlfwInputManager::joyAxisValue(int joyId, int axisId) const
{
	// If the joystick is not present the returned value is zero
	short int axisValue = static_cast<short int>(joyAxisNormValue(joyId, axisId) * MaxAxisValue);

	return axisValue;
}

float GlfwInputManager::joyAxisNormValue(int joyId, int axisId) const
{
	float axisValue = 0.0f;

	if (isJoyPresent(joyId) && axisId >= 0 && axisId < joyNumAxes(joyId))
	{
		axisValue = joystickStates_[joyId].axisValues_[axisId];
	}

	return axisValue;
}

}
