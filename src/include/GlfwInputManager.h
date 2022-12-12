#ifndef CLASS_NCINE_GLFWINPUTMANAGER
#define CLASS_NCINE_GLFWINPUTMANAGER

#include <cstdio>
#include "IInputManager.h"
#include "GlfwGfxDevice.h" // for WindowHandle()
#include <nctl/StaticArray.h>

namespace ncine {

class GlfwInputManager;

/// Utility functions to convert between engine key enumerations and GLFW ones
class GlfwKeys
{
  public:
	static KeySym keySymValueToEnum(int keysym);
	static int keyModMaskToEnumMask(int keymod);
	static int enumToKeySymValue(KeySym keysym);
};

/// Information about GLFW mouse state
class GlfwMouseState : public MouseState
{
  public:
	inline bool isLeftButtonDown() const override
	{
		return (glfwGetMouseButton(GlfwGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
	}
	inline bool isMiddleButtonDown() const override
	{
		return (glfwGetMouseButton(GlfwGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
	}
	inline bool isRightButtonDown() const override
	{
		return (glfwGetMouseButton(GlfwGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
	}
	inline bool isFourthButtonDown() const override
	{
		return (glfwGetMouseButton(GlfwGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_4) == GLFW_PRESS);
	}
	inline bool isFifthButtonDown() const override
	{
		return (glfwGetMouseButton(GlfwGfxDevice::windowHandle(), GLFW_MOUSE_BUTTON_5) == GLFW_PRESS);
	}
};

/// Information about a GLFW mouse event
class GlfwMouseEvent : public MouseEvent
{
  public:
	GlfwMouseEvent()
	    : button_(0) {}

	inline bool isLeftButton() const override { return button_ == GLFW_MOUSE_BUTTON_LEFT; }
	inline bool isMiddleButton() const override { return button_ == GLFW_MOUSE_BUTTON_MIDDLE; }
	inline bool isRightButton() const override { return button_ == GLFW_MOUSE_BUTTON_RIGHT; }
	inline bool isFourthButton() const override { return button_ == GLFW_MOUSE_BUTTON_4; }
	inline bool isFifthButton() const override { return button_ == GLFW_MOUSE_BUTTON_5; }

  private:
	int button_;

	friend class GlfwInputManager;
};

/// Information about a GLFW scroll event
class GlfwScrollEvent : public ScrollEvent
{
  public:
	GlfwScrollEvent() {}

	friend class GlfwInputManager;
};

/// Information about GLFW keyboard state
class GlfwKeyboardState : public KeyboardState
{
  public:
	inline bool isKeyDown(KeySym key) const override
	{
		const int glfwKey = GlfwKeys::enumToKeySymValue(key);
		if (glfwKey == GLFW_KEY_UNKNOWN)
			return false;
		else
			return glfwGetKey(GlfwGfxDevice::windowHandle(), glfwKey) == GLFW_PRESS;
	}

	friend class GlfwInputManager;
};

/// Information about GLFW joystick state
class GlfwJoystickState : public JoystickState
{
  public:
	GlfwJoystickState()
	    : numButtons_(0), numHats_(0), numAxes_(0), buttons_(nullptr), hats_(nullptr), axesValues_(nullptr) {}

	bool isButtonPressed(int buttonId) const override;
	unsigned char hatState(int hatId) const override;
	short int axisValue(int axisId) const override;
	float axisNormValue(int axisId) const override;

  private:
	int numButtons_;
	int numHats_;
	int numAxes_;

	const unsigned char *buttons_;
	const unsigned char *hats_;
	const float *axesValues_;

	friend class GlfwInputManager;
};

/// The class for parsing and dispatching GLFW input events
class GlfwInputManager : public IInputManager
{
  public:
	GlfwInputManager();
	~GlfwInputManager() override;

	/// Detects window focus gain/loss events
	static bool hasFocus();
	/// Updates joystick state structures and simulates events
	static void updateJoystickStates();

	const MouseState &mouseState() const override;
	inline const KeyboardState &keyboardState() const override { return keyboardState_; }

	bool isJoyPresent(int joyId) const override;
	const char *joyName(int joyId) const override;
	const char *joyGuid(int joyId) const override;
	int joyNumButtons(int joyId) const override;
	int joyNumHats(int joyId) const override;
	int joyNumAxes(int joyId) const override;
	const JoystickState &joystickState(int joyId) const override;

	void setMouseCursorMode(MouseCursorMode mode) override;

  private:
	static const int MaxNumJoysticks = GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1 + 1;

	class JoystickEventsSimulator
	{
	  public:
		JoystickEventsSimulator();
		void resetJoystickState(int joyId);
		void simulateButtonsEvents(int joyId, int numButtons, const unsigned char *buttons);
		void simulateHatsEvents(int joyId, int numHats, const unsigned char *hats);
		void simulateAxesEvents(int joyId, int numAxes, const float *axesValues);

	  private:
		static const unsigned int MaxNumButtons = 16;
		static const unsigned int MaxNumHats = 4;
		static const unsigned int MaxNumAxes = 16;
		/// Minimum difference between two axis readings in order to trigger an event
		static const float AxisEventTolerance;

		/// Old state used to simulate joystick buttons events
		unsigned char buttonsState_[MaxNumJoysticks][MaxNumButtons];
		/// Old state used to simulate joystick hats events
		unsigned char hatsState_[MaxNumJoysticks][MaxNumHats];
		/// Old state used to simulate joystick axes events
		float axesValuesState_[MaxNumJoysticks][MaxNumAxes];
	};

	static bool windowHasFocus_;
	static GlfwMouseState mouseState_;
	static GlfwMouseEvent mouseEvent_;
	static GlfwScrollEvent scrollEvent_;
	static GlfwKeyboardState keyboardState_;
	static KeyboardEvent keyboardEvent_;
	static TextInputEvent textInputEvent_;
	static GlfwJoystickState nullJoystickState_;
	static nctl::StaticArray<GlfwJoystickState, MaxNumJoysticks> joystickStates_;
	static JoyButtonEvent joyButtonEvent_;
	static JoyHatEvent joyHatEvent_;
	static JoyAxisEvent joyAxisEvent_;
	static JoyConnectionEvent joyConnectionEvent_;
	static JoystickEventsSimulator joyEventsSimulator_;

	/// The window width before a window content scale event
	static int preScalingWidth_;
	/// The window height before a window content scale event
	static int preScalingHeight_;
	/// The last frame a window size callback was called
	static unsigned long int lastFrameWindowSizeChanged_;

	static void monitorCallback(GLFWmonitor *monitor, int event);
	static void windowCloseCallback(GLFWwindow *window);
	static void windowContentScaleCallback(GLFWwindow *window, float xscale, float yscale);
	static void windowSizeCallback(GLFWwindow *window, int width, int height);
	static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
	static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void charCallback(GLFWwindow *window, unsigned int c);
	static void cursorPosCallback(GLFWwindow *window, double x, double y);
	static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	static void joystickCallback(int joy, int event);
	static void dropCallback(GLFWwindow *window, int count, const char **paths);

	static KeySym keySymValueToEnum(int keysym);
	static KeyMod keyModValueToEnum(int keymod);
	static int enumToKeySymValue(KeySym keysym);

	friend class GlfwGfxDevice; // for `setWindowPosition()`
};

inline const MouseState &GlfwInputManager::mouseState() const
{
	double xCursor, yCursor;

	glfwGetCursorPos(GlfwGfxDevice::windowHandle(), &xCursor, &yCursor);
	mouseState_.x = int(xCursor);
	mouseState_.y = int(yCursor);

	return mouseState_;
}

}

#endif
