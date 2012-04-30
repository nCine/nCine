#include "ncGLFWInputManager.h"
#include "ncIInputEventHandler.h"
#include "ncServiceLocator.h"
#include "ncFileLogger.h"
#include "ncApplication.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

ncIInputEventHandler* ncIInputManager::s_pInputEventHandler = NULL;
ncGLFWMouseState ncGLFWInputManager::s_mouseState;
ncGLFWMouseEvent ncGLFWInputManager::s_mouseEvent;
ncGLFWKeyboardState ncGLFWInputManager::s_keyboardState;
ncKeyboardEvent	ncGLFWInputManager::s_keyboardEvent;

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
	s_mouseEvent.button = button;

	if (action == GLFW_PRESS)
		s_pInputEventHandler->OnMouseButtonPressed(s_mouseEvent);
	else if (action == GLFW_RELEASE)
		s_pInputEventHandler->OnMouseButtonReleased(s_mouseEvent);
}
