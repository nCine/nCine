#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/sensor.h>
#include <nctl/StaticString.h>

#include "AndroidInputManager.h"
#include "IInputEventHandler.h"
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
const unsigned short int IInputManager::MaxVibrationValue = 255;

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
MouseEvent AndroidInputManager::mouseEvent_;
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
	AMOTION_EVENT_AXIS_X, AMOTION_EVENT_AXIS_Y, AMOTION_EVENT_AXIS_Z,
	AMOTION_EVENT_AXIS_RX, AMOTION_EVENT_AXIS_RY, AMOTION_EVENT_AXIS_RZ,
	AMOTION_EVENT_AXIS_LTRIGGER, AMOTION_EVENT_AXIS_RTRIGGER,
	AMOTION_EVENT_AXIS_BRAKE, AMOTION_EVENT_AXIS_GAS,
	AMOTION_EVENT_AXIS_HAT_X, AMOTION_EVENT_AXIS_HAT_Y
};

namespace {

	MouseButton androidToNcineMouseButton(int button)
	{
		if (button == AMOTION_EVENT_BUTTON_PRIMARY)
			return MouseButton::LEFT;
		else if (button == AMOTION_EVENT_BUTTON_TERTIARY)
			return MouseButton::MIDDLE;
		else if (button == AMOTION_EVENT_BUTTON_SECONDARY)
			return MouseButton::RIGHT;
		else if (button == AMOTION_EVENT_BUTTON_BACK)
			return MouseButton::FOURTH;
		else if (button == AMOTION_EVENT_BUTTON_FORWARD)
			return MouseButton::FIFTH;
		else
			return MouseButton::LEFT;
	}

	bool checkMouseButton(int buttonState, MouseButton button)
	{
		switch (button)
		{
			case MouseButton::LEFT: return ((buttonState & AMOTION_EVENT_BUTTON_PRIMARY) != 0);
			case MouseButton::MIDDLE: return ((buttonState & AMOTION_EVENT_BUTTON_TERTIARY) != 0);
			case MouseButton::RIGHT: return ((buttonState & AMOTION_EVENT_BUTTON_SECONDARY) != 0);
			case MouseButton::FOURTH: return ((buttonState & AMOTION_EVENT_BUTTON_BACK) != 0);
			case MouseButton::FIFTH: return ((buttonState & AMOTION_EVENT_BUTTON_FORWARD) != 0);
			default: return false;
		}
	}

}

///////////////////////////////////////////////////////////
// AndroidMouseState
///////////////////////////////////////////////////////////

AndroidMouseState::AndroidMouseState()
    : currentStateIndex_(0), buttonStates_{0, 0}
{
}

bool AndroidMouseState::isButtonDown(MouseButton button) const
{
	return checkMouseButton(buttonStates_[currentStateIndex_], button);
}

bool AndroidMouseState::isButtonPressed(MouseButton button) const
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	return (checkMouseButton(buttonStates_[currentStateIndex_], button) == true &&
	        checkMouseButton(buttonStates_[prevStateIndex], button) == false);
}

bool AndroidMouseState::isButtonReleased(MouseButton button) const
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	return (checkMouseButton(buttonStates_[currentStateIndex_], button) == false &&
	        checkMouseButton(buttonStates_[prevStateIndex], button) == true);
}

void AndroidMouseState::copyButtonStateToPrev()
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	buttonStates_[prevStateIndex] = buttonStates_[currentStateIndex_];
	currentStateIndex_ = prevStateIndex;
}

///////////////////////////////////////////////////////////
// AndroidKeyboardState
///////////////////////////////////////////////////////////

AndroidKeyboardState::AndroidKeyboardState()
    : currentStateIndex_(0)
{
	memset(keys_[0], 0, NumKeys * sizeof(unsigned char));
	memset(keys_[1], 0, NumKeys * sizeof(unsigned char));
}

bool AndroidKeyboardState::isKeyDown(KeySym key) const
{
	const unsigned int keyIndex = static_cast<unsigned int>(key);
	if (key == KeySym::UNKNOWN)
		return false;
	else
		return keys_[currentStateIndex_][keyIndex] != 0;
}

