#include "ncGLFWInputManager.h"
#include "ncIInputEventHandler.h"
#include "ncServiceLocator.h"
#include "ncFileLogger.h"
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
	// Polling GLFW events in ncApplication::Run() to allow pausing
	glfwDisable(GLFW_AUTO_POLL_EVENTS);

	glfwSetWindowCloseCallback(WindowCloseCallback);
	glfwSetKeyCallback(KeyCallback);
	glfwSetMousePosCallback(MousePosCallback);
	glfwSetMouseButtonCallback(MouseButtonCallback);
}

ncGLFWJoystickState::ncGLFWJoystickState()
{
	for (unsigned int i = 0; i < s_uMaxNumButtons; i++)
		m_ubButtons[i] = 0;
	for (unsigned int i = 0; i < s_uMaxNumAxes; i++)
		m_fAxisValues[i] = 0.0f;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int GLFWCALL ncGLFWInputManager::WindowCloseCallback()
{
	ncApplication::Quit();
	return GL_TRUE;
}

void ncGLFWInputManager::KeyCallback(int key, int action)
{
	if (s_pInputEventHandler == NULL)
		return;

	s_keyboardEvent.sym = ncKeySym(key);

	if (action == GLFW_PRESS)
		s_pInputEventHandler->OnKeyPressed(s_keyboardEvent);
	else if (action == GLFW_RELEASE)
		s_pInputEventHandler->OnKeyReleased(s_keyboardEvent);
}

void ncGLFWInputManager::MousePosCallback(int x, int y)
{
	if (s_pInputEventHandler == NULL)
		return;

	s_mouseState.x = x;
	s_mouseState.y = ncApplication::Height()-y;
	s_pInputEventHandler->OnMouseMoved(s_mouseState);
}

void ncGLFWInputManager::MouseButtonCallback(int button, int action)
{
	if (s_pInputEventHandler == NULL)
		return;

	glfwGetMousePos(&s_mouseEvent.x, &s_mouseEvent.y);
	s_mouseEvent.y = ncApplication::Height()-s_mouseEvent.y;
	s_mouseEvent.m_iButton = button;

	if (action == GLFW_PRESS)
		s_pInputEventHandler->OnMouseButtonPressed(s_mouseEvent);
	else if (action == GLFW_RELEASE)
		s_pInputEventHandler->OnMouseButtonReleased(s_mouseEvent);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Detects window focus gain/loss events
bool ncGLFWInputManager::hasFocus()
{
	// A focus event has occurred (either gain or loss)
	if (m_bWindowHasFocus != glfwGetWindowParam(GLFW_ACTIVE))
		m_bWindowHasFocus = glfwGetWindowParam(GLFW_ACTIVE);

	return m_bWindowHasFocus;
}

/// Updates joystick state structures
void ncGLFWInputManager::UpdateJoystickStates()
{
	// TODO: support joystick events when GLFW 3.0 comes out
	for(unsigned int i = 0; i < s_uMaxNumJoysticks; i++)
	{
		if (glfwGetJoystickParam(GLFW_JOYSTICK_1 + i, GLFW_PRESENT))
		{
			glfwGetJoystickButtons(i, s_joystickStates[i].m_ubButtons, ncGLFWJoystickState::s_uMaxNumButtons);
			glfwGetJoystickPos(i, s_joystickStates[i].m_fAxisValues, ncGLFWJoystickState::s_uMaxNumAxes);
		}

		// Odd axes are inverted to maintain consistency with the SDL implementation
		for (unsigned int j = 1; j<ncGLFWJoystickState::s_uMaxNumAxes; j=j+2)
			s_joystickStates[i].m_fAxisValues[j] *= -1.0f;
	}
}

bool ncGLFWInputManager::isJoyPresent(int iJoyId) const
{
	if (GLFW_JOYSTICK_1 + iJoyId <= GLFW_JOYSTICK_LAST)
		return glfwGetJoystickParam(GLFW_JOYSTICK_1 + iJoyId, GLFW_PRESENT) == GL_TRUE;
	else
		return false;
}

int ncGLFWInputManager::JoyNumButtons(int iJoyId) const
{
	int iNumButtons = -1;

	if (isJoyPresent(iJoyId))
		iNumButtons = glfwGetJoystickParam(GLFW_JOYSTICK_1 + iJoyId, GLFW_BUTTONS);

	return iNumButtons;
}

int ncGLFWInputManager::JoyNumAxes(int iJoyId) const
{
	int iNumAxes = -1;

	if (isJoyPresent(iJoyId))
		iNumAxes = glfwGetJoystickParam(GLFW_JOYSTICK_1 + iJoyId, GLFW_AXES);

	return iNumAxes;
}

bool ncGLFWInputManager::isJoyButtonPressed(int iJoyId, int iButtonId) const
{
	if (isJoyPresent(iJoyId))
		return s_joystickStates[iJoyId].m_ubButtons[iButtonId];
	else
		return false;
}

short int ncGLFWInputManager::JoyAxisValue(int iJoyId, int iAxisId) const
{
	if (isJoyPresent(iJoyId))
		return s_joystickStates[iJoyId].m_fAxisValues[iAxisId] * s_iMaxAxisValue;
	else
		return 0;
}
