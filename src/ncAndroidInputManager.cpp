#include "ncAndroidInputManager.h"
#include "ncIInputEventHandler.h"
#include "ncApplication.h"
#include "ncServiceLocator.h"

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
short int ncIInputManager::s_iMaxAxisValue = 32767;
ncAndroidJoystickState ncAndroidInputManager::s_joystickStates[s_uMaxNumJoysticks];
JavaVM *ncAndroidInputManager::s_pJVM = NULL;
JNIEnv *ncAndroidInputManager::s_pEnv = NULL;
jclass ncAndroidInputManager::s_clsInputDevice;
jmethodID ncAndroidInputManager::s_midGetDevice;
jmethodID ncAndroidInputManager::s_midGetName;

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

#if (__ANDROID_API__ >= 13)
	if ((AInputEvent_getSource(event) & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD ||
		(AInputEvent_getSource(event) & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK ||
		(AInputEvent_getSource(event) & AINPUT_SOURCE_DPAD) == AINPUT_SOURCE_DPAD)
	{
		int iDeviceId = AInputEvent_getDeviceId(event);
		int iJoyId = FindJoyId(iDeviceId);

		// If the index is valid then the structure can be updated
		if (iJoyId > -1)
		{
			if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
			{
				int iBtnIndex = -1;
				s_joystickStates[iJoyId].m_iDeviceId = iDeviceId;

				switch(AKeyEvent_getKeyCode(event))
				{
					case AKEYCODE_BACK:
						iBtnIndex = 0;
						break;
					case AKEYCODE_BUTTON_SELECT:
						iBtnIndex = 1;
						break;
					case AKEYCODE_BUTTON_X:
						iBtnIndex = 2;
						break;
					case AKEYCODE_BUTTON_A:
						iBtnIndex = 3;
						break;
					case AKEYCODE_BUTTON_Y:
						iBtnIndex = 4;
						break;
					case AKEYCODE_BUTTON_B:
						iBtnIndex = 5;
						break;
					case AKEYCODE_BUTTON_R1:
						iBtnIndex = 6;
						break;
					case AKEYCODE_BUTTON_L1:
						iBtnIndex = 7;
						break;
					case AKEYCODE_BUTTON_THUMBR:
						iBtnIndex = 8;
						break;
					case AKEYCODE_BUTTON_THUMBL:
						iBtnIndex = 9;
						break;
				}

				switch(AKeyEvent_getAction(event))
				{
					case AKEY_EVENT_ACTION_DOWN:
						s_joystickStates[iJoyId].m_bButtons[iBtnIndex] = true;
						break;
					case AKEY_EVENT_ACTION_UP:
						s_joystickStates[iJoyId].m_bButtons[iBtnIndex] = false;
						break;
					case AKEY_EVENT_ACTION_MULTIPLE:
						break;
				}
			}
			else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
			{
				s_joystickStates[iJoyId].m_iDeviceId = iDeviceId;
				s_joystickStates[iJoyId].m_fAxisValues[0] = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_X, 0);
				s_joystickStates[iJoyId].m_fAxisValues[1] = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Y, 0);
				s_joystickStates[iJoyId].m_fAxisValues[2] = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_LTRIGGER, 0);
				s_joystickStates[iJoyId].m_fAxisValues[3] = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Z, 0);
				s_joystickStates[iJoyId].m_fAxisValues[4] = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_RZ, 0);
				s_joystickStates[iJoyId].m_fAxisValues[5] = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_RTRIGGER, 0);
				s_joystickStates[iJoyId].m_fAxisValues[6] = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_RX, 0);
				s_joystickStates[iJoyId].m_fAxisValues[7] = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_RY, 0);
			}
		}
		else
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncAndroidInputManager::ParseEvent - No available joystick id for device %d, dropping button event", iDeviceId);
	}
	else