bool AndroidKeyboardState::isKeyPressed(KeySym key) const
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	const unsigned int keyIndex = static_cast<unsigned int>(key);
	if (key == KeySym::UNKNOWN)
		return false;
	else
		return keys_[currentStateIndex_][keyIndex] != 0 && keys_[prevStateIndex][keyIndex] == 0;
}

bool AndroidKeyboardState::isKeyReleased(KeySym key) const
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	const unsigned int keyIndex = static_cast<unsigned int>(key);
	if (key == KeySym::UNKNOWN)
		return false;
	else
		return keys_[currentStateIndex_][keyIndex] == 0 && keys_[prevStateIndex][keyIndex] != 0;
}

void AndroidKeyboardState::copyKeyStateToPrev()
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	memcpy(keys_[prevStateIndex], keys_[currentStateIndex_], NumKeys * sizeof(unsigned char));
	currentStateIndex_ = prevStateIndex;
}

///////////////////////////////////////////////////////////
// AndroidJoystickState
///////////////////////////////////////////////////////////

AndroidJoystickState::AndroidJoystickState()
    : deviceId_(-1), numButtons_(0), numAxes_(0), numMappedAxes_(0),
      hasDPad_(false), hasHatAxes_(false), currentStateIndex_(0),
      hatState_(HatState::CENTERED), numVibrators_(0)
{
	guid_[0] = '\0';
	name_[0] = '\0';
	for (int i = 0; i < MaxButtons; i++)
		buttonsMapping_[i] = 0;

	memset(buttonStates_[0], 0, MaxButtons * sizeof(unsigned char));
	memset(buttonStates_[1], 0, MaxButtons * sizeof(unsigned char));

	for (int i = 0; i < MaxAxes; i++)
	{
		axesMapping_[i] = 0;
		axesMinValues_[i] = -1.0f;
		axesRangeValues_[i] = 2.0f;
		axesValues_[i] = 0.0f;
	}

	for (int i = 0; i < MaxVibrators; i++)
		vibratorsIds_[i] = 0;
}

bool AndroidJoystickState::isButtonDown(int buttonId) const
{
	ASSERT(buttonId < MaxButtons);
	bool isDown = false;
	if (buttonId >= 0 && buttonId < numButtons_)
		isDown = buttonStates_[currentStateIndex_][buttonId];
	return isDown;
}

bool AndroidJoystickState::isButtonPressed(int buttonId) const
{
	ASSERT(buttonId < MaxButtons);
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	bool isPressed = false;
	if (buttonId >= 0 && buttonId < numButtons_)
		isPressed = (buttonStates_[currentStateIndex_][buttonId] != 0 && buttonStates_[prevStateIndex][buttonId] == 0);
	return isPressed;
}

bool AndroidJoystickState::isButtonReleased(int buttonId) const
{
	ASSERT(buttonId < MaxButtons);
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	bool isReleased = false;
	if (buttonId >= 0 && buttonId < numButtons_)
		isReleased = (buttonStates_[currentStateIndex_][buttonId] == 0 && buttonStates_[prevStateIndex][buttonId] != 0);
	return isReleased;
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
	short int axisValue = 0;
	if (axisId >= 0 && axisId < numMappedAxes_)
		axisValue = static_cast<short int>(axesValues_[axisId] * IInputManager::MaxAxisValue);

	return axisValue;
}

float AndroidJoystickState::axisNormValue(int axisId) const
{
	float axisValue = 0.0f;
	if (axisId >= 0 && axisId < numMappedAxes_)
	{
		// The value has already been remapped from min..max to -1.0f..1.0f
		axisValue = axesValues_[axisId];
	}
	return axisValue;
}

