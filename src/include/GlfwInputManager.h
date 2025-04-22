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
	GlfwMouseState();

	bool isButtonDown(MouseButton button) const override;
	bool isButtonPressed(MouseButton button) const override;
	bool isButtonReleased(MouseButton button) const override;

  private:
	unsigned char prevButtonState_[MouseState::NumButtons];

	void copyButtonStateToPrev();

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
	GlfwKeyboardState();

	bool isKeyDown(KeySym key) const override;
	bool isKeyPressed(KeySym key) const override;
	bool isKeyReleased(KeySym key) const override;

  private:
	static const unsigned int NumKeys = static_cast<unsigned int>(KeySym::COUNT);
	unsigned char prevKeyState_[NumKeys];

	void copyKeyStateToPrev();

	friend class GlfwInputManager;
};

/// Information about GLFW joystick state
class GlfwJoystickState : public JoystickState
{
  public:
	GlfwJoystickState();

	bool isButtonDown(int buttonId) const override;
	bool isButtonPressed(int buttonId) const override;
	bool isButtonReleased(int buttonId) const override;

	unsigned char hatState(int hatId) const override;
	short int axisValue(int axisId) const override;
	float axisNormValue(int axisId) const override;

  private:
	static const unsigned int MaxNumButtons = 32;

	int numButtons_;
	int numHats_;
	int numAxes_;

	const unsigned char *buttons_;
	unsigned char prevButtonState_[MaxNumButtons];
	const unsigned char *hats_;
	const float *axesValues_;

	void copyButtonStateToPrev();
	void resetPrevButtonState();

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
	static void copyButtonStatesToPrev();
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

	inline bool hasJoyVibration(int joyId) const override { return false; }
	inline void joyVibrate(int joyId, float lowFreqIntensity, float highFreqIntensity, unsigned int duration) const override {}

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
		static const unsigned int MaxNumButtons = 32;
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
	static MouseEvent mouseEvent_;
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
