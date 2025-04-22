#ifndef CLASS_NCINE_QT5INPUTMANAGER
#define CLASS_NCINE_QT5INPUTMANAGER

#include <qevent.h>
#include "IInputManager.h"
#ifdef WITH_QT5GAMEPAD
	#include <nctl/UniquePtr.h>

class QGamepadManager;
class QGamepad;
#endif

class QKeyEvent;
class QMouseEvent;
class QTouchEvent;
class QWheelEvent;

namespace ncine {

class Qt5Widget;

/// Utility functions to convert between engine key enumerations and Qt5 ones
class Qt5Keys
{
  public:
	static KeySym keySymValueToEnum(int keysym);
	static int keyModMaskToEnumMask(Qt::KeyboardModifiers keymod);
};

/// Information about Qt5 mouse state
class Qt5MouseState : public MouseState
{
  public:
	Qt5MouseState();

	bool isButtonDown(MouseButton button) const override;
	bool isButtonPressed(MouseButton button) const override;
	bool isButtonReleased(MouseButton button) const override;

  private:
	unsigned int currentStateIndex_;
	Qt::MouseButtons buttonStates_[2];

	void copyButtonStateToPrev();

	friend class Qt5InputManager;
};

/// Information about a Qt5 scroll event
class Qt5ScrollEvent : public ScrollEvent
{
  public:
	Qt5ScrollEvent() {}

	friend class Qt5InputManager;
};

//// Simulated information about Qt5 keyboard state
class Qt5KeyboardState : public KeyboardState
{
  public:
	Qt5KeyboardState();

	bool isKeyDown(KeySym key) const override;
	bool isKeyPressed(KeySym key) const override;
	bool isKeyReleased(KeySym key) const override;

  private:
	static const unsigned int NumKeys = static_cast<unsigned int>(KeySym::COUNT);
	unsigned int currentStateIndex_;
	unsigned char keys_[2][NumKeys];

	void copyKeyStateToPrev();

	friend class Qt5InputManager;
};

/// Information about Qt5 joystick state
#ifdef WITH_QT5GAMEPAD
class Qt5JoystickState : public JoystickState
{
  public:
	Qt5JoystickState();

	bool isButtonDown(int buttonId) const override;
	bool isButtonPressed(int buttonId) const override;
	bool isButtonReleased(int buttonId) const override;

	unsigned char hatState(int hatId) const override;
	short int axisValue(int axisId) const override;
	float axisNormValue(int axisId) const override;

  private:
	static const unsigned int MaxNameLength = 256;
	static const int NumButtons = 12;
	static const unsigned int NumAxes = 6;
	/// Minimum difference between two axis readings in order to trigger an event
	static const float AxisEventTolerance;

	unsigned int currentStateIndex_;
	unsigned char buttonState_[2][NumButtons];
	unsigned char hatState_;
	float axesValuesState_[NumAxes];

	char name_[MaxNameLength];
	nctl::UniquePtr<QGamepad> gamepad_;

	void copyButtonStateToPrev();
	void resetPrevButtonState();

	friend class Qt5InputManager;
};
#else
class Qt5JoystickState : public JoystickState
{
  public:
	Qt5JoystickState() {}

	inline bool isButtonDown(int buttonId) const override { return false; }
	inline bool isButtonPressed(int buttonId) const override { return false; }
	inline bool isButtonReleased(int buttonId) const override { return false; }

	inline unsigned char hatState(int hatId) const override { return HatState::CENTERED; }
	inline short int axisValue(int axisId) const override { return 0U; }
	inline float axisNormValue(int axisId) const override { return 0.0f; }

  private:
	friend class Qt5InputManager;
};
#endif

/// The class for parsing and dispatching Qt5 input events
class Qt5InputManager : public IInputManager
{
  public:
	/// The constructor takes care of opening available joysticks
	Qt5InputManager(Qt5Widget &widget);
	/// The destructor releases every opened joystick
	~Qt5InputManager();

#ifdef WITH_QT5GAMEPAD
	void updateJoystickStates();
#endif

	void copyButtonStatesToPrev();
	bool shouldQuitOnRequest();
	bool event(QEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void touchBeginEvent(QTouchEvent *event);
	void touchUpdateEvent(QTouchEvent *event);
	void touchEndEvent(QTouchEvent *event);
	void wheelEvent(QWheelEvent *event);
	void dropEvent(QDropEvent *event);

	inline const MouseState &mouseState() const override { return mouseState_; }
	inline const KeyboardState &keyboardState() const override { return keyboardState_; }

#ifdef WITH_QT5GAMEPAD
	bool isJoyPresent(int joyId) const override;
	const char *joyName(int joyId) const override;
	const char *joyGuid(int joyId) const override { return nullptr; }
	inline int joyNumButtons(int joyId) const override { return Qt5JoystickState::NumButtons; }
	inline int joyNumHats(int joyId) const override { return 1; }
	inline int joyNumAxes(int joyId) const override { return Qt5JoystickState::NumAxes; }
	const JoystickState &joystickState(int joyId) const override;
#else
	inline bool isJoyPresent(int joyId) const override { return false; }
	inline const char *joyName(int joyId) const override { return nullptr; }
	inline const char *joyGuid(int joyId) const override { return nullptr; }
	inline int joyNumButtons(int joyId) const override { return 0; }
	inline int joyNumHats(int joyId) const override { return 0; }
	inline int joyNumAxes(int joyId) const override { return 0; }
	inline const JoystickState &joystickState(int joyId) const override { return nullJoystickState_; }
#endif
	inline bool hasJoyVibration(int joyId) const override { return false; }
	inline void joyVibrate(int joyId, float lowFreqIntensity, float highFreqIntensity, unsigned int duration) const override {}

	void setMouseCursorMode(MouseCursorMode mode) override;

  private:
	static const int MaxNumJoysticks = 4;

	static TouchEvent touchEvent_;
	static Qt5MouseState mouseState_;
	static MouseEvent mouseEvent_;
	static Qt5ScrollEvent scrollEvent_;
	static Qt5KeyboardState keyboardState_;
	static KeyboardEvent keyboardEvent_;
	static TextInputEvent textInputEvent_;
	static Qt5JoystickState nullJoystickState_;
#ifdef WITH_QT5GAMEPAD
	static Qt5JoystickState joystickStates_[MaxNumJoysticks];
	static JoyButtonEvent joyButtonEvent_;
	static JoyHatEvent joyHatEvent_;
	static JoyAxisEvent joyAxisEvent_;
	static JoyConnectionEvent joyConnectionEvent_;
#endif

	Qt5Widget &widget_;

	void updateTouchEvent(const QTouchEvent *event);

	/// Deleted copy constructor
	Qt5InputManager(const Qt5InputManager &) = delete;
	/// Deleted assignment operator
	Qt5InputManager &operator=(const Qt5InputManager &) = delete;
};

}

#endif
