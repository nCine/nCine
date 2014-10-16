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

	inline bool isLeftButtonDown() const { return (glfwGetMouseButton(ncGLFWGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS); }
	inline bool isMiddleButtonDown() const { return (glfwGetMouseButton(ncGLFWGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS); }
	inline bool isRightButtonDown() const { return (glfwGetMouseButton(ncGLFWGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS); }
	inline bool isWheelUpButtonDown() const { return (glfwGetMouseButton(ncGLFWGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_4) == GLFW_PRESS); }
	inline bool isWheelDownButtonDown() const { return (glfwGetMouseButton(ncGLFWGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_5) == GLFW_PRESS); }
};

/// Information about a GLFW mouse event
class ncGLFWMouseEvent : public ncMouseEvent
{
  public:
	ncGLFWMouseEvent() : button_(0) { }

	inline bool isLeftButton() const { return button_ == GLFW_MOUSE_BUTTON_LEFT; }
	inline bool isMiddleButton() const { return button_ == GLFW_MOUSE_BUTTON_MIDDLE; }
	inline bool isRightButton() const { return button_ == GLFW_MOUSE_BUTTON_RIGHT; }
	inline bool isWheelUpButton() const { return button_ == GLFW_MOUSE_BUTTON_4; }
	inline bool isWheelDownButton() const { return button_ == GLFW_MOUSE_BUTTON_5; }

  private:
	int button_;

	friend class ncGLFWInputManager;
};

/// Information about GLFW keyboard state
class ncGLFWKeyboardState : public ncKeyboardState
{
  public:
	ncGLFWKeyboardState() { }
	inline bool isKeyDown(ncKeySym key) const { return glfwGetKey(ncGLFWGfxDevice::windowHandle(), key) == GLFW_PRESS; }
};

/// Information about GLFW joystick state
class ncGLFWJoystickState
{
  public:
	ncGLFWJoystickState()
		: numButtons_(0), numAxes_(0), buttons_(NULL), axisValues_(NULL) { }

  private:
	int numButtons_;
	int numAxes_;

	const unsigned char *buttons_;
	const float *axisValues_;

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
	static void updateJoystickStates();

	const ncMouseState& mouseState();
	inline const ncKeyboardState& keyboardState() const	{ return keyboardState_; }

	bool isJoyPresent(int joyId) const;
	const char* joyName(int joyId) const;
	int joyNumButtons(int joyId) const;
	int joyNumAxes(int joyId) const;
	bool isJoyButtonPressed(int joyId, int buttonId) const;
	short int joyAxisValue(int joyId, int axisId) const;
	float joyAxisNormValue(int joyId, int axisId) const;

  private:
	static const unsigned int MaxNumJoysticks = GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1 + 1;

	static void windowCloseCallback(GLFWwindow *window);
	static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void cursorPosCallback(GLFWwindow *window, double x, double y);
	static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

	static bool windowHasFocus_;
	static ncGLFWMouseState mouseState_;
	static ncGLFWMouseEvent mouseEvent_;
	static ncGLFWKeyboardState keyboardState_;
	static ncKeyboardEvent	keyboardEvent_;
	static ncGLFWJoystickState joystickStates_[MaxNumJoysticks];
};

inline const ncMouseState& ncGLFWInputManager::mouseState()
{
	double xCursor, yCursor;

	glfwGetCursorPos(ncGLFWGfxDevice::windowHandle(), &xCursor, &yCursor);
	mouseState_.x = int(xCursor); mouseState_.y = int(yCursor);

	return mouseState_;
}

#endif
