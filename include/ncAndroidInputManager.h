#ifndef CLASS_NCANDROIDINPUTMANAGER
#define CLASS_NCANDROIDINPUTMANAGER

#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/sensor.h>
#include "ncIInputManager.h"
#include "ncAndroidJNIHelper.h"

class ncTimer;

/// Information about Android joystick state
class ncAndroidJoystickState
{
  public:
	ncAndroidJoystickState();

  private:
	static const unsigned int s_uMaxNameLength = 256;
	static const int s_iMaxButtons = 10;
	static const int s_iMaxAxes = 10;
	static const int s_iNumAxesToMap = 10;
	static const int s_vAxesToMap[s_iNumAxesToMap];

	int m_iDeviceId;
	char m_vName[s_uMaxNameLength];

	int m_iNumButtons;
	int m_iNumAxes;
	bool m_bHasDPad;
	short int m_vButtonsMapping[AKEYCODE_ESCAPE - AKEYCODE_BUTTON_A];
	short int m_vAxesMapping[s_iMaxAxes];
	bool m_bButtons[s_iMaxButtons];
	float m_fAxisValues[s_iMaxAxes];

	friend class ncAndroidInputManager;
};

/// The class for parsing and dispatching Android input events
class ncAndroidInputManager : public ncIInputManager
{
  public:
	ncAndroidInputManager(struct android_app* state);

	// Enables the accelerometer sensor
	static void EnableAccelerometerSensor();
	// Disables the accelerometer sensor
	static void DisableAccelerometerSensor();

	// Allows the application to make use of the accelerometer
	static void EnableAccelerometer(bool bEnabled);

	// Parses and Android sensor event related to the accelerometer
	static void ParseAccelerometerEvent();
	// Parses an Android input event
	static void ParseEvent(const AInputEvent* event);

	bool isJoyPresent(int iJoyId) const;
	const char* JoyName(int iJoyId) const;
	int JoyNumButtons(int iJoyId) const;
	int JoyNumAxes(int iJoyId) const;
	bool isJoyButtonPressed(int iJoyId, int iButtonId) const;
	short int JoyAxisValue(int iJoyId, int iAxisId) const;
	float JoyAxisNormValue(int iJoyId, int iAxisId) const;

  private:
	static const unsigned int s_uMaxNumJoysticks = 4;

	static ASensorManager* s_pSensorManager;
	static const ASensor* s_pAccelerometerSensor;
	static ASensorEventQueue* s_pSensorEventQueue;
	static bool s_bAccelerometerEnabled;

	static ncAccelerometerEvent s_accelerometerEvent;
	static ncTouchEvent s_touchEvent;
	static ncKeyboardEvent s_keyboardEvent;

	static ncAndroidJoystickState s_joystickStates[s_uMaxNumJoysticks];
	static ncJoyButtonEvent s_joyButtonEvent;
	static ncJoyAxisEvent s_joyAxisEvent;
	/// Update rate of CheckJoystickDisconnections() in seconds
	static const float s_joyCheckRate = 0.25f;
	static ncTimer s_joyCheckTimer;

	// Initializes the accelerometer sensor
	static void InitAccelerometerSensor(struct android_app* state);

	// Updates joystick states after connections and disconnections
	static void UpdateJoystickConnections();
	// Checks if a previously connected joystick has been disconnected
	static void CheckDisconnectedJoysticks();
	// Checks if a new joystick has been connected
	static void CheckConnectedJoysticks();

	static int FindJoyId(int iDeviceId);
	static bool isDeviceConnected(int iDeviceId);
	static void DeviceInfo(int iDeviceId, int iJoyId);

	friend void android_main(struct android_app* state);
};

#endif
