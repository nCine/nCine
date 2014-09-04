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
ncJoyButtonEvent ncAndroidInputManager::s_joyButtonEvent;
ncJoyAxisEvent ncAndroidInputManager::s_joyAxisEvent;
const int ncAndroidJoystickState::s_vAxesToMap[ncAndroidJoystickState::s_iNumAxesToMap] =
{ AMOTION_EVENT_AXIS_X,			AMOTION_EVENT_AXIS_Y,
  AMOTION_EVENT_AXIS_LTRIGGER,	AMOTION_EVENT_AXIS_Z,
  AMOTION_EVENT_AXIS_RZ,		AMOTION_EVENT_AXIS_RTRIGGER,
  AMOTION_EVENT_AXIS_RX,		AMOTION_EVENT_AXIS_RY };

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncAndroidInputManager::ncAndroidInputManager(struct android_app* state)
{
	InitAccelerometerSensor(state);
	InitJoyIds();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

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

	// Checking for gamepad events first
	if ((AInputEvent_getSource(event) & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD ||
		(AInputEvent_getSource(event) & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK ||
		(AInputEvent_getSource(event) & AINPUT_SOURCE_DPAD) == AINPUT_SOURCE_DPAD)
	{
		int iDeviceId = AInputEvent_getDeviceId(event);
		int iJoyId = FindJoyId(iDeviceId);

		// If the index is valid then the structure can be updated
		if (iJoyId > -1)
		{
			s_joystickStates[iJoyId].m_iDeviceId = iDeviceId;

			if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
			{
				int iKeyCode = AKeyEvent_getKeyCode(event);
				int iBtnIndex = -1;
				if (iKeyCode >= AKEYCODE_BUTTON_A && iKeyCode < AKEYCODE_ESCAPE)
					iBtnIndex = s_joystickStates[iJoyId].m_vButtonsMapping[iKeyCode - AKEYCODE_BUTTON_A];

				if (iBtnIndex > -1)
				{
					s_joyButtonEvent.joyId = iJoyId;
					s_joyButtonEvent.buttonId = iBtnIndex;
					switch(AKeyEvent_getAction(event))
					{
						case AKEY_EVENT_ACTION_DOWN:
							s_joystickStates[iJoyId].m_bButtons[iBtnIndex] = true;
							s_pInputEventHandler->OnJoyButtonPressed(s_joyButtonEvent);
							break;
						case AKEY_EVENT_ACTION_UP:
							s_joystickStates[iJoyId].m_bButtons[iBtnIndex] = false;
							s_pInputEventHandler->OnJoyButtonReleased(s_joyButtonEvent);
							break;
						case AKEY_EVENT_ACTION_MULTIPLE:
							break;
					}
				}

				// Handling a D-Pad button event as an axis value
				if (iKeyCode >= AKEYCODE_DPAD_UP && iKeyCode < AKEYCODE_DPAD_CENTER)
				{
					int iAxisIdx = -1;
					float fAxisValue = 1.0f;
					switch(iKeyCode)
					{
						case AKEYCODE_DPAD_UP:
							iAxisIdx = s_joystickStates[iJoyId].m_iNumAxes-1;
							fAxisValue = 1.0f;
							break;
						case AKEYCODE_DPAD_DOWN:
							iAxisIdx = s_joystickStates[iJoyId].m_iNumAxes-1;
							fAxisValue = -1.0f;
							break;
						case AKEYCODE_DPAD_LEFT:
							iAxisIdx = s_joystickStates[iJoyId].m_iNumAxes-2;
							fAxisValue = -1.0f;
							break;
						case AKEYCODE_DPAD_RIGHT:
							iAxisIdx = s_joystickStates[iJoyId].m_iNumAxes-2;
							fAxisValue = 1.0f;
							break;
					}

					if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN)
						s_joystickStates[iJoyId].m_fAxisValues[iAxisIdx] = fAxisValue;
					else
						s_joystickStates[iJoyId].m_fAxisValues[iAxisIdx] = 0.0f;

					s_joyAxisEvent.joyId = iJoyId;
					s_joyAxisEvent.axisId = iAxisIdx;
					s_joyAxisEvent.value = fAxisValue * s_iMaxAxisValue;
					s_joyAxisEvent.normValue = fAxisValue;
					s_pInputEventHandler->OnJoyAxisMoved(s_joyAxisEvent);
				}
			}
			else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
			{
				s_joyAxisEvent.joyId = iJoyId;

				int iNumAxesNoDPad = s_joystickStates[iJoyId].m_iNumAxes;
				if (s_joystickStates[iJoyId].m_bHasDPad)
					iNumAxesNoDPad -= 2;
				for(int i = 0; i < iNumAxesNoDPad; i++)
				{
					int iAxis = s_joystickStates[iJoyId].m_vAxesMapping[i];
					float fAxisValue = AMotionEvent_getAxisValue(event, iAxis, 0);
					s_joystickStates[iJoyId].m_fAxisValues[i] = fAxisValue;

					s_joyAxisEvent.axisId = iAxis;
					s_joyAxisEvent.value = fAxisValue * s_iMaxAxisValue;
					s_joyAxisEvent.normValue = fAxisValue;
					s_pInputEventHandler->OnJoyAxisMoved(s_joyAxisEvent);
				}
			}
		}
		else
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncAndroidInputManager::ParseEvent - No available joystick id for device %d, dropping button event", iDeviceId);
	}
	else if ((AInputEvent_getSource(event) & AINPUT_SOURCE_KEYBOARD) == AINPUT_SOURCE_KEYBOARD &&
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
		iNumButtons = s_joystickStates[iJoyId].m_iNumButtons;

	return iNumButtons;
}

int ncAndroidInputManager::JoyNumAxes(int iJoyId) const
{
	int iNumAxes = -1;

	if (isJoyPresent(iJoyId))
		iNumAxes = s_joystickStates[iJoyId].m_iNumAxes;

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



///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
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

/// Updates joystick state structures
void ncAndroidInputManager::UpdateJoystickStates()
{
	for(unsigned int i = 0; i < s_uMaxNumJoysticks; i++)
	{
		int iDeviceId = s_joystickStates[i].m_iDeviceId;
		if(iDeviceId > -1 && isDeviceConnected(iDeviceId) == false)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAndroidInputManager::UpdateJoystickStates - Joystick %d (device %d) \"%s\" has been disconnected",
				i, iDeviceId, s_joystickStates[i].m_vName);
			s_joystickStates[i].m_iDeviceId = -1;
		}
	}
}

void ncAndroidInputManager::InitJoyIds()
{
	const int iMaxDevs = s_uMaxNumJoysticks * 2;
	int iDevIds[iMaxDevs];

	int iConnectedJoys = 0;
	int iConnectedDevs = ncAndroidJNIClass_InputDevice::getDeviceIds(iDevIds, iMaxDevs);
	for (int i = 0; i < iMaxDevs && i < iConnectedDevs; i++)
	{
		ncAndroidJNIClass_InputDevice inputDevice = ncAndroidJNIClass_InputDevice::getDevice(iDevIds[i]);
		int sources = inputDevice.getSources();

		if (((sources & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD) ||
			((sources & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK))
		{
			int iJoyId = FindJoyId(iDevIds[i]);
			if (iJoyId > -1)
				s_joystickStates[iJoyId].m_iDeviceId = iDevIds[i];

			iConnectedJoys++;
			if (iConnectedJoys >= int(s_uMaxNumJoysticks))
				break;
		}
	}
}

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
		DeviceInfo(iDeviceId, iJoyId);
		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAndroidInputManager::FindJoyId - Joystick %d (device %d) \"%s\" has been connected - %d axes, %d buttons",
			iJoyId, iDeviceId, s_joystickStates[iJoyId].m_vName, s_joystickStates[iJoyId].m_iNumAxes, s_joystickStates[iJoyId].m_iNumButtons);

		// TODO: handle connection as en event
	}

	return iJoyId;
}

