#ifndef CLASS_NCINE_ANDROIDINPUTMANAGER
#define CLASS_NCINE_ANDROIDINPUTMANAGER

#include <android/keycodes.h>
#include "IInputManager.h"
#include "AndroidJniHelper.h"

struct AInputEvent;
struct ASensorManager;
struct ASensor;
struct ASensorEventQueue;

namespace ncine {

class AndroidApplication;
class Timer;

/// Utility functions to convert between engine key enumerations and Android ones
class AndroidKeys
{
  public:
	static KeySym keySymValueToEnum(int keysym);
	static int keyModMaskToEnumMask(int keymod);
};

/// Information about Android mouse state
class AndroidMouseState : public MouseState
{
  public:
	AndroidMouseState();

	bool isButtonDown(MouseButton button) const override;
	bool isButtonPressed(MouseButton button) const override;
	bool isButtonReleased(MouseButton button) const override;

  private:
	unsigned int currentStateIndex_;
	int buttonStates_[2];

	void copyButtonStateToPrev();

	friend class AndroidInputManager;
};

/// Simulated information about Android keyboard state
class AndroidKeyboardState : public KeyboardState
{
  public:
	AndroidKeyboardState();

	bool isKeyDown(KeySym key) const override;
	bool isKeyPressed(KeySym key) const override;
	bool isKeyReleased(KeySym key) const override;

  private:
	static const unsigned int NumKeys = static_cast<unsigned int>(KeySym::COUNT);
	unsigned int currentStateIndex_;
	unsigned char keys_[2][NumKeys];

	void copyKeyStateToPrev();

	friend class AndroidInputManager;
};

/// Information about Android joystick state
class AndroidJoystickState : JoystickState
{
  public:
	/// Supporting no more than a left and a right vibrator
	static const int MaxVibrators = 2;

	AndroidJoystickState();

	bool isButtonDown(int buttonId) const override;
	bool isButtonPressed(int buttonId) const override;
	bool isButtonReleased(int buttonId) const override;

	unsigned char hatState(int hatId) const override;
	short int axisValue(int axisId) const override;
	float axisNormValue(int axisId) const override;

  private:
	/// SDL_HARDWARE_BUS_BLUETOOTH value
	static const uint16_t BluetoothBus = 0x05;

	static const unsigned int MaxNameLength = 256;
	/// All AKEYCODE_BUTTON_* plus AKEYCODE_BACK
	static const int MaxButtons = AKEYCODE_ESCAPE - AKEYCODE_BUTTON_A + 1;
	static const int MaxAxes = 10;
	static const int NumAxesToMap = 12;
	static const int AxesToMap[NumAxesToMap];

	int deviceId_;
	static const unsigned int GuidLength = 16;
	char guid_[GuidLength];
	char name_[MaxNameLength];

	int numButtons_;
	int numHats_;
	int numAxes_;
	int numMappedAxes_;
	bool hasDPad_;
	bool hasHatAxes_;
	short int buttonsMapping_[MaxButtons];
	short int axesMapping_[MaxAxes];
	unsigned int currentStateIndex_;
	unsigned char buttonStates_[2][MaxButtons];
	/// Minimum value for every available axis (used for -1..1 range remapping)
	float axesMinValues_[MaxAxes];
	/// Range value for every available axis (used for -1..1 range remapping)
	float axesRangeValues_[MaxAxes];
	/// Normalized value in the -1..1 range
	float axesValues_[MaxAxes];
	unsigned char hatState_; // no more than one hat is supported
	int numVibrators_;
	int vibratorsIds_[MaxVibrators];
	AndroidJniClass_Vibrator vibrators_[MaxVibrators];

	void createGuid(uint16_t bus, uint16_t vendor, uint16_t product, uint16_t version, const char *name, uint8_t driverSignature, uint8_t driverData);
	void updateGuidWithCapabilities();

