#ifndef CLASS_NCGLFWINPUTMANAGER
#define CLASS_NCGLFWINPUTMANAGER

#include "ncIInputManager.h"
#include <GL/glfw.h>

/// Information about GLFW mouse state
class ncGLFWMouseState : public ncMouseState
{
public:
	ncGLFWMouseState() { }

	inline bool isLeftButtonDown() const { return (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS); }
	inline bool isMiddleButtonDown() const { return (glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS); }
	inline bool isRightButtonDown() const { return (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS); }
	inline bool isWheelUpButtonDown() const { return (glfwGetMouseButton(GLFW_MOUSE_BUTTON_4) == GLFW_PRESS); }
	inline bool isWheelDownButtonDown() const { return (glfwGetMouseButton(GLFW_MOUSE_BUTTON_5) == GLFW_PRESS); }

	friend class ncGLFWInputManager;
};

/// Information about a GLFW mouse event
class ncGLFWMouseEvent : public ncMouseEvent
{
private:
	int button;
public:
	ncGLFWMouseEvent() : button(0) { }

	inline bool isLeftButton() const { return button == GLFW_MOUSE_BUTTON_LEFT; }
	inline bool isMiddleButton() const { return button == GLFW_MOUSE_BUTTON_MIDDLE; }
	inline bool isRightButton() const { return button == GLFW_MOUSE_BUTTON_RIGHT; }
	inline bool isWheelUpButton() const { return button == GLFW_MOUSE_BUTTON_4; }
	inline bool isWheelDownButton() const { return button == GLFW_MOUSE_BUTTON_5; }

	friend class ncGLFWInputManager;
};

/// Information about GLFW keyboard state
class ncGLFWKeyboardState : public ncKeyboardState
{
public:
	ncGLFWKeyboardState() { }

	inline bool isKeyDown(ncKeySym key) const { return glfwGetKey(key) == GLFW_PRESS; }

	friend class ncGLFWInputManager;
};

/// The class for parsing and dispatching GLFW input events
class ncGLFWInputManager : public ncIInputManager
{
private:
	static ncGLFWMouseState s_mouseState;
	static ncGLFWMouseEvent s_mouseEvent;
	static ncGLFWKeyboardState s_keyboardState;
	static ncKeyboardEvent	s_keyboardEvent;

	static int GLFWCALL WindowCloseCallback();
	static void GLFWCALL KeyCallback(int key, int action);
	static void GLFWCALL MousePosCallback(int x, int y);
	static void GLFWCALL MouseButtonCallback(int button, int action);
public:
	ncGLFWInputManager();

	inline const ncMouseState& MouseState()
	{
		glfwGetMousePos(&s_mouseState.x, &s_mouseState.y);
		return s_mouseState;
	}

	inline const ncKeyboardState& KeyboardState() const
	{
		return s_keyboardState;
	}
};

#endif
