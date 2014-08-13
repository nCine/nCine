#ifndef CLASS_NCANDROIDINPUTMANAGER
#define CLASS_NCANDROIDINPUTMANAGER

#include <jni.h>
#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/sensor.h>
#include <android/api-level.h>
#include "ncIInputManager.h"

#if (__ANDROID_API__ >= 13)
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
#endif

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

#if (__ANDROID_API__ >= 13)
	static int FindJoyId(int iDeviceId);
	static JavaVM *s_pJVM;
	static JNIEnv *s_pEnv;
	static jclass s_clsInputDevice;
	static jmethodID s_midGetDevice;
	static jmethodID s_midGetName;
	static jmethodID s_midGetMotionRange;
	static jclass s_clsKeyCharacterMap;
	static jmethodID s_midDeviceHasKey;

	static bool isDeviceConnected(int iDeviceId);
	static void DeviceInfo(int iDeviceId, int iJoyId);
#endif
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

#if (__ANDROID_API__ >= 13)
	bool isJoyPresent(int iJoyId) const;
	const char* JoyName(int iJoyId) const;
	int JoyNumButtons(int iJoyId) const;
	int JoyNumAxes(int iJoyId) const;
	bool isJoyButtonPressed(int iJoyId, int iButtonId) const;
	short int JoyAxisValue(int iJoyId, int iAxisId) const;
	float JoyAxisNormValue(int iJoyId, int iAxisId) const;
	static void AttachJVM(struct android_app* state);
	static void DetachJVM();

	// Updates joystick state structures and raises disconnection events
	static void UpdateJoystickStates();
#endif
};

#endif
