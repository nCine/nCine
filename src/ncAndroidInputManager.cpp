#include "ncAndroidInputManager.h"
#include "ncIInputEventHandler.h"
#include "ncApplication.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

ncIInputEventHandler* ncIInputManager::s_pInputEventHandler = NULL;
ASensorManager* ncAndroidInputManager::s_pSensorManager = NULL;
const ASensor* ncAndroidInputManager::s_pAccelerometerSensor = NULL;
ASensorEventQueue* ncAndroidInputManager::s_pSensorEventQueue = NULL;
bool ncAndroidInputManager::s_bAccelerometerEnabled = false;
ncAccelerometerEvent ncAndroidInputManager::s_accelerometerEvent;
ncTouchEvent ncAndroidInputManager::s_touchEvent;
ncKeyboardEvent ncAndroidInputManager::s_keyboardEvent;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes the accelerometer sensor
void ncAndroidInputManager::InitAccelerometerSensor(android_app *state)
{
	// Prepare to monitor accelerometer
	s_pSensorManager = ASensorManager_getInstance();
	s_pAccelerometerSensor = ASensorManager_getDefaultSensor(s_pSensorManager,
			ASENSOR_TYPE_ACCELEROMETER);
	s_pSensorEventQueue = ASensorManager_createEventQueue(s_pSensorManager,
			state->looper, LOOPER_ID_USER, NULL, NULL);
}

/// Enables the accelerometer sensor
/*! It is called by EnableAccelerometer() and when the application gains focus */
void ncAndroidInputManager::EnableAccelerometerSensor()
{
	if (s_bAccelerometerEnabled && s_pAccelerometerSensor != NULL)
	{
		ASensorEventQueue_enableSensor(s_pSensorEventQueue, s_pAccelerometerSensor);
		// 60 events per second
		ASensorEventQueue_setEventRate(s_pSensorEventQueue, s_pAccelerometerSensor, (1000L/60)*1000);
	}
}

/// Disables the accelerometer sensor
/*! It is called by EnableAccelerometer() and when the application loses focus */
void ncAndroidInputManager::DisableAccelerometerSensor()
{
	if (s_bAccelerometerEnabled && s_pAccelerometerSensor != NULL)
		ASensorEventQueue_disableSensor(s_pSensorEventQueue, s_pAccelerometerSensor);
}

/// Allows the application to make use of the accelerometer
/*! Activates the sensor and raises the flag needed for application focus handling */
void ncAndroidInputManager::EnableAccelerometer(bool bEnabled)
{
	if (bEnabled)
		EnableAccelerometerSensor();
	else
		DisableAccelerometerSensor();

	s_bAccelerometerEnabled = bEnabled;
}

/// Parses and Android sensor event related to the accelerometer
void ncAndroidInputManager::ParseAccelerometerEvent()
{
	if (s_bAccelerometerEnabled && s_pAccelerometerSensor != NULL)
	{
		ASensorEvent event;
		while (ASensorEventQueue_getEvents(s_pSensorEventQueue, &event, 1) > 0)
		{
			s_accelerometerEvent.x = event.acceleration.x;
			s_accelerometerEvent.y = event.acceleration.y;
			s_accelerometerEvent.z = event.acceleration.z;
			s_pInputEventHandler->OnAcceleration(s_accelerometerEvent);
		}
	}
}

/// Parses an Android input event
void ncAndroidInputManager::ParseEvent(const AInputEvent* event)
{
	if (s_pInputEventHandler == NULL)
		return;

	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
	{
		s_keyboardEvent.scancode = AKeyEvent_getScanCode(event);
		s_keyboardEvent.sym = ncKeySym(AKeyEvent_getKeyCode(event));
		s_keyboardEvent.mod = AKeyEvent_getMetaState(event);

		switch(AKeyEvent_getAction(event))
		{
			case AKEY_EVENT_ACTION_DOWN:
				s_pInputEventHandler->OnKeyPressed(s_keyboardEvent);
				break;
			case AKEY_EVENT_ACTION_UP:
				s_pInputEventHandler->OnKeyReleased(s_keyboardEvent);
				break;
			case AKEY_EVENT_ACTION_MULTIPLE:
				break;
		}
	}
	else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		s_touchEvent.count = AMotionEvent_getPointerCount(event);
		s_touchEvent.id = AMotionEvent_getPointerId(event, 0);
		s_touchEvent.x = AMotionEvent_getX(event, 0);
		s_touchEvent.y = ncApplication::Height() - AMotionEvent_getY(event, 0);
		s_touchEvent.id2 = AMotionEvent_getPointerId(event, 1);
		s_touchEvent.x2 = AMotionEvent_getX(event, 1);
		s_touchEvent.y2 = ncApplication::Height() - AMotionEvent_getY(event, 1);

		switch(AMotionEvent_getAction(event))
		{
			case AMOTION_EVENT_ACTION_DOWN:
				s_pInputEventHandler->OnTouchDown(s_touchEvent);
				break;
			case AMOTION_EVENT_ACTION_UP:
				s_pInputEventHandler->OnTouchUp(s_touchEvent);
				break;
			case AMOTION_EVENT_ACTION_MOVE:
				s_pInputEventHandler->OnTouchMove(s_touchEvent);
				break;
			case AMOTION_EVENT_ACTION_POINTER_DOWN:
				s_pInputEventHandler->OnSecondaryTouchDown(s_touchEvent);
				break;
			case AMOTION_EVENT_ACTION_POINTER_UP:
				s_pInputEventHandler->OnSecondaryTouchUp(s_touchEvent);
				break;
		}
	}
}
