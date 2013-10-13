#ifndef CLASS_NCGLFWINPUTMANAGER
#define CLASS_NCGLFWINPUTMANAGER

#include "ncIInputManager.h"
#if defined(__APPLE__)
	#include <GLFW/glfw.h>
#else
	#include <GL/glfw.h>
#endif

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
};

/// Information about a GLFW mouse event
class ncGLFWMouseEvent : public ncMouseEvent
{
private:
	int m_iButton;
public:
	ncGLFWMouseEvent() : m_iButton(0) { }

	inline bool isLeftButton() const { return m_iButton == GLFW_MOUSE_BUTTON_LEFT; }
	inline bool isMiddleButton() const { return m_iButton == GLFW_MOUSE_BUTTON_MIDDLE; }
	inline bool isRightButton() const { return m_iButton == GLFW_MOUSE_BUTTON_RIGHT; }
	inline bool isWheelUpButton() const { return m_iButton == GLFW_MOUSE_BUTTON_4; }
	inline bool isWheelDownButton() const { return m_iButton == GLFW_MOUSE_BUTTON_5; }

	friend class ncGLFWInputManager;
};

/// Information about GLFW keyboard state
class ncGLFWKeyboardState : public ncKeyboardState
{
public:
	ncGLFWKeyboardState() { }
	inline bool isKeyDown(ncKeySym key) const { return glfwGetKey(key) == GLFW_PRESS; }
};

/// Information about GLFW joystick state
class ncGLFWJoystickState
{
private:
	static const unsigned int s_uMaxNumButtons = 16;
	static const unsigned int s_uMaxNumAxes = 16;

	unsigned char m_ubButtons[s_uMaxNumButtons];
	float m_fAxisValues[s_uMaxNumAxes];
public:
	ncGLFWJoystickState();
	friend class ncGLFWInputManager;
};

/// The class for parsing and dispatching GLFW input events
class ncGLFWInputManager : public ncIInputManager
{
private:
	static const unsigned int s_uMaxNumJoysticks = GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1 + 1;

	static bool m_bWindowHasFocus;
	static ncGLFWMouseState s_mouseState;
	static ncGLFWMouseEvent s_mouseEvent;
	static ncGLFWKeyboardState s_keyboardState;
	static ncKeyboardEvent	s_keyboardEvent;
	static ncGLFWJoystickState s_joystickStates[s_uMaxNumJoysticks];

	static int GLFWCALL WindowCloseCallback();
	static void GLFWCALL KeyCallback(int key, int action);
	static void GLFWCALL MousePosCallback(int x, int y);
	static void GLFWCALL MouseButtonCallback(int button, int action);
public:
	ncGLFWInputManager();

	// Detects window focus gain/loss events
	static bool hasFocus();
	// Updates joystick state structures
	static void UpdateJoystickStates();

	inline const ncMouseState& MouseState()
	{
		glfwGetMousePos(&s_mouseState.x, &s_mouseState.y);
		return s_mouseState;
	}

	inline const ncKeyboardState& KeyboardState() const
	{
		return s_keyboardState;
	}

	bool isJoyPresent(int iJoyId) const;
	int JoyNumButtons(int iJoyId) const;
	int JoyNumAxes(int iJoyId) const;
	bool isJoyButtonPressed(int iJoyId, int iButtonId) const;
	short int JoyAxisValue(int iJoyId, int iAxisId) const;
};

#endif
