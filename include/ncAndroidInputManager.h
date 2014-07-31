#ifndef CLASS_NCANDROIDINPUTMANAGER
#define CLASS_NCANDROIDINPUTMANAGER

#include <jni.h>
#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/sensor.h>
#include <android/api-level.h>
#include "ncIInputManager.h"

/// Information about Android joystick state
class ncAndroidJoystickState
{
private:
	static const unsigned int s_uNumButtons = 10;
	static const unsigned int s_uNumAxes = 8;

	int m_iDeviceId;

	bool m_bButtons[s_uNumButtons];
	float m_fAxisValues[s_uNumAxes];
public:
	ncAndroidJoystickState()
		: m_iDeviceId(-1)
	{
		for (unsigned int i = 0; i < s_uNumButtons; i++)
			m_bButtons[i] = false;
		for (unsigned int i = 0; i < s_uNumAxes; i++)
			m_fAxisValues[i] = 0.0f;
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

#if __ANDROID_API__ >= 13
	static int FindJoyId(int iDeviceId);
	static JavaVM *s_pJVM;
	static JNIEnv *s_pEnv;
	static jclass s_inputDeviceClass;
	static jmethodID s_getDeviceMethod;
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

#if __ANDROID_API__ >= 13
	bool isJoyPresent(int iJoyId) const;
	int JoyNumButtons(int iJoyId) const;
	int JoyNumAxes(int iJoyId) const;
	bool isJoyButtonPressed(int iJoyId, int iButtonId) const;
	short int JoyAxisValue(int iJoyId, int iAxisId) const;
	float JoyAxisNormValue(int iJoyId, int iAxisId) const;
	static void AttachJVM(JavaVM *pJVM);
	static void DetachJVM();
	static bool isDeviceConnected(int iDeviceId);
#endif
};

#endif