#endif
		if ((AInputEvent_getSource(event) & AINPUT_SOURCE_KEYBOARD) == AINPUT_SOURCE_KEYBOARD &&
			  AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
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
	else if ((AInputEvent_getSource(event) & AINPUT_SOURCE_TOUCHSCREEN) == AINPUT_SOURCE_TOUCHSCREEN &&
			  AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
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

#if (__ANDROID_API__ >= 13)
bool ncAndroidInputManager::isJoyPresent(int iJoyId) const
{
	bool bIsPresent = false;

	if (iJoyId >= 0 && iJoyId < int(s_uMaxNumJoysticks))
	{
		int iDeviceId = s_joystickStates[iJoyId].m_iDeviceId;
		if (iDeviceId > -1)
			bIsPresent = true;
	}

	return bIsPresent;
}

const char* ncAndroidInputManager::JoyName(int iJoyId) const
{
	if (isJoyPresent(iJoyId))
		return s_joystickStates[iJoyId].m_vName;
	else
		return '\0';
}

int ncAndroidInputManager::JoyNumButtons(int iJoyId) const
{
	int iNumButtons = -1;

	if (isJoyPresent(iJoyId))
		return ncAndroidJoystickState::s_uNumButtons; // TODO: Query available buttons

	return iNumButtons;
}

int ncAndroidInputManager::JoyNumAxes(int iJoyId) const
{
	int iNumAxes = -1;

	if (isJoyPresent(iJoyId))
		return ncAndroidJoystickState::s_uNumAxes; // TODO: Query available axes

	return iNumAxes;
}

bool ncAndroidInputManager::isJoyButtonPressed(int iJoyId, int iButtonId) const
{
	if (isJoyPresent(iJoyId) && iButtonId >=0 && iButtonId < JoyNumButtons(iJoyId))
		return s_joystickStates[iJoyId].m_bButtons[iButtonId];
	else
		return false;
}

short int ncAndroidInputManager::JoyAxisValue(int iJoyId, int iAxisId) const
{
	// If the joystick is not present the returned value is zero
	short int iAxisValue = JoyAxisNormValue(iJoyId, iAxisId) * s_iMaxAxisValue;

	return iAxisValue;
}

float ncAndroidInputManager::JoyAxisNormValue(int iJoyId, int iAxisId) const
{
	float fAxisValue = 0.0f;

	if (isJoyPresent(iJoyId) && iAxisId >= 0 && iAxisId < JoyNumAxes(iJoyId))
		fAxisValue = s_joystickStates[iJoyId].m_fAxisValues[iAxisId];

	return fAxisValue;
}

void ncAndroidInputManager::AttachJVM(JavaVM *pJVM)
{
	s_pJVM = pJVM;

	if (s_pJVM == NULL)
		ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidInputManager::AttachJVM - JavaVM pointer is null");

	s_pJVM->AttachCurrentThread(&s_pEnv, NULL);
	if (s_pJVM == NULL)
		ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidInputManager::AttachJVM - JNIEnv pointer is null");

	s_clsInputDevice = s_pEnv->FindClass("android/view/InputDevice");
	if (s_clsInputDevice == NULL)
		ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidInputManager::AttachJVM - jclass pointer is null");

	s_midGetDevice = s_pEnv->GetStaticMethodID(s_clsInputDevice, "getDevice", "(I)Landroid/view/InputDevice;");
	if (s_midGetDevice == NULL)
		ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidInputManager::AttachJVM - jmethod1 pointer is null");

	s_midGetName = s_pEnv->GetMethodID(s_clsInputDevice, "getName", "()Ljava/lang/String;");
	if (s_midGetName == NULL)
		ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidInputManager::AttachJVM - jmethod2 pointer is null");
}

void ncAndroidInputManager::DetachJVM()
{
	if (s_pJVM)
	{
		s_pJVM->DetachCurrentThread();
		ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAndroidInputManager::DetachJVM - Thread detached");
		s_pJVM = NULL;
		s_pEnv = NULL;
	}
}

/// Updates joystick state structures and raises disconnection events
void ncAndroidInputManager::UpdateJoystickStates()
{
	for(unsigned int i = 0; i < s_uMaxNumJoysticks; i++)
	{
		int iDeviceId = s_joystickStates[i].m_iDeviceId;
		if(iDeviceId > -1 && isDeviceConnected(iDeviceId) == false)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAndroidInputManager::isJoyPresent - Joystick %d (device %d) \"%s\" has been disconnected",
				i, iDeviceId, s_joystickStates[i].m_vName);
			s_joystickStates[i].m_iDeviceId = -1;
		}
	}
}
#endif

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

#if (__ANDROID_API__ >= 13)
int ncAndroidInputManager::FindJoyId(int iDeviceId)
{
	int iJoyId = -1;

	for(unsigned int i = 0; i < s_uMaxNumJoysticks; i++)
	{
		// Keeping track of the first unused joystick id, in
		// case this is the first event from a new joystick
		if (s_joystickStates[i].m_iDeviceId < 0 && iJoyId == -1)
		{
			iJoyId = i;
		}
		// If the joystick is already known then the loop ends
		else if (s_joystickStates[i].m_iDeviceId == iDeviceId)
		{
			iJoyId = i;
			break;
		}
	}

	if (iJoyId >= -1 && s_joystickStates[iJoyId].m_iDeviceId != iDeviceId)
	{
		DeviceName(iDeviceId, iJoyId);
		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAndroidInputManager::FindJoyId - Joystick %d (device %d) \"%s\" has been connected",
			iJoyId, iDeviceId, s_joystickStates[iJoyId].m_vName);

		// TODO: handle connection as en event
	}

	return iJoyId;
}

bool ncAndroidInputManager::isDeviceConnected(int iDeviceId)
{
	bool bIsConnected = false;

	jobject objInputDevice = s_pEnv->CallStaticObjectMethod(s_clsInputDevice, s_midGetDevice, iDeviceId);
	if (objInputDevice)
	{
		s_pEnv->DeleteLocalRef(objInputDevice);
		bIsConnected = true;
	}

	return bIsConnected;
}

void ncAndroidInputManager::DeviceName(int iDeviceId, int iJoyId)
{
	jobject objInputDevice = s_pEnv->CallStaticObjectMethod(s_clsInputDevice, s_midGetDevice, iDeviceId);
	if (objInputDevice)
	{
		jstring jDeviceName = (jstring)s_pEnv->CallObjectMethod(objInputDevice, s_midGetName);
		if (jDeviceName)
		{
			const char *vDeviceName = s_pEnv->GetStringUTFChars(jDeviceName, 0);
			strncpy(s_joystickStates[iJoyId].m_vName, vDeviceName, ncAndroidJoystickState::s_uMaxNameLength);
			s_joystickStates[iJoyId].m_vName[ncAndroidJoystickState::s_uMaxNameLength-1] = '\0';
			s_pEnv->ReleaseStringUTFChars(jDeviceName, vDeviceName);
			s_pEnv->DeleteLocalRef(jDeviceName);
		}
		else
			strncpy(s_joystickStates[iJoyId].m_vName, (const char *)"Unknown", ncAndroidJoystickState::s_uMaxNameLength);

		s_pEnv->DeleteLocalRef(objInputDevice);
	}
}
#endif
