#ifndef CLASS_NCANDROIDINPUTMANAGER
#define CLASS_NCANDROIDINPUTMANAGER

#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/sensor.h>
#include "ncIInputManager.h"
#include "ncAndroidJNIHelper.h"

/// Information about Android joystick state
class ncAndroidJoystickState
{
private:
	static const unsigned int s_uMaxNameLength = 256;
	static const int s_iMaxButtons = 10;
	static const int s_iMaxAxes = 10;
	static const int s_iNumAxesToMap = 8;
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
public:
	ncAndroidJoystickState()
		: m_iDeviceId(-1), m_iNumButtons(0), m_iNumAxes(0), m_bHasDPad(false)
	{
		m_vName[0] = '\0';
		for (int i = 0; i < s_iMaxButtons; i++)
		{
			m_vButtonsMapping[i] = 0;
			m_bButtons[i] = false;
		}
		for (int i = 0; i < s_iMaxAxes; i++)
		{
			m_vAxesMapping[i] = 0;
			m_fAxisValues[i] = 0.0f;
		}
	}

	friend class ncAndroidInputManager;
};

/// The class for parsing and dispatching Android input events
class ncAndroidInputManager : public ncIInputManager
{
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

	static int FindJoyId(int iDeviceId);
	static bool isDeviceConnected(int iDeviceId);
	static void DeviceInfo(int iDeviceId, int iJoyId);

	// Updates joystick state structures
	static void UpdateJoystickStates();

	friend void android_main(struct android_app* state);

public:
	// Initializes the accelerometer sensor
	static void InitAccelerometerSensor(struct android_app* state);
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
};

#endif