bool ncAndroidInputManager::isDeviceConnected(int iDeviceId)
{
	bool bIsConnected = false;

	ncAndroidJNIClass_InputDevice inputDevice = ncAndroidJNIClass_InputDevice::getDevice(iDeviceId);
	if (inputDevice.isNull() == false)
		bIsConnected = true;

	return bIsConnected;
}

void ncAndroidInputManager::DeviceInfo(int iDeviceId, int iJoyId)
{
	ncAndroidJNIClass_InputDevice inputDevice = ncAndroidJNIClass_InputDevice::getDevice(iDeviceId);
	if (inputDevice.isNull() == false)
	{
		// InputDevice.getName()
		inputDevice.getName(s_joystickStates[iJoyId].m_vName, ncAndroidJoystickState::s_uMaxNameLength);

		// KeyCharacterMap.deviceHasKey()
		int iNumButtons = 0;
		for(int iButton = AKEYCODE_BUTTON_A; iButton < AKEYCODE_ESCAPE; iButton++)
		{
			bool bHasKey = ncAndroidJNIClass_KeyCharacterMap::deviceHasKey(iButton);
			if (bHasKey)
			{
				s_joystickStates[iJoyId].m_vButtonsMapping[iButton - AKEYCODE_BUTTON_A] = iNumButtons;
				ncServiceLocator::Logger().Write(ncILogger::LOG_VERBOSE, (const char *)"ncAndroidInputManager::DeviceInfo (%d, %d) - Button %d : %d", iDeviceId, iJoyId, iNumButtons, iButton);
				iNumButtons++;			
			}
			else
				s_joystickStates[iJoyId].m_vButtonsMapping[iButton - AKEYCODE_BUTTON_A] = -1;

			if(iNumButtons >= ncAndroidJoystickState::s_iMaxButtons)
				break;
		}
		s_joystickStates[iJoyId].m_iNumButtons = iNumButtons;

		s_joystickStates[iJoyId].m_bHasDPad = true;
		for(int iButton = AKEYCODE_DPAD_UP; iButton < AKEYCODE_DPAD_CENTER; iButton++)
		{
			bool bHasKey = ncAndroidJNIClass_KeyCharacterMap::deviceHasKey(iButton);
			if (bHasKey == false)
			{
				s_joystickStates[iJoyId].m_bHasDPad = false;
				ncServiceLocator::Logger().Write(ncILogger::LOG_VERBOSE, (const char *)"ncAndroidInputManager::DeviceInfo (%d, %d) - D-Pad not detected", iDeviceId, iJoyId);
				break;
			}
		}

		// InputDevice.getMotionRange()
		int iNumAxes = 0;
		for(int i = 0; i < ncAndroidJoystickState::s_iNumAxesToMap; i++)
		{
			int iAxis = ncAndroidJoystickState::s_vAxesToMap[i];
			ncAndroidJNIClass_MotionRange motionRange = inputDevice.getMotionRange(iAxis);

			if(motionRange.isNull() == false)
			{
				s_joystickStates[iJoyId].m_vAxesMapping[iNumAxes] = iAxis;
				ncServiceLocator::Logger().Write(ncILogger::LOG_VERBOSE, (const char *)"ncAndroidInputManager::DeviceInfo (%d, %d) - Axis %d: %d", iDeviceId, iJoyId, iNumAxes, iAxis);
				iNumAxes++;
			}
		}
		s_joystickStates[iJoyId].m_iNumAxes = iNumAxes;
		// Taking into account the D-Pad
		if (s_joystickStates[iJoyId].m_bHasDPad)
			s_joystickStates[iJoyId].m_iNumAxes += 2;
	}
}
