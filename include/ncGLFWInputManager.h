#ifndef CLASS_NCGLFWINPUTMANAGER
#define CLASS_NCGLFWINPUTMANAGER

#include <cstdio>
#include "ncIInputManager.h"
#include "ncGLFWGfxDevice.h" // for WindowHandle()

class ncGLFWInputManager;

/// Information about GLFW mouse state
class ncGLFWMouseState : public ncMouseState
{
  public:
	ncGLFWMouseState() { }

	inline bool isLeftButtonDown() const { return (glfwGetMouseButton(ncGLFWGfxDevice::WindowHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS); }
	inline bool isMiddleButtonDown() const { return (glfwGetMouseButton(ncGLFWGfxDevice::WindowHandle(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS); }
	inline bool isRightButtonDown() const { return (glfwGetMouseButton(ncGLFWGfxDevice::WindowHandle(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS); }
	inline bool isWheelUpButtonDown() const { return (glfwGetMouseButton(ncGLFWGfxDevice::WindowHandle(), GLFW_MOUSE_BUTTON_4) == GLFW_PRESS); }
	inline bool isWheelDownButtonDown() const { return (glfwGetMouseButton(ncGLFWGfxDevice::WindowHandle(), GLFW_MOUSE_BUTTON_5) == GLFW_PRESS); }
};

/// Information about a GLFW mouse event
class ncGLFWMouseEvent : public ncMouseEvent
{
  public:
	ncGLFWMouseEvent() : m_iButton(0) { }

	inline bool isLeftButton() const { return m_iButton == GLFW_MOUSE_BUTTON_LEFT; }
	inline bool isMiddleButton() const { return m_iButton == GLFW_MOUSE_BUTTON_MIDDLE; }
	inline bool isRightButton() const { return m_iButton == GLFW_MOUSE_BUTTON_RIGHT; }
	inline bool isWheelUpButton() const { return m_iButton == GLFW_MOUSE_BUTTON_4; }
	inline bool isWheelDownButton() const { return m_iButton == GLFW_MOUSE_BUTTON_5; }

  private:
	int m_iButton;

	friend class ncGLFWInputManager;
};

/// Information about GLFW keyboard state
class ncGLFWKeyboardState : public ncKeyboardState
{
  public:
	ncGLFWKeyboardState() { }
	inline bool isKeyDown(ncKeySym key) const { return glfwGetKey(ncGLFWGfxDevice::WindowHandle(), key) == GLFW_PRESS; }
};

/// Information about GLFW joystick state
class ncGLFWJoystickState
{
  public:
	ncGLFWJoystickState()
		: m_iNumButtons(0), m_iNumAxes(0), m_ubButtons(NULL), m_fAxisValues(NULL) { }

  private:
	int m_iNumButtons;
	int m_iNumAxes;

	const unsigned char *m_ubButtons;
	const float *m_fAxisValues;

	friend class ncGLFWInputManager;
};

/// The class for parsing and dispatching GLFW input events
class ncGLFWInputManager : public ncIInputManager
{
  public:
	ncGLFWInputManager();

	// Detects window focus gain/loss events
	static bool hasFocus();
	// Updates joystick state structures
	static void UpdateJoystickStates();

	const ncMouseState& MouseState();
	inline const ncKeyboardState& KeyboardState() const	{ return s_keyboardState; }

	bool isJoyPresent(int iJoyId) const;
	const char* JoyName(int iJoyId) const;
	int JoyNumButtons(int iJoyId) const;
	int JoyNumAxes(int iJoyId) const;
	bool isJoyButtonPressed(int iJoyId, int iButtonId) const;
	short int JoyAxisValue(int iJoyId, int iAxisId) const;
	float JoyAxisNormValue(int iJoyId, int iAxisId) const;

  private:
	static const unsigned int s_uMaxNumJoysticks = GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1 + 1;

	static void WindowCloseCallback(GLFWwindow *window);
	static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void CursorPosCallback(GLFWwindow *window, double x, double y);
	static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

	static bool m_bWindowHasFocus;
	static ncGLFWMouseState s_mouseState;
	static ncGLFWMouseEvent s_mouseEvent;
	static ncGLFWKeyboardState s_keyboardState;
	static ncKeyboardEvent	s_keyboardEvent;
	static ncGLFWJoystickState s_joystickStates[s_uMaxNumJoysticks];
};

inline const ncMouseState& ncGLFWInputManager::MouseState()
{
	double dCursorX, dCursorY;

	glfwGetCursorPos(ncGLFWGfxDevice::WindowHandle(), &dCursorX, &dCursorY);
	s_mouseState.x = int(dCursorX); s_mouseState.y = int(dCursorY);

	return s_mouseState;
}

#endif
