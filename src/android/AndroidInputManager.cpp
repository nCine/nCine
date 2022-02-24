#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/sensor.h>
#include <cstring> // for memset()

#include "AndroidInputManager.h"
#include "IInputEventHandler.h"
#include "AndroidJniHelper.h"
#include "AndroidApplication.h"
#include "Timer.h"
#include "JoyMapping.h"

#ifdef WITH_IMGUI
	#include "ImGuiAndroidInput.h"
#endif

#ifdef WITH_NUKLEAR
	#include "NuklearAndroidInput.h"
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const int IInputManager::MaxNumJoysticks = 4;

ASensorManager *AndroidInputManager::sensorManager_ = nullptr;
const ASensor *AndroidInputManager::accelerometerSensor_ = nullptr;
ASensorEventQueue *AndroidInputManager::sensorEventQueue_ = nullptr;
bool AndroidInputManager::accelerometerEnabled_ = false;
AccelerometerEvent AndroidInputManager::accelerometerEvent_;
TouchEvent AndroidInputManager::touchEvent_;
AndroidKeyboardState AndroidInputManager::keyboardState_;
KeyboardEvent AndroidInputManager::keyboardEvent_;
TextInputEvent AndroidInputManager::textInputEvent_;
AndroidMouseState AndroidInputManager::mouseState_;
AndroidMouseEvent AndroidInputManager::mouseEvent_;
ScrollEvent AndroidInputManager::scrollEvent_;
int AndroidInputManager::simulatedMouseButtonState_ = 0;

AndroidJoystickState AndroidInputManager::nullJoystickState_;
AndroidJoystickState AndroidInputManager::joystickStates_[MaxNumJoysticks];
JoyButtonEvent AndroidInputManager::joyButtonEvent_;
JoyHatEvent AndroidInputManager::joyHatEvent_;
JoyAxisEvent AndroidInputManager::joyAxisEvent_;
JoyConnectionEvent AndroidInputManager::joyConnectionEvent_;
const float AndroidInputManager::JoyCheckRate = 0.25f;
Timer AndroidInputManager::joyCheckTimer_;
const int AndroidJoystickState::AxesToMap[AndroidJoystickState::NumAxesToMap] = {
	AMOTION_EVENT_AXIS_X, AMOTION_EVENT_AXIS_Y,
	AMOTION_EVENT_AXIS_Z, AMOTION_EVENT_AXIS_RZ,
	AMOTION_EVENT_AXIS_LTRIGGER, AMOTION_EVENT_AXIS_RTRIGGER,
	AMOTION_EVENT_AXIS_HAT_X, AMOTION_EVENT_AXIS_HAT_Y
};

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AndroidJoystickState::AndroidJoystickState()
    : deviceId_(-1), numButtons_(0), numAxes_(0),
      hasDPad_(false), hasHatAxes_(false), hatState_(HatState::CENTERED)
{
	guid_[0] = '\0';
	name_[0] = '\0';
	for (int i = 0; i < MaxButtons; i++)
	{
		buttonsMapping_[i] = 0;
		buttons_[i] = false;
	}
	for (int i = 0; i < MaxAxes; i++)
	{
		axesMapping_[i] = 0;
		axesValues_[i] = 0.0f;
	}
}

AndroidInputManager::AndroidInputManager(struct android_app *state)
{
	initAccelerometerSensor(state);
	joyMapping_.init(this);
	checkConnectedJoysticks();

#ifdef WITH_IMGUI
	ImGuiAndroidInput::init(state->window);
#endif

#ifdef WITH_NUKLEAR
	NuklearAndroidInput::init();
#endif
}

