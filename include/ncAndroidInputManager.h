#ifndef CLASS_NCANDROIDINPUTMANAGER
#define CLASS_NCANDROIDINPUTMANAGER

#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/sensor.h>
#include "ncIInputManager.h"

/// The class for parsing and dispatching Android input events
class ncAndroidInputManager : public ncIInputManager
{
private:
	static ASensorManager* s_pSensorManager;
	static const ASensor* s_pAccelerometerSensor;
	static ASensorEventQueue* s_pSensorEventQueue;
	static bool s_bAccelerometerEnabled;

	static ncAccelerometerEvent s_accelerometerEvent;
	static ncTouchEvent s_touchEvent;
	static ncKeyboardEvent s_keyboardEvent;

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
};

#endif
