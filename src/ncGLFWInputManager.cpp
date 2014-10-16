#include "ncGLFWInputManager.h"
#include "ncIInputEventHandler.h"
#include "ncApplication.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

ncIInputEventHandler* ncIInputManager::inputEventHandler_ = NULL;
bool ncGLFWInputManager::windowHasFocus_ = true;
ncGLFWMouseState ncGLFWInputManager::mouseState_;
ncGLFWMouseEvent ncGLFWInputManager::mouseEvent_;
ncGLFWKeyboardState ncGLFWInputManager::keyboardState_;
ncKeyboardEvent	ncGLFWInputManager::keyboardEvent_;
short int ncIInputManager::MaxAxisValue = 32767;
ncGLFWJoystickState ncGLFWInputManager::joystickStates_[MaxNumJoysticks];

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncGLFWInputManager::ncGLFWInputManager()
{
	glfwSetWindowCloseCallback(ncGLFWGfxDevice::windowHandle(), windowCloseCallback);
	glfwSetKeyCallback(ncGLFWGfxDevice::windowHandle(), keyCallback);
	glfwSetCursorPosCallback(ncGLFWGfxDevice::windowHandle(), cursorPosCallback);
	glfwSetMouseButtonCallback(ncGLFWGfxDevice::windowHandle(), mouseButtonCallback);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncGLFWInputManager::windowCloseCallback(GLFWwindow *window)
{
	ncApplication::quit();
}

void ncGLFWInputManager::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (inputEventHandler_ == NULL)
	{
		return;
	}

	keyboardEvent_.scancode = scancode;
	keyboardEvent_.sym = ncKeySym(key);
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

void ncGLFWInputManager::cursorPosCallback(GLFWwindow *window, double x, double y)
{
	if (inputEventHandler_ == NULL)
	{
		return;
	}

	mouseState_.x = int(x);
	mouseState_.y = ncApplication::height() - int(y);
	inputEventHandler_->onMouseMoved(mouseState_);
}

void ncGLFWInputManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (inputEventHandler_ == NULL)
	{
		return;
	}

	double xCursor, yCursor;
	glfwGetCursorPos(window, &xCursor, &yCursor);
	mouseEvent_.x = int(xCursor); mouseEvent_.y = int(yCursor);
	mouseEvent_.y = ncApplication::height() - mouseEvent_.y;
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
bool ncGLFWInputManager::hasFocus()
{
	// A focus event has occurred (either gain or loss)
	if (windowHasFocus_ != glfwGetWindowAttrib(ncGLFWGfxDevice::windowHandle(), GLFW_FOCUSED))
	{
		windowHasFocus_ = glfwGetWindowAttrib(ncGLFWGfxDevice::windowHandle(), GLFW_FOCUSED);
	}

	return windowHasFocus_;
}

/// Updates joystick state structures
void ncGLFWInputManager::updateJoystickStates()
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

bool ncGLFWInputManager::isJoyPresent(int joyId) const
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

const char *ncGLFWInputManager::joyName(int joyId) const
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

int ncGLFWInputManager::joyNumButtons(int joyId) const
{
	int numButtons = -1;

	if (isJoyPresent(joyId))
	{
		glfwGetJoystickButtons(GLFW_JOYSTICK_1 + joyId, &numButtons);
	}

	return numButtons;
}

int ncGLFWInputManager::joyNumAxes(int joyId) const
{
	int numAxes = -1;

	if (isJoyPresent(joyId))
	{
		glfwGetJoystickAxes(GLFW_JOYSTICK_1 + joyId, &numAxes);
	}

	return numAxes;
}

bool ncGLFWInputManager::isJoyButtonPressed(int joyId, int buttonId) const
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

short int ncGLFWInputManager::joyAxisValue(int joyId, int axisId) const
{
	// If the joystick is not present the returned value is zero
	short int axisValue = joyAxisNormValue(joyId, axisId) * MaxAxisValue;

	return axisValue;
}

float ncGLFWInputManager::joyAxisNormValue(int joyId, int axisId) const
{
	float axisValue = 0.0f;

	if (isJoyPresent(joyId) && axisId >= 0 && axisId < joyNumAxes(joyId))
	{
		axisValue = joystickStates_[joyId].axisValues_[axisId];
	}

	return axisValue;
}