AndroidInputManager::~AndroidInputManager()
{
#ifdef WITH_NUKLEAR
	NuklearAndroidInput::shutdown();
#endif

#ifdef WITH_IMGUI
	ImGuiAndroidInput::shutdown();
#endif
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool AndroidMouseState::isLeftButtonDown() const
{
	return (buttonState_ & AMOTION_EVENT_BUTTON_PRIMARY) != 0;
}

bool AndroidMouseState::isMiddleButtonDown() const
{
	return (buttonState_ & AMOTION_EVENT_BUTTON_TERTIARY) != 0;
}

bool AndroidMouseState::isRightButtonDown() const
{
	return (buttonState_ & AMOTION_EVENT_BUTTON_SECONDARY) != 0;
}

bool AndroidMouseState::isFourthButtonDown() const
{
	return (buttonState_ & AMOTION_EVENT_BUTTON_BACK) != 0;
}

bool AndroidMouseState::isFifthButtonDown() const
{
	return (buttonState_ & AMOTION_EVENT_BUTTON_FORWARD) != 0;
}

bool AndroidMouseEvent::isLeftButton() const
{
	return (button_ & AMOTION_EVENT_BUTTON_PRIMARY) != 0;
}

bool AndroidMouseEvent::isMiddleButton() const
{
	return (button_ & AMOTION_EVENT_BUTTON_TERTIARY) != 0;
}

bool AndroidMouseEvent::isRightButton() const
{
	return (button_ & AMOTION_EVENT_BUTTON_SECONDARY) != 0;
}

bool AndroidMouseEvent::isFourthButton() const
{
	return (button_ & AMOTION_EVENT_BUTTON_BACK) != 0;
}

bool AndroidMouseEvent::isFifthButton() const
{
	return (button_ & AMOTION_EVENT_BUTTON_FORWARD) != 0;
}

bool AndroidJoystickState::isButtonPressed(int buttonId) const
{
	bool isPressed = false;
	if (buttonId >= 0 && buttonId < numButtons_)
		isPressed = buttons_[buttonId];
	return isPressed;
}

unsigned char AndroidJoystickState::hatState(int hatId) const
{
	unsigned char hatState = HatState::CENTERED;
	if (hatId >= 0 && hatId < numHats_)
		hatState = hatState_;
	return hatState;
}

short int AndroidJoystickState::axisValue(int axisId) const
{
	// If the joystick is not present the returned value is zero
	const short int axisValue = static_cast<short int>(axisNormValue(axisId) * IInputManager::MaxAxisValue);

	return axisValue;
}

float AndroidJoystickState::axisNormValue(int axisId) const
{
	float axisValue = 0.0f;
	if (axisId >= 0 && axisId < numAxes_)
		axisValue = axesValues_[axisId];
	return axisValue;
}

/*! This method is called by `enableAccelerometer()` and when the application gains focus */
void AndroidInputManager::enableAccelerometerSensor()
{
	if (accelerometerEnabled_ && accelerometerSensor_ != nullptr)
	{
		ASensorEventQueue_enableSensor(sensorEventQueue_, accelerometerSensor_);
		// 60 events per second
		ASensorEventQueue_setEventRate(sensorEventQueue_, accelerometerSensor_, (1000L / 60) * 1000);
	}
}

/*! This method is called by `enableAccelerometer()` and when the application loses focus */
void AndroidInputManager::disableAccelerometerSensor()
{
	if (accelerometerEnabled_ && accelerometerSensor_ != nullptr)
		ASensorEventQueue_disableSensor(sensorEventQueue_, accelerometerSensor_);
}

/*! Activates the sensor and raises the flag needed for application focus handling */
void AndroidInputManager::enableAccelerometer(bool enabled)
{
	if (enabled)
		enableAccelerometerSensor();
	else
		disableAccelerometerSensor();

	accelerometerEnabled_ = enabled;
}

void AndroidInputManager::parseAccelerometerEvent()
{
	if (inputEventHandler_ != nullptr && accelerometerEnabled_ && accelerometerSensor_ != nullptr)
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

bool AndroidInputManager::parseEvent(const AInputEvent *event)
{
	// Early out if there is no input event handler
	if (inputEventHandler_ == nullptr)
		return false;

	bool isEventHandled = false;

#ifdef WITH_IMGUI
	isEventHandled |= ImGuiAndroidInput::processEvent(event);
#endif

#ifdef WITH_NUKLEAR
	isEventHandled |= NuklearAndroidInput::processEvent(event);
#endif

	// Checking for gamepad events first
	if (((AInputEvent_getSource(event) & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD ||
	     (AInputEvent_getSource(event) & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK ||
	     (AInputEvent_getSource(event) & AINPUT_SOURCE_DPAD) == AINPUT_SOURCE_DPAD))
	{
		isEventHandled = processGamepadEvent(event);
	}
	else if (((AInputEvent_getSource(event) & AINPUT_SOURCE_KEYBOARD) == AINPUT_SOURCE_KEYBOARD ||
	          (AKeyEvent_getFlags(event) & AKEY_EVENT_FLAG_SOFT_KEYBOARD) == AKEY_EVENT_FLAG_SOFT_KEYBOARD) &&
	         AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
	{
		isEventHandled = processKeyboardEvent(event);
	}
	else if ((AInputEvent_getSource(event) & AINPUT_SOURCE_TOUCHSCREEN) == AINPUT_SOURCE_TOUCHSCREEN &&
	         AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		isEventHandled = processTouchEvent(event);
	}
	else if ((AInputEvent_getSource(event) & AINPUT_SOURCE_MOUSE) == AINPUT_SOURCE_MOUSE &&
	         AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		isEventHandled = processMouseEvent(event);
	}
	else if ((AInputEvent_getSource(event) & AINPUT_SOURCE_MOUSE) == AINPUT_SOURCE_MOUSE &&
	         AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
	{
		isEventHandled = processMouseKeyEvent(event);
	}

	return isEventHandled;
}

bool AndroidInputManager::isJoyPresent(int joyId) const
{
	ASSERT(joyId >= 0);
	ASSERT_MSG_X(joyId < int(MaxNumJoysticks), "joyId is %d and the maximum is %u", joyId, MaxNumJoysticks - 1);

	const int deviceId = joystickStates_[joyId].deviceId_;
	return (deviceId != -1);
}

const char *AndroidInputManager::joyName(int joyId) const
{
	if (isJoyPresent(joyId))
		return joystickStates_[joyId].name_;
	else
		return nullptr;
}

const char *AndroidInputManager::joyGuid(int joyId) const
{
	if (isJoyPresent(joyId))
		return joystickStates_[joyId].guid_;
	else
		return nullptr;
}

int AndroidInputManager::joyNumButtons(int joyId) const
{
	int numButtons = -1;

	if (isJoyPresent(joyId))
		numButtons = joystickStates_[joyId].numButtons_;

	return numButtons;
}

int AndroidInputManager::joyNumHats(int joyId) const
{
	int numHats = -1;

	if (isJoyPresent(joyId))
		numHats = joystickStates_[joyId].numHats_;

	return numHats;
}

int AndroidInputManager::joyNumAxes(int joyId) const
{
	int numAxes = -1;

	if (isJoyPresent(joyId))
		numAxes = joystickStates_[joyId].numAxes_;

	return numAxes;
}

const JoystickState &AndroidInputManager::joystickState(int joyId) const
{
	if (isJoyPresent(joyId))
		return joystickStates_[joyId];
	else
		return nullJoystickState_;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool AndroidInputManager::processGamepadEvent(const AInputEvent *event)
{
	const int deviceId = AInputEvent_getDeviceId(event);
	const int joyId = findJoyId(deviceId);

	// If the index is valid then the structure can be updated
	if (joyId > -1)
	{
		joystickStates_[joyId].deviceId_ = deviceId;

		if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
		{
			const int keyCode = AKeyEvent_getKeyCode(event);
			int buttonIndex = -1;
			if (keyCode >= AKEYCODE_BUTTON_A && keyCode < AKEYCODE_ESCAPE)
				buttonIndex = joystickStates_[joyId].buttonsMapping_[keyCode - AKEYCODE_BUTTON_A];
			else if (keyCode == AKEYCODE_BACK)
			{
				// Back button is always the last one
				const unsigned int lastIndex = AndroidJoystickState::MaxButtons - 1;
				buttonIndex = joystickStates_[joyId].buttonsMapping_[lastIndex];
			}

			if (buttonIndex > -1)
			{
				joyButtonEvent_.joyId = joyId;
				joyButtonEvent_.buttonId = buttonIndex;
				switch (AKeyEvent_getAction(event))
				{
					case AKEY_EVENT_ACTION_DOWN:
						joystickStates_[joyId].buttons_[buttonIndex] = true;
						joyMapping_.onJoyButtonPressed(joyButtonEvent_);
						inputEventHandler_->onJoyButtonPressed(joyButtonEvent_);
						break;
					case AKEY_EVENT_ACTION_UP:
						joystickStates_[joyId].buttons_[buttonIndex] = false;
						joyMapping_.onJoyButtonReleased(joyButtonEvent_);
						inputEventHandler_->onJoyButtonReleased(joyButtonEvent_);
						break;
					case AKEY_EVENT_ACTION_MULTIPLE:
						break;
				}
			}

			if (keyCode >= AKEYCODE_DPAD_UP && keyCode < AKEYCODE_DPAD_CENTER)
			{
				joyHatEvent_.joyId = joyId;
				joyHatEvent_.hatId = 0; // No more than one hat is supported

				unsigned char hatState = joystickStates_[joyId].hatState_;
				unsigned char hatValue = 0;

				switch (keyCode)
				{
					case AKEYCODE_DPAD_UP:
						hatValue = HatState::UP;
						break;
					case AKEYCODE_DPAD_DOWN:
						hatValue = HatState::DOWN;
						break;
					case AKEYCODE_DPAD_LEFT:
						hatValue = HatState::LEFT;
						break;
					case AKEYCODE_DPAD_RIGHT:
						hatValue = HatState::RIGHT;
						break;
				}
				if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN)
					hatState |= hatValue;
				else
					hatState &= ~hatValue;

				if (joystickStates_[joyId].hatState_ != hatState)
				{
					joystickStates_[joyId].hatState_ = hatState;
					joyHatEvent_.hatState = joystickStates_[joyId].hatState_;

					joyMapping_.onJoyHatMoved(joyHatEvent_);
					inputEventHandler_->onJoyHatMoved(joyHatEvent_);
				}
			}
		}
		else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
		{
			joyAxisEvent_.joyId = joyId;

			unsigned char hatState = 0;
			for (int i = 0; i < joystickStates_[joyId].numAxes_; i++)
			{
				const int axis = joystickStates_[joyId].axesMapping_[i];
				const float axisValue = AMotionEvent_getAxisValue(event, axis, 0);
				joystickStates_[joyId].axesValues_[i] = axisValue;

				const float AxisThresholdValue = 0.01f;
				const float HatThresholdValue = 0.99f;

				if (axis == AMOTION_EVENT_AXIS_HAT_X || axis == AMOTION_EVENT_AXIS_HAT_Y)
				{
					joyHatEvent_.joyId = joyId;
					joyHatEvent_.hatId = 0; // No more than one hat is supported

					if (axis == AMOTION_EVENT_AXIS_HAT_X)
					{
						if (axisValue > HatThresholdValue)
							hatState |= HatState::RIGHT;
						else if (axisValue < -HatThresholdValue)
							hatState |= HatState::LEFT;
					}
					else
					{
						if (axisValue > HatThresholdValue)
							hatState |= HatState::DOWN;
						else if (axisValue < -HatThresholdValue)
							hatState |= HatState::UP;
					}
				}
				else if (axisValue > AxisThresholdValue || axisValue < -AxisThresholdValue)
				{
					joyAxisEvent_.axisId = i;
					joyAxisEvent_.value = axisValue * MaxAxisValue;
					joyAxisEvent_.normValue = axisValue;
					joyMapping_.onJoyAxisMoved(joyAxisEvent_);
					inputEventHandler_->onJoyAxisMoved(joyAxisEvent_);
				}
			}

			if (joystickStates_[joyId].hatState_ != hatState)
			{
				joystickStates_[joyId].hatState_ = hatState;
				joyHatEvent_.hatState = joystickStates_[joyId].hatState_;

				joyMapping_.onJoyHatMoved(joyHatEvent_);
				inputEventHandler_->onJoyHatMoved(joyHatEvent_);
			}
		}
	}
	else
		LOGW_X("No available joystick id for device %d, dropping button event", deviceId);

	return true;
}

bool AndroidInputManager::processKeyboardEvent(const AInputEvent *event)
{
	const int keyCode = AKeyEvent_getKeyCode(event);

	// Hardware volume keys are not handled by the engine
	if (keyCode == AKEYCODE_VOLUME_UP || keyCode == AKEYCODE_VOLUME_DOWN)
		return false;

	keyboardEvent_.scancode = AKeyEvent_getScanCode(event);
	keyboardEvent_.sym = AndroidKeys::keySymValueToEnum(AKeyEvent_getKeyCode(event));
	keyboardEvent_.mod = AndroidKeys::keyModMaskToEnumMask(AKeyEvent_getMetaState(event));

	const unsigned int keySym = static_cast<unsigned int>(keyboardEvent_.sym);
	switch (AKeyEvent_getAction(event))
	{
		case AKEY_EVENT_ACTION_DOWN:
			if (keyboardEvent_.sym != KeySym::UNKNOWN)
				keyboardState_.keys_[keySym] = 1;
			inputEventHandler_->onKeyPressed(keyboardEvent_);
			break;
		case AKEY_EVENT_ACTION_UP:
			if (keyboardEvent_.sym != KeySym::UNKNOWN)
				keyboardState_.keys_[keySym] = 0;
			inputEventHandler_->onKeyReleased(keyboardEvent_);
			break;
		case AKEY_EVENT_ACTION_MULTIPLE:
			inputEventHandler_->onKeyPressed(keyboardEvent_);
			break;
	}

	if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN &&
	    (AKeyEvent_getMetaState(event) & AMETA_CTRL_ON) == 0)
	{
		AndroidJniClass_KeyEvent keyEvent(AInputEvent_getType(event), keyCode);
		if (keyEvent.isPrintingKey())
		{
			const int unicodeKey = keyEvent.getUnicodeChar(AKeyEvent_getMetaState(event));
			nctl::Utf8::codePointToUtf8(unicodeKey, textInputEvent_.text, nullptr);
			inputEventHandler_->onTextInput(textInputEvent_);
		}
	}

	return true;
}

bool AndroidInputManager::processTouchEvent(const AInputEvent *event)
{
	const int action = AMotionEvent_getAction(event);

	touchEvent_.count = AMotionEvent_getPointerCount(event);
	touchEvent_.actionIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
	for (unsigned int i = 0; i < touchEvent_.count && i < TouchEvent::MaxPointers; i++)
	{
		TouchEvent::Pointer &pointer = touchEvent_.pointers[i];
		pointer.id = AMotionEvent_getPointerId(event, i);
		pointer.x = AMotionEvent_getX(event, i);
		pointer.y = theApplication().height() - AMotionEvent_getY(event, i);
		pointer.pressure = AMotionEvent_getPressure(event, i);
	}

	switch (action & AMOTION_EVENT_ACTION_MASK)
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
			inputEventHandler_->onPointerDown(touchEvent_);
			break;
		case AMOTION_EVENT_ACTION_POINTER_UP:
			inputEventHandler_->onPointerUp(touchEvent_);
			break;
	}

	return true;
}

bool AndroidInputManager::processMouseEvent(const AInputEvent *event)
{
	const int action = AMotionEvent_getAction(event);
	int buttonState = 0;

	mouseEvent_.x = static_cast<int>(AMotionEvent_getX(event, 0));
	mouseEvent_.y = static_cast<int>(theApplication().height() - AMotionEvent_getY(event, 0));
	mouseState_.x = mouseEvent_.x;
	mouseState_.y = mouseEvent_.y;

	// Mask out back and forward buttons in the detected state
	// as those are simulated as right and middle buttons
	int maskOutButtons = 0;
	if (simulatedMouseButtonState_ & AMOTION_EVENT_BUTTON_SECONDARY)
		maskOutButtons |= AMOTION_EVENT_BUTTON_BACK;
	if (simulatedMouseButtonState_ & AMOTION_EVENT_BUTTON_TERTIARY)
		maskOutButtons |= AMOTION_EVENT_BUTTON_FORWARD;

	switch (action)
	{
		case AMOTION_EVENT_ACTION_DOWN:
			buttonState = AMotionEvent_getButtonState(event);
			buttonState &= ~maskOutButtons;
			buttonState |= simulatedMouseButtonState_;

			mouseEvent_.button_ = mouseState_.buttonState_ ^ buttonState; // pressed button mask
			mouseState_.buttonState_ = buttonState;
			inputEventHandler_->onMouseButtonPressed(mouseEvent_);
			break;
		case AMOTION_EVENT_ACTION_UP:
			buttonState = AMotionEvent_getButtonState(event);
			buttonState &= ~maskOutButtons;
			buttonState |= simulatedMouseButtonState_;

			mouseEvent_.button_ = mouseState_.buttonState_ ^ buttonState; // released button mask
			mouseState_.buttonState_ = buttonState;
			inputEventHandler_->onMouseButtonReleased(mouseEvent_);
			break;
		case AMOTION_EVENT_ACTION_MOVE:
		case AMOTION_EVENT_ACTION_HOVER_MOVE:
			inputEventHandler_->onMouseMoved(mouseState_);
			break;
	}

	scrollEvent_.x = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HSCROLL, 0);
	scrollEvent_.y = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_VSCROLL, 0);
	if (fabsf(scrollEvent_.x) > 0.0f || fabsf(scrollEvent_.y) > 0.0f)
		inputEventHandler_->onScrollInput(scrollEvent_);

	return true;
}

bool AndroidInputManager::processMouseKeyEvent(const AInputEvent *event)
{
	const int keyCode = AKeyEvent_getKeyCode(event);
	if (keyCode == AKEYCODE_BACK || keyCode == AKEYCODE_FORWARD)
	{
		const int simulatedButton = (keyCode == AKEYCODE_BACK) ? AMOTION_EVENT_BUTTON_SECONDARY : AMOTION_EVENT_BUTTON_TERTIARY;
		static int oldAction = AKEY_EVENT_ACTION_UP;
		const int action = AKeyEvent_getAction(event);

		// checking previous action to avoid key repeat events
		if (action == AKEY_EVENT_ACTION_DOWN && oldAction == AKEY_EVENT_ACTION_UP)
		{
			oldAction = action;
			simulatedMouseButtonState_ |= simulatedButton;
			mouseEvent_.button_ = simulatedButton;
			mouseState_.buttonState_ |= simulatedButton;
			inputEventHandler_->onMouseButtonPressed(mouseEvent_);
		}
		else if (action == AKEY_EVENT_ACTION_UP && oldAction == AKEY_EVENT_ACTION_DOWN)
		{
			oldAction = action;
			simulatedMouseButtonState_ &= ~simulatedButton;
			mouseEvent_.button_ = simulatedButton;
			mouseState_.buttonState_ &= ~simulatedButton;
			inputEventHandler_->onMouseButtonReleased(mouseEvent_);
		}
	}

	return true;
}

void AndroidInputManager::initAccelerometerSensor(android_app *state)
{
	// Prepare to monitor accelerometer
#if __ANDROID_API__ >= 26
	nc::AndroidApplication &application = static_cast<nc::AndroidApplication &>(nc::theApplication());
	sensorManager_ = ASensorManager_getInstanceForPackage(application.packageName().data());
#else
	sensorManager_ = ASensorManager_getInstance();
#endif
	accelerometerSensor_ = ASensorManager_getDefaultSensor(sensorManager_, ASENSOR_TYPE_ACCELEROMETER);
	sensorEventQueue_ = ASensorManager_createEventQueue(sensorManager_, state->looper, LOOPER_ID_USER, nullptr, nullptr);

	if (accelerometerSensor_ == nullptr)
		LOGW("No accelerometer sensor available");
}

void AndroidInputManager::updateJoystickConnections()
{
	if (joyCheckTimer_.interval() >= JoyCheckRate)
	{
		checkDisconnectedJoysticks();
		checkConnectedJoysticks();

		joyCheckTimer_.start();
	}
}

void AndroidInputManager::checkDisconnectedJoysticks()
{
	for (unsigned int i = 0; i < MaxNumJoysticks; i++)
	{
		const int deviceId = joystickStates_[i].deviceId_;
		if (deviceId > -1 && isDeviceConnected(deviceId) == false)
		{
			LOGI_X("Joystick %d (device %d) \"%s\" has been disconnected", i, deviceId, joystickStates_[i].name_);
			joystickStates_[i].deviceId_ = -1;

			if (inputEventHandler_ != nullptr)
			{
				joyConnectionEvent_.joyId = i;
				inputEventHandler_->onJoyDisconnected(joyConnectionEvent_);
				joyMapping_.onJoyDisconnected(joyConnectionEvent_);
			}
		}
	}
}

void AndroidInputManager::checkConnectedJoysticks()
{
	// InputDevice.getDeviceIds() will not fill an array longer than MaxDevices
	const int MaxDevices = MaxNumJoysticks * 2;
	int deviceIds[MaxDevices];

	int connectedJoys = 0;
	for (unsigned int i = 0; i < MaxNumJoysticks; i++)
	{
		if (joystickStates_[i].deviceId_ > -1)
			connectedJoys++;
	}

	const int connectedDevices = AndroidJniClass_InputDevice::getDeviceIds(deviceIds, MaxDevices);
	for (int i = 0; i < MaxDevices && i < connectedDevices; i++)
	{
		AndroidJniClass_InputDevice inputDevice = AndroidJniClass_InputDevice::getDevice(deviceIds[i]);
		const int sources = inputDevice.getSources();

		if (((sources & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD) ||
		    ((sources & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK))
		{
			const int joyId = findJoyId(deviceIds[i]);
			if (joyId > -1)
				joystickStates_[joyId].deviceId_ = deviceIds[i];

			connectedJoys++;
			if (connectedJoys >= int(MaxNumJoysticks))
				break;
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
			joyId = i;
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
		LOGI_X("Device %d, \"%s\" (%s), has been connected as joystick %d - %d axes, %d buttons",
		       deviceId, joystickStates_[joyId].name_, joystickStates_[joyId].guid_, joyId, joystickStates_[joyId].numAxes_, joystickStates_[joyId].numButtons_);
		joystickStates_[joyId].deviceId_ = deviceId;

		if (inputEventHandler_ != nullptr)
		{
			joyConnectionEvent_.joyId = joyId;
			joyMapping_.onJoyConnected(joyConnectionEvent_);
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
		isConnected = true;

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

		memset(joystickStates_[joyId].guid_, 0, 33);
		const int vendorId = inputDevice.getVendorId();
		const int productId = inputDevice.getProductId();
		if (vendorId > 0 && productId > 0)
		{
			// GUID calculated concatenating the vendorId and the productId
			snprintf(joystickStates_[joyId].guid_, 33, "%04x%04x000000000000000000000000", vendorId, productId);
		}
		else
		{
			const char *joyName = joystickStates_[joyId].name_;
			const unsigned int charsToCopy = strnlen(joyName, 16);

			// Currently used by SDL2 to calculate the GUID
			for (unsigned int i = 0; i < charsToCopy; i++)
			{
				// GUID calculated using the first 16 characters of the device name
				snprintf(&joystickStates_[joyId].guid_[i * 2], 32 - i * 2, "%02x", joyName[i]);
			}
		}
		joystickStates_[joyId].guid_[32] = '\0';

		// Checking all AKEYCODE_BUTTON_* plus AKEYCODE_BACK
		const int maxButtons = AndroidJoystickState::MaxButtons;
		bool checkedButtons[maxButtons];
		int numFoundButtons = 0;
		if (AndroidJniHelper::sdkVersion() >= 19 && __ANDROID_API__ >= 19)
		{
			int buttonsToCheck[maxButtons];
			for (int i = 0; i < maxButtons - 1; i++)
				buttonsToCheck[i] = AKEYCODE_BUTTON_A + i;
			// Back button is always the last one
			buttonsToCheck[maxButtons - 1] = AKEYCODE_BACK;

			// InputDevice.hasKeys()
			inputDevice.hasKeys(buttonsToCheck, maxButtons, checkedButtons);
		}

		memset(deviceInfoString, 0, MaxStringLength);
		for (int i = 0; i < maxButtons; i++)
		{
			bool hasKey = false;
			const int keyCode = (i < maxButtons - 1) ? AKEYCODE_BUTTON_A + i : AKEYCODE_BACK;

			if (AndroidJniHelper::sdkVersion() >= 19 && __ANDROID_API__ >= 19)
				hasKey = checkedButtons[i];
			else // KeyCharacterMap.deviceHasKey()
				hasKey = AndroidJniClass_KeyCharacterMap::deviceHasKey(keyCode);

			if (hasKey)
			{
				joystickStates_[joyId].buttonsMapping_[i] = numFoundButtons;
				sprintf(&deviceInfoString[strlen(deviceInfoString)], "%d:%d ", numFoundButtons, keyCode);
				numFoundButtons++;
			}
			else
				joystickStates_[joyId].buttonsMapping_[i] = -1;

			if (numFoundButtons >= AndroidJoystickState::MaxButtons)
				break;
		}
		joystickStates_[joyId].numButtons_ = numFoundButtons;
		LOGV_X("device (%d, %d) - Buttons %s", deviceId, joyId, deviceInfoString);

		joystickStates_[joyId].hasDPad_ = true;
		for (int button = AKEYCODE_DPAD_UP; button < AKEYCODE_DPAD_CENTER; button++)
		{
			const bool hasKey = AndroidJniClass_KeyCharacterMap::deviceHasKey(button);
			if (hasKey == false)
			{
				joystickStates_[joyId].hasDPad_ = false;
				LOGV_X("device (%d, %d) - D-Pad not detected", deviceId, joyId);
				break;
			}
		}

		memset(deviceInfoString, 0, MaxStringLength);
		joystickStates_[joyId].hasHatAxes_ = true;
		// InputDevice.getMotionRange()
		int numAxes = 0;
		for (int i = 0; i < AndroidJoystickState::NumAxesToMap; i++)
		{
			const int axis = AndroidJoystickState::AxesToMap[i];
			AndroidJniClass_MotionRange motionRange = inputDevice.getMotionRange(axis);

			if (motionRange.isNull() == false)
			{
				joystickStates_[joyId].axesMapping_[numAxes] = axis;
				sprintf(&deviceInfoString[strlen(deviceInfoString)], "%d:%d ", numAxes, axis);
				numAxes++;
			}
			else
			{
				if ((axis == AMOTION_EVENT_AXIS_HAT_X || axis == AMOTION_EVENT_AXIS_HAT_Y) &&
				    joystickStates_[joyId].hasHatAxes_ == true)
				{
					joystickStates_[joyId].hasHatAxes_ = false;
					LOGV_X("device (%d, %d) - Axis hats not detected", deviceId, joyId);
				}
			}
		}
		LOGV_X("device (%d, %d) - Axes %s", deviceId, joyId, deviceInfoString);
		joystickStates_[joyId].numAxes_ = numAxes;

		joystickStates_[joyId].numHats_ = 0;
		if (joystickStates_[joyId].hasDPad_ || joystickStates_[joyId].hasHatAxes_)
			joystickStates_[joyId].numHats_ = 1; // No more than one hat is supported
	}
}

}