void AndroidJoystickState::createGuid(uint16_t bus, uint16_t vendor, uint16_t product, uint16_t version,
                                      const char *name, uint8_t driverSignature, uint8_t driverData)
{
	uint16_t *guid16 = reinterpret_cast<uint16_t *>(guid_);
	const unsigned int nameLength = name ? nctl::strnlen(name, MaxNameLength) : 0;

	memset(guid_, 0, GuidLength);

	*guid16++ = bus;
	// CRC16 field is missing, but it is never used for GUID comparison
	*guid16++ = 0;

	if (vendor && product)
	{
		*guid16++ = vendor;
		*guid16++ = 0;
		*guid16++ = product;
		*guid16++ = 0;
		*guid16++ = version;
		guid_[14] = driverSignature;
		guid_[15] = driverData;
	}
	else
	{
		unsigned int availableSpace = 3 * sizeof(uint32_t);

		if (driverSignature)
		{
			availableSpace -= 2;
			guid_[14] = driverSignature;
			guid_[15] = driverData;
		}

		if (nameLength > 0)
			memcpy(guid16, name, nctl::min(nameLength, availableSpace));
	}
}

void AndroidJoystickState::updateGuidWithCapabilities()
{
	uint16_t axisMask = 0;
	if (numAxes_ >= 2)
		axisMask |= 0x01 | 0x02;
	if (numAxes_ >= 4)
		axisMask |= 0x04 | 0x08;
	if (numAxes_ >= 6)
		axisMask |= 0x10 | 0x20;

	// From NDK's android/keycodes.h: A, B, C, X, Y, Z, L1, R1, L2, R2, THUMBL, THUMBR, START, SELECT, MODE
	static const int ButtonMasks[15] = {
		(1 << 0), (1 << 1), (1 << 17), (1 << 2), (1 << 3), (1 << 18), (1 << 9), (1 << 10),
		(1 << 15), (1 << 16), (1 << 7), (1 << 8), (1 << 6), (1 << 4), (1 << 5)
	};

	uint16_t buttonMask = 0;
	for (unsigned int i = 0; i < MaxButtons; i++)
	{
		if (buttonsMapping_[i] != -1)
			buttonMask |= ButtonMasks[i];
	}

	if (hasDPad_ || hasHatAxes_)
		buttonMask |= 0x800 | 0x1000 | 0x2000 | 0x4000;

	guid_[12] = buttonMask & 0xff;
	guid_[13] = (buttonMask >> 8) & 0xff;
	guid_[14] = axisMask & 0xff;
	guid_[15] = (axisMask >> 8) & 0xff;
}

void AndroidJoystickState::copyButtonStateToPrev()
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	memcpy(buttonStates_[prevStateIndex], buttonStates_[currentStateIndex_], MaxButtons * sizeof(unsigned char));
	currentStateIndex_ = prevStateIndex;
}

