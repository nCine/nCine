#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/sensor.h>

#include "AndroidInputManager.h"
#include "IInputEventHandler.h"
#include "AndroidJniHelper.h"
#include "AndroidApplication.h"
#include "ServiceLocator.h"
#include "Timer.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

IInputEventHandler* IInputManager::inputEventHandler_ = NULL;
ASensorManager* AndroidInputManager::sensorManager_ = NULL;
const ASensor* AndroidInputManager::accelerometerSensor_ = NULL;
ASensorEventQueue* AndroidInputManager::sensorEventQueue_ = NULL;
bool AndroidInputManager::accelerometerEnabled_ = false;
AccelerometerEvent AndroidInputManager::accelerometerEvent_;
TouchEvent AndroidInputManager::touchEvent_;
KeyboardEvent AndroidInputManager::keyboardEvent_;

short int IInputManager::MaxAxisValue = 32767;
AndroidJoystickState AndroidInputManager::joystickStates_[MaxNumJoysticks];
JoyButtonEvent AndroidInputManager::joyButtonEvent_;
JoyAxisEvent AndroidInputManager::joyAxisEvent_;
JoyConnectionEvent AndroidInputManager::joyConnectionEvent_;
const float AndroidInputManager::JoyCheckRate = 0.25f;
Timer AndroidInputManager::joyCheckTimer_;
const int AndroidJoystickState::AxesToMap[AndroidJoystickState::NumAxesToMap] =
{
	AMOTION_EVENT_AXIS_X,			AMOTION_EVENT_AXIS_Y,
	AMOTION_EVENT_AXIS_LTRIGGER,	AMOTION_EVENT_AXIS_Z,
	AMOTION_EVENT_AXIS_RZ,			AMOTION_EVENT_AXIS_RTRIGGER,
	AMOTION_EVENT_AXIS_RX,			AMOTION_EVENT_AXIS_RY,
	AMOTION_EVENT_AXIS_HAT_X,		AMOTION_EVENT_AXIS_HAT_Y
};

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AndroidJoystickState::AndroidJoystickState()
	: deviceId_(-1), numButtons_(0), numAxes_(0), hasDPad_(false)
{
	name_[0] = '\0';
	for (int i = 0; i < MaxButtons; i++)
	{
		buttonsMapping_[i] = 0;
		buttons_[i] = false;
	}
	for (int i = 0; i < MaxAxes; i++)
	{
		axesMapping_[i] = 0;
		axisValues_[i] = 0.0f;
	}
}