	void copyButtonStateToPrev();
	void resetPrevButtonState();

	friend class AndroidInputManager;
};

/// The class for parsing and dispatching Android input events
class AndroidInputManager : public IInputManager
{
  public:
	explicit AndroidInputManager(struct android_app *state);
	~AndroidInputManager() override;

	/// Enables the accelerometer sensor
	static void enableAccelerometerSensor();
	/// Disables the accelerometer sensor
	static void disableAccelerometerSensor();

	/// Allows the application to make use of the accelerometer
	static void enableAccelerometer(bool enabled);

	inline const MouseState &mouseState() const override { return mouseState_; }
	inline const KeyboardState &keyboardState() const override { return keyboardState_; }

	/// Parses an Android sensor event related to the accelerometer
	static void parseAccelerometerEvent();
	/// Parses an Android input event
	static bool parseEvent(const AInputEvent *event);

	bool isJoyPresent(int joyId) const override;
	const char *joyName(int joyId) const override;
	const char *joyGuid(int joyId) const override;
	int joyNumButtons(int joyId) const override;
	int joyNumHats(int joyId) const override;
	int joyNumAxes(int joyId) const override;
	const JoystickState &joystickState(int joyId) const override;

	bool hasJoyVibration(int joyId) const override;
	void joyVibrate(int joyId, float lowFreqIntensity, float highFreqIntensity, unsigned int duration) const override;

	void setMouseCursorMode(MouseCursorMode mode) override { mouseCursorMode_ = mode; }

  private:
	static const int MaxNumJoysticks = 4;

	static ASensorManager *sensorManager_;
	static const ASensor *accelerometerSensor_;
	static ASensorEventQueue *sensorEventQueue_;
	static bool accelerometerEnabled_;

	static AccelerometerEvent accelerometerEvent_;
	static TouchEvent touchEvent_;
	static AndroidKeyboardState keyboardState_;
	static KeyboardEvent keyboardEvent_;
	static TextInputEvent textInputEvent_;
	static AndroidMouseState mouseState_;
	static MouseEvent mouseEvent_;
	static ScrollEvent scrollEvent_;
	/// Back and forward key events triggered by the mouse are simulated as right and middle button
	static int simulatedMouseButtonState_;

	static AndroidJoystickState nullJoystickState_;
	static AndroidJoystickState joystickStates_[MaxNumJoysticks];
	static JoyButtonEvent joyButtonEvent_;
	static JoyHatEvent joyHatEvent_;
	static JoyAxisEvent joyAxisEvent_;
	static JoyConnectionEvent joyConnectionEvent_;
	/// Update rate of `updateJoystickConnections()` in seconds
	static const float JoyCheckRate;
	static Timer joyCheckTimer_;

	/// Processes a gamepad event
	static bool processGamepadEvent(const AInputEvent *event);
	/// Processes a keyboard event
	static bool processKeyboardEvent(const AInputEvent *event);
	/// Processes a touch event
	static bool processTouchEvent(const AInputEvent *event);
	/// Processes a mouse event
	static bool processMouseEvent(const AInputEvent *event);
	/// Processes a keycode event generated by the mouse, like the back key on right mouse click
	static bool processMouseKeyEvent(const AInputEvent *event);

	/// Initializes the accelerometer sensor
	static void initAccelerometerSensor(struct android_app *state);

	/// Updates joystick states after connections and disconnections
	static void updateJoystickConnections();
	/// Checks if a previously connected joystick has been disconnected
	static void checkDisconnectedJoysticks();
	/// Checks if a new joystick has been connected
	static void checkConnectedJoysticks();

	static int findJoyId(int deviceId);
	static bool isDeviceConnected(int deviceId);
	static void deviceInfo(int deviceId, int joyId);

	static void copyButtonStatesToPrev();

	/// To update joystick connections in `AndroidApplication::androidMain()`
	friend class AndroidApplication;
};

}

#endif