void AndroidJoystickState::resetPrevButtonState()
{
	const unsigned int prevStateIndex = (currentStateIndex_ == 0 ? 1 : 0);
	memset(buttonStates_[prevStateIndex], 0, MaxButtons * sizeof(unsigned char));
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

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
	if ((AInputEvent_getSource(event) & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD ||
	    (AInputEvent_getSource(event) & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK ||
	    (AInputEvent_getSource(event) & AINPUT_SOURCE_DPAD) == AINPUT_SOURCE_DPAD)
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
	/// Used to return a printable string
	static nctl::StaticString<33> joyGuidString;

	if (isJoyPresent(joyId))
	{
		joyGuidString.clear();
		for (unsigned int i = 0; i < AndroidJoystickState::GuidLength; i++)
			joyGuidString.formatAppend("%02x", joystickStates_[joyId].guid_[i]);
		return joyGuidString.data();
	}
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
		numAxes = joystickStates_[joyId].numMappedAxes_;

	return numAxes;
}

const JoystickState &AndroidInputManager::joystickState(int joyId) const
{
	if (isJoyPresent(joyId))
		return joystickStates_[joyId];
	else
		return nullJoystickState_;
}

bool AndroidInputManager::hasJoyVibration(int joyId) const
{
	bool hasVibration = false;

	if (isJoyPresent(joyId))
		hasVibration = (joystickStates_[joyId].numVibrators_ > 0);

	return hasVibration;
}

void AndroidInputManager::joyVibrate(int joyId, float lowFreqIntensity, float highFreqIntensity, unsigned int duration) const
{
	if (isJoyPresent(joyId))
	{
		if (joystickStates_[joyId].numVibrators_ > 0)
		{
			// Clamp intensity between 0.0f and 1.0f
			lowFreqIntensity = (lowFreqIntensity < 0.0f) ? 0.0f : lowFreqIntensity;
			lowFreqIntensity = (lowFreqIntensity > 1.0f) ? 1.0f : lowFreqIntensity;
			const unsigned char amplitude = static_cast<unsigned char>(lowFreqIntensity * MaxVibrationValue);

			joystickStates_[joyId].vibrators_[0].cancel();
			// `amplitude` must either be `DEFAULT_AMPLITUDE`, or between 1 and 255 inclusive
			if (amplitude > 0)
			{
				const AndroidJniClass_VibrationEffect vibration = AndroidJniClass_VibrationEffect::createOneShot(duration, amplitude);
				joystickStates_[joyId].vibrators_[0].vibrate(vibration);
			}
		}
		if (joystickStates_[joyId].numVibrators_ > 1)
		{
			// Clamp intensity between 0.0f and 1.0f
			highFreqIntensity = (highFreqIntensity < 0.0f) ? 0.0f : highFreqIntensity;
			highFreqIntensity = (highFreqIntensity > 1.0f) ? 1.0f : highFreqIntensity;
			const unsigned char amplitude = static_cast<unsigned char>(highFreqIntensity * MaxVibrationValue);

			joystickStates_[joyId].vibrators_[1].cancel();
			// `amplitude` must either be `DEFAULT_AMPLITUDE`, or between 1 and 255 inclusive
			if (amplitude > 0)
			{
				const AndroidJniClass_VibrationEffect vibration = AndroidJniClass_VibrationEffect::createOneShot(duration, amplitude);
				joystickStates_[joyId].vibrators_[1].vibrate(vibration);
			}
		}
	}
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
		AndroidJoystickState &joystickState = joystickStates_[joyId];
		joystickState.deviceId_ = deviceId;

		if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
		{
			const int keyCode = AKeyEvent_getKeyCode(event);
			int buttonIndex = -1;
			if (keyCode >= AKEYCODE_BUTTON_A && keyCode < AKEYCODE_ESCAPE)
				buttonIndex = joystickState.buttonsMapping_[keyCode - AKEYCODE_BUTTON_A];
			else if (keyCode == AKEYCODE_BACK)
			{
				// Back button is always the last one
				const unsigned int lastIndex = AndroidJoystickState::MaxButtons - 1;
				buttonIndex = joystickState.buttonsMapping_[lastIndex];
			}

			if (buttonIndex > -1)
			{
				joyButtonEvent_.joyId = joyId;
				joyButtonEvent_.buttonId = buttonIndex;
				switch (AKeyEvent_getAction(event))
				{
					case AKEY_EVENT_ACTION_DOWN:
						joystickState.buttonStates_[joystickState.currentStateIndex_][buttonIndex] = true;
						joyMapping_.onJoyButtonPressed(joyButtonEvent_);
						inputEventHandler_->onJoyButtonPressed(joyButtonEvent_);
						break;
					case AKEY_EVENT_ACTION_UP:
						joystickState.buttonStates_[joystickState.currentStateIndex_][buttonIndex] = false;
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

				unsigned char hatState = joystickState.hatState_;
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

				if (joystickState.hatState_ != hatState)
				{
					joystickState.hatState_ = hatState;
					joyHatEvent_.hatState = joystickState.hatState_;

					joyMapping_.onJoyHatMoved(joyHatEvent_);
					inputEventHandler_->onJoyHatMoved(joyHatEvent_);
				}
			}
		}
		else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
		{
			joyAxisEvent_.joyId = joyId;

			unsigned char hatState = 0;
			for (int i = 0; i < joystickState.numAxes_; i++)
			{
				const int axis = joystickState.axesMapping_[i];
				const float axisRawValue = AMotionEvent_getAxisValue(event, axis, 0);
				// Remapping the axis range from min..max to -1.0f..1.0f
				const float normValue = -1.0f + 2.0f * (axisRawValue - joystickState.axesMinValues_[i]) / joystickState.axesRangeValues_[i];

				if (axis == AMOTION_EVENT_AXIS_HAT_X || axis == AMOTION_EVENT_AXIS_HAT_Y)
				{
					joyHatEvent_.joyId = joyId;
					joyHatEvent_.hatId = 0; // No more than one hat is supported

					const float HatThresholdValue = 0.99f;
					if (axis == AMOTION_EVENT_AXIS_HAT_X)
					{
						if (normValue > HatThresholdValue)
							hatState |= HatState::RIGHT;
						else if (normValue < -HatThresholdValue)
							hatState |= HatState::LEFT;
					}
					else
					{
						if (normValue > HatThresholdValue)
							hatState |= HatState::DOWN;
						else if (normValue < -HatThresholdValue)
							hatState |= HatState::UP;
					}
				}
				else
				{
					joystickState.axesValues_[i] = normValue;

					joyAxisEvent_.axisId = i;
					joyAxisEvent_.value = normValue * MaxAxisValue;
					joyAxisEvent_.normValue = normValue;
					joyMapping_.onJoyAxisMoved(joyAxisEvent_);
					inputEventHandler_->onJoyAxisMoved(joyAxisEvent_);
				}
			}

			if (joystickState.hatState_ != hatState)
			{
				joystickState.hatState_ = hatState;
				joyHatEvent_.hatState = joystickState.hatState_;

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
				keyboardState_.keys_[keyboardState_.currentStateIndex_][keySym] = 1;
			inputEventHandler_->onKeyPressed(keyboardEvent_);
			break;
		case AKEY_EVENT_ACTION_UP:
			if (keyboardEvent_.sym != KeySym::UNKNOWN)
				keyboardState_.keys_[keyboardState_.currentStateIndex_][keySym] = 0;
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
		{
			buttonState = AMotionEvent_getButtonState(event);
			buttonState &= ~maskOutButtons;
			buttonState |= simulatedMouseButtonState_;

			const int button = mouseState_.buttonStates_[mouseState_.currentStateIndex_] ^ buttonState; // pressed button mask
			mouseEvent_.button = androidToNcineMouseButton(button);
			mouseState_.buttonStates_[mouseState_.currentStateIndex_] = buttonState;
			inputEventHandler_->onMouseButtonPressed(mouseEvent_);
			break;
		}
		case AMOTION_EVENT_ACTION_UP:
		{
			buttonState = AMotionEvent_getButtonState(event);
			buttonState &= ~maskOutButtons;
			buttonState |= simulatedMouseButtonState_;

			const int button = mouseState_.buttonStates_[mouseState_.currentStateIndex_] ^ buttonState; // released button mask
			mouseEvent_.button = androidToNcineMouseButton(button);
			mouseState_.buttonStates_[mouseState_.currentStateIndex_] = buttonState;
			inputEventHandler_->onMouseButtonReleased(mouseEvent_);
			break;
		}
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
			mouseEvent_.button = androidToNcineMouseButton(simulatedButton);
			mouseState_.buttonStates_[mouseState_.currentStateIndex_] |= simulatedButton;
			inputEventHandler_->onMouseButtonPressed(mouseEvent_);
		}
		else if (action == AKEY_EVENT_ACTION_UP && oldAction == AKEY_EVENT_ACTION_DOWN)
		{
			oldAction = action;
			simulatedMouseButtonState_ &= ~simulatedButton;
			mouseEvent_.button = androidToNcineMouseButton(simulatedButton);
			mouseState_.buttonStates_[mouseState_.currentStateIndex_] &= ~simulatedButton;
			inputEventHandler_->onMouseButtonReleased(mouseEvent_);
		}
	}

	return true;
}

void AndroidInputManager::initAccelerometerSensor(android_app *state)
{
	// Prepare to monitor accelerometer
#if __ANDROID_API__ >= 26
	AndroidApplication &application = static_cast<AndroidApplication &>(theApplication());
	sensorManager_ = ASensorManager_getInstanceForPackage(application.packageName());
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
			joystickStates_[i].resetPrevButtonState();
			joystickStates_[i].deviceId_ = -1;

			LOGI_X("Joystick %d (device %d) \"%s\" has been disconnected", i, deviceId, joystickStates_[i].name_);
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
		joystickStates_[joyId].deviceId_ = deviceId;

		const uint8_t *g = reinterpret_cast<const uint8_t *>(joystickStates_[joyId].guid_);
		LOGI_X("Device %d, \"%s\" (GUID: \"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\"), has been connected as joystick %d - %d axes, %d buttons, %d vibrators",
		       deviceId, joystickStates_[joyId].name_, g[0], g[1], g[2], g[3], g[4], g[5], g[6], g[7], g[8], g[9], g[10], g[11], g[12], g[13], g[14], g[15],
		       joyId, joystickStates_[joyId].numAxes_, joystickStates_[joyId].numButtons_, joystickStates_[joyId].numVibrators_);

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
	AndroidJniClass_InputDevice inputDevice = AndroidJniClass_InputDevice::getDevice(deviceId);
	return (inputDevice.isNull() == false);
}

void AndroidInputManager::deviceInfo(int deviceId, int joyId)
{
	const int MaxStringLength = 256;
	nctl::StaticString<MaxStringLength> deviceInfoString;

	AndroidJniClass_InputDevice inputDevice = AndroidJniClass_InputDevice::getDevice(deviceId);
	if (inputDevice.isNull() == false)
	{
		AndroidJoystickState &joyState = joystickStates_[joyId];

		// InputDevice.getName()
		inputDevice.getName(joyState.name_, AndroidJoystickState::MaxNameLength);

		const int vendorId = inputDevice.getVendorId();
		const int productId = inputDevice.getProductId();
		const unsigned int length = inputDevice.getDescriptor(deviceInfoString.data(), MaxStringLength);
		deviceInfoString.setLength(length);
		joyState.createGuid(AndroidJoystickState::BluetoothBus, vendorId, productId, 0, deviceInfoString.data(), 0, 0);

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

		deviceInfoString.clear();
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
				joyState.buttonsMapping_[i] = numFoundButtons;
				deviceInfoString.formatAppend(" %d:%d", numFoundButtons, keyCode);
				numFoundButtons++;
			}
			else
				joyState.buttonsMapping_[i] = -1;

			if (numFoundButtons >= AndroidJoystickState::MaxButtons)
				break;
		}
		joyState.numButtons_ = numFoundButtons;
		if (numFoundButtons == 0)
			deviceInfoString.append(" not detected");
		LOGV_X("device (%d, %d) - Buttons%s", deviceId, joyId, deviceInfoString.data());

		// Checking hats buttons from AKEYCODE_DPAD_UP to AKEYCODE_DPAD_CENTER
		joyState.hasDPad_ = true;

		const int hatButtons = 4;
		bool checkedHatButtons[hatButtons];
		if (AndroidJniHelper::sdkVersion() >= 19 && __ANDROID_API__ >= 19)
		{
			int hatButtonsToCheck[hatButtons];
			for (int i = 0; i < hatButtons; i++)
				hatButtonsToCheck[i] = AKEYCODE_DPAD_UP + i;

			// InputDevice.hasKeys()
			inputDevice.hasKeys(hatButtonsToCheck, hatButtons, checkedHatButtons);
		}

		for (int i = 0; i < hatButtons; i++)
		{
			bool hasKey = false;
			const int keyCode = AKEYCODE_DPAD_UP + i;

			if (AndroidJniHelper::sdkVersion() >= 19 && __ANDROID_API__ >= 19)
				hasKey = checkedHatButtons[i];
			else // KeyCharacterMap.deviceHasKey()
				hasKey = AndroidJniClass_KeyCharacterMap::deviceHasKey(keyCode);

			if (hasKey == false)
			{
				joyState.hasDPad_ = false;
				LOGV_X("device (%d, %d) - D-Pad not detected", deviceId, joyId);
				break;
			}
		}

		deviceInfoString.clear();
		joyState.hasHatAxes_ = true;
		// InputDevice.getMotionRange()
		int numAxes = 0;
		int numMappedAxes = 0;
		for (int i = 0; i < AndroidJoystickState::NumAxesToMap; i++)
		{
			const int axis = AndroidJoystickState::AxesToMap[i];
			AndroidJniClass_MotionRange motionRange = inputDevice.getMotionRange(axis);

			if (motionRange.isNull() == false)
			{
				const float minValue = motionRange.getMin();
				const float rangeValue = motionRange.getRange();

				if (numAxes < AndroidJoystickState::MaxAxes)
				{
					joyState.axesMapping_[numAxes] = axis;
					deviceInfoString.formatAppend(" %d:%d (%.2f to %.2f)", numAxes, axis, minValue, minValue + rangeValue);

					ASSERT(rangeValue != 0.0f);
					// Avoid a division by zero by only assigning valid range values
					if (rangeValue != 0.0f)
					{
						joyState.axesMinValues_[numAxes] = minValue;
						joyState.axesRangeValues_[numAxes] = rangeValue;
					}
					else
					{
						joyState.axesMinValues_[numAxes] = -1.0f;
						joyState.axesRangeValues_[numAxes] = 2.0f;
					}
				}

				numAxes++;
				if (axis != AMOTION_EVENT_AXIS_HAT_X && axis != AMOTION_EVENT_AXIS_HAT_Y)
					numMappedAxes++;
			}
			else
			{
				if ((axis == AMOTION_EVENT_AXIS_HAT_X || axis == AMOTION_EVENT_AXIS_HAT_Y) && joyState.hasHatAxes_ == true)
				{
					joyState.hasHatAxes_ = false;
					LOGV_X("device (%d, %d) - Axis hats not detected", deviceId, joyId);
				}
			}
		}
		joyState.numAxes_ = numAxes;
		joyState.numMappedAxes_ = numMappedAxes;
		if (numAxes == 0)
			deviceInfoString.append(" not detected");
		LOGV_X("device (%d, %d) - Axes%s", deviceId, joyId, deviceInfoString.data());

		joyState.numHats_ = 0;
		if (joyState.hasDPad_ || joyState.hasHatAxes_)
			joyState.numHats_ = 1; // No more than one hat is supported

		deviceInfoString.clear();
		if (AndroidJniHelper::sdkVersion() >= 31)
		{
			AndroidJniClass_VibratorManager vibratorManager = inputDevice.getVibratorManager();

			const int numVibrators = vibratorManager.getVibratorIds(joyState.vibratorsIds_, AndroidJoystickState::MaxVibrators);
			// There might be more vibrators available than the maximum number supported
			joyState.numVibrators_ = (numVibrators > AndroidJoystickState::MaxVibrators) ? AndroidJoystickState::MaxVibrators : numVibrators;

			if (joyState.numVibrators_ == 0)
				deviceInfoString.append(" not detected");
			else
			{
				for (int i = 0; i < joyState.numVibrators_; i++)
				{
					joyState.vibrators_[i] = vibratorManager.getVibrator(joyState.vibratorsIds_[i]);
					deviceInfoString.formatAppend(" %d", joyState.vibratorsIds_[i]);
				}
			}
			LOGV_X("device (%d, %d) - Vibrator Ids%s (%d)", deviceId, joyId, deviceInfoString.data(), numVibrators);
		}

		joyState.updateGuidWithCapabilities();
	}
}

void AndroidInputManager::copyButtonStatesToPrev()
{
	keyboardState_.copyKeyStateToPrev();
	mouseState_.copyButtonStateToPrev();

	for (unsigned int joyId = 0; joyId < MaxNumJoysticks; joyId++)
		joystickStates_[joyId].copyButtonStateToPrev();
	joyMapping_.copyButtonStateToPrev();
}

}