AndroidInputManager::AndroidInputManager(struct android_app* state)
{
	initAccelerometerSensor(state);
	checkConnectedJoysticks();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Enables the accelerometer sensor
/*! It is called by EnableAccelerometer() and when the application gains focus */
void AndroidInputManager::enableAccelerometerSensor()
{
	if (accelerometerEnabled_ && accelerometerSensor_ != NULL)
	{
		ASensorEventQueue_enableSensor(sensorEventQueue_, accelerometerSensor_);
		// 60 events per second
		ASensorEventQueue_setEventRate(sensorEventQueue_, accelerometerSensor_, (1000L / 60) * 1000);
	}
}

/// Disables the accelerometer sensor
/*! It is called by EnableAccelerometer() and when the application loses focus */
void AndroidInputManager::disableAccelerometerSensor()
{
	if (accelerometerEnabled_ && accelerometerSensor_ != NULL)
	{
		ASensorEventQueue_disableSensor(sensorEventQueue_, accelerometerSensor_);
	}
}

/// Allows the application to make use of the accelerometer
/*! Activates the sensor and raises the flag needed for application focus handling */
void AndroidInputManager::enableAccelerometer(bool enabled)
{
	if (enabled)
	{
		enableAccelerometerSensor();
	}
	else
	{
		disableAccelerometerSensor();
	}

	accelerometerEnabled_ = enabled;
}

/// Parses an Android sensor event related to the accelerometer
void AndroidInputManager::parseAccelerometerEvent()
{
	if (inputEventHandler_ != NULL && accelerometerEnabled_ && accelerometerSensor_ != NULL)
	{
		ASensorEvent event;
		while (ASensorEventQueue_getEvents(sensorEventQueue_, &event, 1) > 0)
		{
			accelerometerEvent_.x = event.acceleration.x;
			accelerometerEvent_.y = event.acceleration.y;
			accelerometerEvent_.z = event.acceleration.z;
			inputEventHandler_->onAcceleration(accelerometerEvent_);
		}
	}
}

/// Parses an Android input event
bool AndroidInputManager::parseEvent(const AInputEvent *event)
{
	bool isEventHandled = false;

	// Checking for gamepad events first
	if (inputEventHandler_ != NULL &&
		((AInputEvent_getSource(event) & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD ||
		(AInputEvent_getSource(event) & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK ||
		(AInputEvent_getSource(event) & AINPUT_SOURCE_DPAD) == AINPUT_SOURCE_DPAD))
	{
		int deviceId = AInputEvent_getDeviceId(event);
		int joyId = findJoyId(deviceId);

		// If the index is valid then the structure can be updated
		if (joyId > -1)
		{
			joystickStates_[joyId].deviceId_ = deviceId;

			if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
			{
				int keyCode = AKeyEvent_getKeyCode(event);
				int buttonIndex = -1;
				if (keyCode >= AKEYCODE_BUTTON_A && keyCode < AKEYCODE_ESCAPE)
				{
					buttonIndex = joystickStates_[joyId].buttonsMapping_[keyCode - AKEYCODE_BUTTON_A];
				}

				if (buttonIndex > -1)
				{
					joyButtonEvent_.joyId = joyId;
					joyButtonEvent_.buttonId = buttonIndex;
					switch (AKeyEvent_getAction(event))
					{
						case AKEY_EVENT_ACTION_DOWN:
							joystickStates_[joyId].buttons_[buttonIndex] = true;
							inputEventHandler_->onJoyButtonPressed(joyButtonEvent_);
							break;
						case AKEY_EVENT_ACTION_UP:
							joystickStates_[joyId].buttons_[buttonIndex] = false;
							inputEventHandler_->onJoyButtonReleased(joyButtonEvent_);
							break;
						case AKEY_EVENT_ACTION_MULTIPLE:
							break;
					}
				}

				// Handling a D-Pad button event as an axis value
				if (keyCode >= AKEYCODE_DPAD_UP && keyCode < AKEYCODE_DPAD_CENTER)
				{
					int axisIndex = -1;
					float axisValue = 1.0f;
					switch (keyCode)
					{
						case AKEYCODE_DPAD_UP:
							axisIndex = joystickStates_[joyId].numAxes_ - 1;
							axisValue = -1.0f;
							break;
						case AKEYCODE_DPAD_DOWN:
							axisIndex = joystickStates_[joyId].numAxes_ - 1;
							axisValue = 1.0f;
							break;
						case AKEYCODE_DPAD_LEFT:
							axisIndex = joystickStates_[joyId].numAxes_ - 2;
							axisValue = -1.0f;
							break;
						case AKEYCODE_DPAD_RIGHT:
							axisIndex = joystickStates_[joyId].numAxes_ - 2;
							axisValue = 1.0f;
							break;
					}

					if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN)
					{
						joystickStates_[joyId].axisValues_[axisIndex] = axisValue;
					}
					else
					{
						joystickStates_[joyId].axisValues_[axisIndex] = 0.0f;
					}

					joyAxisEvent_.joyId = joyId;
					joyAxisEvent_.axisId = axisIndex;
					joyAxisEvent_.value = axisValue * MaxAxisValue;
					joyAxisEvent_.normValue = axisValue;
					inputEventHandler_->onJoyAxisMoved(joyAxisEvent_);
				}
			}
			else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
			{
				joyAxisEvent_.joyId = joyId;

				int numAxesNoDPad = joystickStates_[joyId].numAxes_;
				if (joystickStates_[joyId].hasDPad_)
				{
					numAxesNoDPad -= 2;
				}
				for (int i = 0; i < numAxesNoDPad; i++)
				{
					int axis = joystickStates_[joyId].axesMapping_[i];
					float axisValue = AMotionEvent_getAxisValue(event, axis, 0);
					joystickStates_[joyId].axisValues_[i] = axisValue;

					joyAxisEvent_.axisId = axis;
					joyAxisEvent_.value = axisValue * MaxAxisValue;
					joyAxisEvent_.normValue = axisValue;
					inputEventHandler_->onJoyAxisMoved(joyAxisEvent_);
				}
			}
		}
		else
		{
			LOGW_X("No available joystick id for device %d, dropping button event", deviceId);
		}

		isEventHandled = true;
	}
	else if ((AInputEvent_getSource(event) & AINPUT_SOURCE_KEYBOARD) == AINPUT_SOURCE_KEYBOARD &&
			  AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY &&
			 // Hardware volume keys are not handled by the engine
			 AKeyEvent_getKeyCode(event) != AKEYCODE_VOLUME_UP &&
			 AKeyEvent_getKeyCode(event) != AKEYCODE_VOLUME_DOWN)
	{
		keyboardEvent_.scancode = AKeyEvent_getScanCode(event);
		keyboardEvent_.sym = AndroidKeys::keySymValueToEnum(AKeyEvent_getKeyCode(event));
		keyboardEvent_.mod = AndroidKeys::keyModValueToEnum(AKeyEvent_getMetaState(event));

		switch (AKeyEvent_getAction(event))
		{
			case AKEY_EVENT_ACTION_DOWN:
				inputEventHandler_->onKeyPressed(keyboardEvent_);
				break;
			case AKEY_EVENT_ACTION_UP:
				inputEventHandler_->onKeyReleased(keyboardEvent_);
				break;
			case AKEY_EVENT_ACTION_MULTIPLE:
				break;
		}

		isEventHandled = true;
	}
	else if ((AInputEvent_getSource(event) & AINPUT_SOURCE_TOUCHSCREEN) == AINPUT_SOURCE_TOUCHSCREEN &&
			  AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		touchEvent_.count = AMotionEvent_getPointerCount(event);
		touchEvent_.id = AMotionEvent_getPointerId(event, 0);
		touchEvent_.x = AMotionEvent_getX(event, 0);
		touchEvent_.y = theApplication().height() - AMotionEvent_getY(event, 0);
		touchEvent_.id2 = AMotionEvent_getPointerId(event, 1);
		touchEvent_.x2 = AMotionEvent_getX(event, 1);
		touchEvent_.y2 = theApplication().height() - AMotionEvent_getY(event, 1);

		switch (AMotionEvent_getAction(event))
		{
			case AMOTION_EVENT_ACTION_DOWN:
				inputEventHandler_->onTouchDown(touchEvent_);
				break;
			case AMOTION_EVENT_ACTION_UP:
				inputEventHandler_->onTouchUp(touchEvent_);
				break;
			case AMOTION_EVENT_ACTION_MOVE:
				inputEventHandler_->onTouchMove(touchEvent_);
				break;
			case AMOTION_EVENT_ACTION_POINTER_DOWN:
				inputEventHandler_->onSecondaryTouchDown(touchEvent_);
				break;
			case AMOTION_EVENT_ACTION_POINTER_UP:
				inputEventHandler_->onSecondaryTouchUp(touchEvent_);
				break;
		}

		isEventHandled = true;
	}

	return isEventHandled;
}

bool AndroidInputManager::isJoyPresent(int joyId) const
{
	bool isPresent = false;

	if (joyId >= 0 && joyId < int(MaxNumJoysticks))
	{
		int deviceId = joystickStates_[joyId].deviceId_;
		if (deviceId > -1)
		{
			isPresent = true;
		}
	}

	return isPresent;
}

const char *AndroidInputManager::joyName(int joyId) const
{
	if (isJoyPresent(joyId))
	{
		return joystickStates_[joyId].name_;
	}
	else
	{
		return NULL;
	}
}

int AndroidInputManager::joyNumButtons(int joyId) const
{
	int numButtons = -1;

	if (isJoyPresent(joyId))
	{
		numButtons = joystickStates_[joyId].numButtons_;
	}

	return numButtons;
}

int AndroidInputManager::joyNumAxes(int joyId) const
{
	int numAxes = -1;

	if (isJoyPresent(joyId))
	{
		numAxes = joystickStates_[joyId].numAxes_;
	}

	return numAxes;
}

bool AndroidInputManager::isJoyButtonPressed(int joyId, int buttonId) const
{
	if (isJoyPresent(joyId) && buttonId >= 0 && buttonId < joyNumButtons(joyId))
	{
		return joystickStates_[joyId].buttons_[buttonId];
	}
	else
	{
		return false;
	}
}

short int AndroidInputManager::joyAxisValue(int joyId, int axisId) const
{
	// If the joystick is not present the returned value is zero
	short int axisValue = joyAxisNormValue(joyId, axisId) * MaxAxisValue;

	return axisValue;
}

float AndroidInputManager::joyAxisNormValue(int joyId, int axisId) const
{
	float axisValue = 0.0f;

	if (isJoyPresent(joyId) && axisId >= 0 && axisId < joyNumAxes(joyId))
	{
		axisValue = joystickStates_[joyId].axisValues_[axisId];
	}

	return axisValue;
}



///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes the accelerometer sensor
void AndroidInputManager::initAccelerometerSensor(android_app *state)
{
	// Prepare to monitor accelerometer
	sensorManager_ = ASensorManager_getInstance();
	accelerometerSensor_ = ASensorManager_getDefaultSensor(sensorManager_,
		ASENSOR_TYPE_ACCELEROMETER);
	sensorEventQueue_ = ASensorManager_createEventQueue(sensorManager_,
		state->looper, LOOPER_ID_USER, NULL, NULL);

	if (accelerometerSensor_ == NULL)
	{
		LOGW("No accelerometer sensor available");
	}
}

/// Updates joystick states after connections and disconnections
void AndroidInputManager::updateJoystickConnections()
{
	if (joyCheckTimer_.interval() >= JoyCheckRate)
	{
		checkDisconnectedJoysticks();
		checkConnectedJoysticks();

		joyCheckTimer_.start();
	}
}

/// Checks if a previously connected joystick has been disconnected
void AndroidInputManager::checkDisconnectedJoysticks()
{
	for (unsigned int i = 0; i < MaxNumJoysticks; i++)
	{
		int deviceId = joystickStates_[i].deviceId_;
		if (deviceId > -1 && isDeviceConnected(deviceId) == false)
		{
			LOGI_X("Joystick %d (device %d) \"%s\" has been disconnected", i, deviceId, joystickStates_[i].name_);
			joystickStates_[i].deviceId_ = -1;

			if (inputEventHandler_ != NULL)
			{
				joyConnectionEvent_.joyId = i;
				inputEventHandler_->onJoyDisconnected(joyConnectionEvent_);
			}
		}
	}
}

/// Checks if a new joystick has been connected
void AndroidInputManager::checkConnectedJoysticks()
{
	// InputDevice.getDeviceIds() will not fill an array longer than MaxDevices
	const int MaxDevices = MaxNumJoysticks * 2;
	int deviceIds[MaxDevices];

	int connectedJoys = 0;
	for (unsigned int i = 0; i < MaxNumJoysticks; i++)
	{
		if (joystickStates_[i].deviceId_ > -1)
		{
			connectedJoys++;
		}
	}

	int connectedDevices = AndroidJniClass_InputDevice::getDeviceIds(deviceIds, MaxDevices);
	for (int i = 0; i < MaxDevices && i < connectedDevices; i++)
	{
		AndroidJniClass_InputDevice inputDevice = AndroidJniClass_InputDevice::getDevice(deviceIds[i]);
		int sources = inputDevice.getSources();

		if (((sources & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD) ||
			((sources & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK))
		{
			int joyId = findJoyId(deviceIds[i]);
			if (joyId > -1)
			{
				joystickStates_[joyId].deviceId_ = deviceIds[i];
			}

			connectedJoys++;
			if (connectedJoys >= int(MaxNumJoysticks))
			{
				break;
			}
		}
	}
}

int AndroidInputManager::findJoyId(int deviceId)
{
	int joyId = -1;

	for (unsigned int i = 0; i < MaxNumJoysticks; i++)
	{
		// Keeping track of the first unused joystick id, in
		// case this is the first event from a new joystick
		if (joystickStates_[i].deviceId_ < 0 && joyId == -1)
		{
			joyId = i;
		}
		// If the joystick is already known then the loop ends
		else if (joystickStates_[i].deviceId_ == deviceId)
		{
			joyId = i;
			break;
		}
	}

	if (joyId > -1 && joystickStates_[joyId].deviceId_ != deviceId)
	{
		deviceInfo(deviceId, joyId);
		LOGI_X("Joystick %d (device %d) \"%s\" has been connected - %d axes, %d buttons",
			joyId, deviceId, joystickStates_[joyId].name_, joystickStates_[joyId].numAxes_, joystickStates_[joyId].numButtons_);

		if (inputEventHandler_ != NULL)
		{
			joyConnectionEvent_.joyId = joyId;
			inputEventHandler_->onJoyConnected(joyConnectionEvent_);
		}
	}

	return joyId;
}

bool AndroidInputManager::isDeviceConnected(int deviceId)
{
	bool isConnected = false;

	AndroidJniClass_InputDevice inputDevice = AndroidJniClass_InputDevice::getDevice(deviceId);
	if (inputDevice.isNull() == false)
	{
		isConnected = true;
	}

	return isConnected;
}

void AndroidInputManager::deviceInfo(int deviceId, int joyId)
{
	const int MaxStringLength = 256;
	char deviceInfoString[MaxStringLength];

	AndroidJniClass_InputDevice inputDevice = AndroidJniClass_InputDevice::getDevice(deviceId);
	if (inputDevice.isNull() == false)
	{
		// InputDevice.getName()
		inputDevice.getName(joystickStates_[joyId].name_, AndroidJoystickState::MaxNameLength);

		const int FirstButton = AKEYCODE_BUTTON_A;
		const int LastButton = AKEYCODE_ESCAPE;
		bool checkedButtons[LastButton - FirstButton];
		int numButtons = 0;
		if (AndroidJniHelper::sdkVersion() >= 19 && __ANDROID_API__ >= 19)
		{
			int buttonsToCheck[LastButton - FirstButton];
			for (int i = 0; i < LastButton - FirstButton; i++)
			{
				buttonsToCheck[i] = FirstButton + i;
			}

			// InoutDevice.hasKeys()
			inputDevice.hasKeys(buttonsToCheck, LastButton - FirstButton, checkedButtons);
		}

		memset(deviceInfoString, 0, MaxStringLength);
		for (int button = FirstButton; button < LastButton; button++)
		{
			bool hasKey;

			if (AndroidJniHelper::sdkVersion() >= 19 && __ANDROID_API__ >= 19)
			{
				hasKey = checkedButtons[button - FirstButton];
			}
			else // KeyCharacterMap.deviceHasKey()
			{
				hasKey = AndroidJniClass_KeyCharacterMap::deviceHasKey(button);
			}

			if (hasKey)
			{
				joystickStates_[joyId].buttonsMapping_[button - FirstButton] = numButtons;
				sprintf(&deviceInfoString[strlen(deviceInfoString)], "%d:%d ", numButtons, button);
				numButtons++;
			}
			else
			{
				joystickStates_[joyId].buttonsMapping_[button - FirstButton] = -1;
			}

			if (numButtons >= AndroidJoystickState::MaxButtons)
			{
				break;
			}
		}
		joystickStates_[joyId].numButtons_ = numButtons;
		LOGV_X("device (%d, %d) - Buttons %s", deviceId, joyId, deviceInfoString);

		joystickStates_[joyId].hasDPad_ = true;
		for (int button = AKEYCODE_DPAD_UP; button < AKEYCODE_DPAD_CENTER; button++)
		{
			bool hasKey = AndroidJniClass_KeyCharacterMap::deviceHasKey(button);
			if (hasKey == false)
			{
				joystickStates_[joyId].hasDPad_ = false;
				LOGV_X("device (%d, %d) - D-Pad not detected", deviceId, joyId);
				break;
			}
		}

		memset(deviceInfoString, 0, MaxStringLength);
		// InputDevice.getMotionRange()
		int numAxes = 0;
		for (int i = 0; i < AndroidJoystickState::NumAxesToMap; i++)
		{
			int axis = AndroidJoystickState::AxesToMap[i];
			AndroidJniClass_MotionRange motionRange = inputDevice.getMotionRange(axis);

			if (motionRange.isNull() == false)
			{
				joystickStates_[joyId].axesMapping_[numAxes] = axis;
				sprintf(&deviceInfoString[strlen(deviceInfoString)], "%d:%d ", numAxes, axis);
				numAxes++;
			}
		}
		LOGV_X("device (%d, %d) - Axes %s", deviceId, joyId, deviceInfoString);

		joystickStates_[joyId].numAxes_ = numAxes;
		// Taking into account the D-Pad
		if (joystickStates_[joyId].hasDPad_)
		{
			joystickStates_[joyId].numAxes_ += 2;
		}
	}
}

}
