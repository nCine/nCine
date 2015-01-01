#ifndef CLASS_NCINE_NDROIDINPUTMANAGER
#define CLASS_NCINE_ANDROIDINPUTMANAGER

#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/sensor.h>
#include "IInputManager.h"
#include "AndroidJniHelper.h"

namespace ncine {

class Timer;

/// Information about Android joystick state
class AndroidJoystickState
{
  public:
	AndroidJoystickState();

  private:
	static const unsigned int MaxNameLength = 256;
	static const int MaxButtons = 10;
	static const int MaxAxes = 10;
	static const int NumAxesToMap = 10;
	static const int AxesToMap[NumAxesToMap];

	int deviceId_;
	char name_[MaxNameLength];

	int numButtons_;
	int numAxes_;
	bool hasDPad_;
	short int buttonsMapping_[AKEYCODE_ESCAPE - AKEYCODE_BUTTON_A];
	short int axesMapping_[MaxAxes];
	bool buttons_[MaxButtons];
	float axisValues_[MaxAxes];

	friend class AndroidInputManager;
};

/// The class for parsing and dispatching Android input events
class AndroidInputManager : public IInputManager
{
  public:
	AndroidInputManager(struct android_app* state);

	// Enables the accelerometer sensor
	static void enableAccelerometerSensor();
	// Disables the accelerometer sensor
	static void disableAccelerometerSensor();

	// Allows the application to make use of the accelerometer
	static void enableAccelerometer(bool enabled);

	// Parses and Android sensor event related to the accelerometer
	static void parseAccelerometerEvent();
	// Parses an Android input event
	static bool parseEvent(const AInputEvent* event);

	bool isJoyPresent(int joyId) const;
	const char* joyName(int joyId) const;
	int joyNumButtons(int joyId) const;
	int joyNumAxes(int joyId) const;
	bool isJoyButtonPressed(int joyId, int buttonId) const;
	short int joyAxisValue(int joyId, int axisId) const;
	float joyAxisNormValue(int joyId, int axisId) const;

  private:
	static const unsigned int MaxNumJoysticks = 4;

	static ASensorManager* sensorManager_;
	static const ASensor* accelerometerSensor_;
	static ASensorEventQueue* sensorEventQueue_;
	static bool accelerometerEnabled_;

	static AccelerometerEvent accelerometerEvent_;
	static TouchEvent touchEvent_;
	static KeyboardEvent keyboardEvent_;

	static AndroidJoystickState joystickStates_[MaxNumJoysticks];
	static JoyButtonEvent joyButtonEvent_;
	static JoyAxisEvent joyAxisEvent_;
	/// Update rate of CheckJoystickDisconnections() in seconds
	static const float JoyCheckRate;
	static Timer joyCheckTimer_;

	// Initializes the accelerometer sensor
	static void initAccelerometerSensor(struct android_app* state);

	// Updates joystick states after connections and disconnections
	static void updateJoystickConnections();
	// Checks if a previously connected joystick has been disconnected
	static void checkDisconnectedJoysticks();
	// Checks if a new joystick has been connected
	static void checkConnectedJoysticks();

	static int findJoyId(int deviceId);
	static bool isDeviceConnected(int deviceId);
	static void deviceInfo(int deviceId, int joyId);

	friend void ::android_main(struct android_app* state);
};

}

#endif
