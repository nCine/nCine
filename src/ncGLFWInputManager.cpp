#include "ncGLFWInputManager.h"
#include "ncIInputEventHandler.h"
#include "ncApplication.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

ncIInputEventHandler* ncIInputManager::s_pInputEventHandler = NULL;
bool ncGLFWInputManager::m_bWindowHasFocus = true;
ncGLFWMouseState ncGLFWInputManager::s_mouseState;
ncGLFWMouseEvent ncGLFWInputManager::s_mouseEvent;
ncGLFWKeyboardState ncGLFWInputManager::s_keyboardState;
ncKeyboardEvent	ncGLFWInputManager::s_keyboardEvent;
short int ncIInputManager::s_iMaxAxisValue = 32767;
ncGLFWJoystickState ncGLFWInputManager::s_joystickStates[s_uMaxNumJoysticks];

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncGLFWInputManager::ncGLFWInputManager()
{
	glfwSetWindowCloseCallback(ncGLFWGfxDevice::WindowHandle(), WindowCloseCallback);
	glfwSetKeyCallback(ncGLFWGfxDevice::WindowHandle(), KeyCallback);
	glfwSetCursorPosCallback(ncGLFWGfxDevice::WindowHandle(), CursorPosCallback);
	glfwSetMouseButtonCallback(ncGLFWGfxDevice::WindowHandle(), MouseButtonCallback);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncGLFWInputManager::WindowCloseCallback(GLFWwindow *window)
{
	ncApplication::Quit();
}

void ncGLFWInputManager::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (s_pInputEventHandler == NULL)
	{
		return;
	}

	s_keyboardEvent.scancode = scancode;
	s_keyboardEvent.sym = ncKeySym(key);
	s_keyboardEvent.mod = mods;

	if (action == GLFW_PRESS)
	{
		s_pInputEventHandler->OnKeyPressed(s_keyboardEvent);
	}
	else if (action == GLFW_RELEASE)
	{
		s_pInputEventHandler->OnKeyReleased(s_keyboardEvent);
	}
}

void ncGLFWInputManager::CursorPosCallback(GLFWwindow *window, double x, double y)
{
	if (s_pInputEventHandler == NULL)
	{
		return;
	}

	s_mouseState.x = int(x);
	s_mouseState.y = ncApplication::Height() - int(y);
	s_pInputEventHandler->OnMouseMoved(s_mouseState);
}

void ncGLFWInputManager::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (s_pInputEventHandler == NULL)
	{
		return;
	}

	double dCursorX, dCursory;
	glfwGetCursorPos(window, &dCursorX, &dCursory);
	s_mouseEvent.x = int(dCursorX); s_mouseEvent.y = int(dCursory);
	s_mouseEvent.y = ncApplication::Height() - s_mouseEvent.y;
	s_mouseEvent.m_iButton = button;

	if (action == GLFW_PRESS)
	{
		s_pInputEventHandler->OnMouseButtonPressed(s_mouseEvent);
	}
	else if (action == GLFW_RELEASE)
	{
		s_pInputEventHandler->OnMouseButtonReleased(s_mouseEvent);
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Detects window focus gain/loss events
bool ncGLFWInputManager::hasFocus()
{
	// A focus event has occurred (either gain or loss)
	if (m_bWindowHasFocus != glfwGetWindowAttrib(ncGLFWGfxDevice::WindowHandle(), GLFW_FOCUSED))
	{
		m_bWindowHasFocus = glfwGetWindowAttrib(ncGLFWGfxDevice::WindowHandle(), GLFW_FOCUSED);
	}

	return m_bWindowHasFocus;
}

/// Updates joystick state structures
void ncGLFWInputManager::UpdateJoystickStates()
{
	for (unsigned int i = 0; i < s_uMaxNumJoysticks; i++)
	{
		if (glfwJoystickPresent(GLFW_JOYSTICK_1 + i))
		{
			s_joystickStates[i].m_ubButtons = glfwGetJoystickButtons(i, &s_joystickStates[i].m_iNumButtons);
			s_joystickStates[i].m_fAxisValues = glfwGetJoystickAxes(i, &s_joystickStates[i].m_iNumAxes);
		}
	}
}

bool ncGLFWInputManager::isJoyPresent(int iJoyId) const
{
	if (iJoyId >= 0 && GLFW_JOYSTICK_1 + iJoyId <= GLFW_JOYSTICK_LAST)
	{
		return glfwJoystickPresent(GLFW_JOYSTICK_1 + iJoyId);
	}
	else
	{
		return false;
	}
}

const char *ncGLFWInputManager::JoyName(int iJoyId) const
{
	if (isJoyPresent(iJoyId))
	{
		return glfwGetJoystickName(iJoyId);
	}
	else
	{
		return '\0';
	}
}

int ncGLFWInputManager::JoyNumButtons(int iJoyId) const
{
	int iNumButtons = -1;

	if (isJoyPresent(iJoyId))
	{
		glfwGetJoystickButtons(GLFW_JOYSTICK_1 + iJoyId, &iNumButtons);
	}

	return iNumButtons;
}

int ncGLFWInputManager::JoyNumAxes(int iJoyId) const
{
	int iNumAxes = -1;

	if (isJoyPresent(iJoyId))
	{
		glfwGetJoystickAxes(GLFW_JOYSTICK_1 + iJoyId, &iNumAxes);
	}

	return iNumAxes;
}

bool ncGLFWInputManager::isJoyButtonPressed(int iJoyId, int iButtonId) const
{
	if (isJoyPresent(iJoyId) && iButtonId >= 0 && iButtonId < JoyNumButtons(iJoyId))
	{
		return s_joystickStates[iJoyId].m_ubButtons[iButtonId];
	}
	else
	{
		return false;
	}
}

short int ncGLFWInputManager::JoyAxisValue(int iJoyId, int iAxisId) const
{
	// If the joystick is not present the returned value is zero
	short int iAxisValue = JoyAxisNormValue(iJoyId, iAxisId) * s_iMaxAxisValue;

	return iAxisValue;
}

float ncGLFWInputManager::JoyAxisNormValue(int iJoyId, int iAxisId) const
{
	float fAxisValue = 0.0f;

	if (isJoyPresent(iJoyId) && iAxisId >= 0 && iAxisId < JoyNumAxes(iJoyId))
	{
		fAxisValue = s_joystickStates[iJoyId].m_fAxisValues[iAxisId];
	}

	return fAxisValue;
}
