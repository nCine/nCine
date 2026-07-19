#ifndef CLASS_NCINE_QTINPUTMANAGER
#define CLASS_NCINE_QTINPUTMANAGER

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

class QtWidget;

/// Utility functions to convert between engine key enumerations and Qt ones
class QtKeys
{
  public:
	static KeySym keySymValueToEnum(int keysym);
	static int keyModMaskToEnumMask(Qt::KeyboardModifiers keymod);
};

/// Information about Qt mouse state
class QtMouseState : public MouseState
{
  public:
	QtMouseState();

	bool isButtonDown(MouseButton button) const override;
	bool isButtonPressed(MouseButton button) const override;
	bool isButtonReleased(MouseButton button) const override;

  private:
	unsigned int currentStateIndex_;
	Qt::MouseButtons buttonStates_[2];

	void copyButtonStateToPrev();

	friend class QtInputManager;
};

/// Information about a Qt scroll event
class QtScrollEvent : public ScrollEvent
{
  public:
	QtScrollEvent() {}

	friend class QtInputManager;
};

/// Simulated information about Qt keyboard state
class QtKeyboardState : public KeyboardState
{
  public:
	QtKeyboardState();

	bool isKeyDown(KeySym key) const override;
	bool isKeyPressed(KeySym key) const override;
	bool isKeyReleased(KeySym key) const override;

  private:
	static const unsigned int NumKeys = static_cast<unsigned int>(KeySym::COUNT);
	unsigned int currentStateIndex_;
	unsigned char keys_[2][NumKeys];

	void copyKeyStateToPrev();

	friend class QtInputManager;
};

/// Information about Qt joystick state
#ifdef WITH_QT5GAMEPAD
class QtJoystickState : public JoystickState
{
  public:
	QtJoystickState();

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

	friend class QtInputManager;
};
#else
class QtJoystickState : public JoystickState
{
  public:
	QtJoystickState() {}

	inline bool isButtonDown(int buttonId) const override { return false; }
	inline bool isButtonPressed(int buttonId) const override { return false; }
	inline bool isButtonReleased(int buttonId) const override { return false; }

	inline unsigned char hatState(int hatId) const override { return HatState::CENTERED; }
	inline short int axisValue(int axisId) const override { return 0U; }
	inline float axisNormValue(int axisId) const override { return 0.0f; }

  private:
	friend class QtInputManager;
};
#endif

/// The class for parsing and dispatching Qt input events
class QtInputManager : public IInputManager
{
  public:
	/// The constructor takes care of opening available joysticks
	QtInputManager(QtWidget &widget);
	/// The destructor releases every opened joystick
	~QtInputManager() override;

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
	inline int joyNumButtons(int joyId) const override { return QtJoystickState::NumButtons; }
	inline int joyNumHats(int joyId) const override { return 1; }
	inline int joyNumAxes(int joyId) const override { return QtJoystickState::NumAxes; }
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
	static QtMouseState mouseState_;
	static MouseEvent mouseEvent_;
	static QtScrollEvent scrollEvent_;
	static QtKeyboardState keyboardState_;
	static KeyboardEvent keyboardEvent_;
	static TextInputEvent textInputEvent_;
	static QtJoystickState nullJoystickState_;
#ifdef WITH_QT5GAMEPAD
	static QtJoystickState joystickStates_[MaxNumJoysticks];
	static JoyButtonEvent joyButtonEvent_;
	static JoyHatEvent joyHatEvent_;
	static JoyAxisEvent joyAxisEvent_;
	static JoyConnectionEvent joyConnectionEvent_;
#endif

	QtWidget &widget_;

	void updateTouchEvent(const QTouchEvent *event);

	/// Deleted copy constructor
	QtInputManager(const QtInputManager &) = delete;
	/// Deleted assignment operator
	QtInputManager &operator=(const QtInputManager &) = delete;
};

}

#endif
