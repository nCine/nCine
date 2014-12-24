#ifndef CLASS_NCINE_GLFWINPUTMANAGER
#define CLASS_NCINE_GLFWINPUTMANAGER

#include <cstdio>
#include "IInputManager.h"
#include "GlfwGfxDevice.h" // for WindowHandle()

namespace ncine {

class GlfwInputManager;

/// Information about GLFW mouse state
class GlfwMouseState : public MouseState
{
  public:
	inline bool isLeftButtonDown() const { return (glfwGetMouseButton(GlfwGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS); }
	inline bool isMiddleButtonDown() const { return (glfwGetMouseButton(GlfwGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS); }
	inline bool isRightButtonDown() const { return (glfwGetMouseButton(GlfwGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS); }
	inline bool isWheelUpButtonDown() const { return (glfwGetMouseButton(GlfwGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_4) == GLFW_PRESS); }
	inline bool isWheelDownButtonDown() const { return (glfwGetMouseButton(GlfwGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_5) == GLFW_PRESS); }
};

/// Information about a GLFW mouse event
class GlfwMouseEvent : public MouseEvent
{
  public:
	GlfwMouseEvent() : button_(0) { }

	inline bool isLeftButton() const { return button_ == GLFW_MOUSE_BUTTON_LEFT; }
	inline bool isMiddleButton() const { return button_ == GLFW_MOUSE_BUTTON_MIDDLE; }
	inline bool isRightButton() const { return button_ == GLFW_MOUSE_BUTTON_RIGHT; }
	inline bool isWheelUpButton() const { return button_ == GLFW_MOUSE_BUTTON_4; }
	inline bool isWheelDownButton() const { return button_ == GLFW_MOUSE_BUTTON_5; }

  private:
	int button_;

	friend class GlfwInputManager;
};

/// Information about GLFW keyboard state
class GlfwKeyboardState : public KeyboardState
{
  public:
	inline bool isKeyDown(KeySym key) const { return glfwGetKey(GlfwGfxDevice::windowHandle(), key) == GLFW_PRESS; }
};

/// Information about GLFW joystick state
class GlfwJoystickState
{
  public:
	GlfwJoystickState()
		: numButtons_(0), numAxes_(0), buttons_(NULL), axisValues_(NULL) { }

  private:
	int numButtons_;
	int numAxes_;

	const unsigned char *buttons_;
	const float *axisValues_;

	friend class GlfwInputManager;
};

/// The class for parsing and dispatching GLFW input events
class GlfwInputManager : public IInputManager
{
  public:
	GlfwInputManager();

	// Detects window focus gain/loss events
	static bool hasFocus();
	// Updates joystick state structures
	static void updateJoystickStates();

	const MouseState& mouseState();
	inline const KeyboardState& keyboardState() const { return keyboardState_; }

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
	static GlfwMouseState mouseState_;
	static GlfwMouseEvent mouseEvent_;
	static GlfwKeyboardState keyboardState_;
	static KeyboardEvent	keyboardEvent_;
	static GlfwJoystickState joystickStates_[MaxNumJoysticks];
};

inline const MouseState& GlfwInputManager::mouseState()
{
	double xCursor, yCursor;

	glfwGetCursorPos(GlfwGfxDevice::windowHandle(), &xCursor, &yCursor);
	mouseState_.x = int(xCursor); mouseState_.y = int(yCursor);

	return mouseState_;
}

}

#endif
