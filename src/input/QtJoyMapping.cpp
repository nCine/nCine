#include "JoyMapping.h"
#include "IInputManager.h"
#include "IInputEventHandler.h"
#include "Vector2.h"
#include <nctl/algorithms.h>

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const unsigned int JoyMapping::MaxNameLength;

const char *JoyMapping::AxesStrings[JoyMappedState::NumAxes] = {
	"leftx",
	"lefty",
	"rightx",
	"righty",
	"lefttrigger",
	"righttrigger"
};

const char *JoyMapping::ButtonsStrings[JoyMappedState::NumButtons] = {
	"a",
	"b",
	"x",
	"y",
	"back",
	"guide",
	"start",
	"leftstick",
	"rightstick",
	"leftshoulder",
	"rightshoulder",
	"dpup",
	"dpdown",
	"dpleft",
	"dpright",
	"misc1",
	"paddle1",
	"paddle2",
	"paddle3",
	"paddle4"
};

JoyMappedStateImpl JoyMapping::nullMappedJoyState_;
nctl::StaticArray<JoyMappedStateImpl, JoyMapping::MaxNumJoysticks> JoyMapping::mappedJoyStates_(nctl::StaticArrayMode::EXTEND_SIZE);
JoyMappedButtonEvent JoyMapping::mappedButtonEvent_;
JoyMappedAxisEvent JoyMapping::mappedAxisEvent_;

///////////////////////////////////////////////////////////
// JoyMappedStateImpl (copied from `JoyMapping.cpp`)
///////////////////////////////////////////////////////////

JoyMappedStateImpl::JoyMappedStateImpl()
{
	memset(buttons_[0], 0, JoyMappedState::NumButtons);
	memset(buttons_[1], 0, JoyMappedState::NumButtons);
	for (unsigned int i = 0; i < JoyMappedState::NumAxes; i++)
		axesValues_[i] = 0.0f;
	lastHatState_ = HatState::CENTERED;
}

bool JoyMappedStateImpl::isButtonDown(ButtonName name) const
{
	const unsigned int buttonIndex = static_cast<unsigned int>(name);
	bool isDown = false;
	if (name != ButtonName::UNKNOWN)
		isDown = (buttons_[currentButtonStateIndex_][buttonIndex] != 0);
	return isDown;
}

bool JoyMappedStateImpl::isButtonPressed(ButtonName name) const
{
	const unsigned int prevButtonStateIndex = (currentButtonStateIndex_ == 0 ? 1 : 0);
	const unsigned int buttonIndex = static_cast<unsigned int>(name);
	bool isPressed = false;
	if (name != ButtonName::UNKNOWN)
		isPressed = (buttons_[currentButtonStateIndex_][buttonIndex] != 0 && buttons_[prevButtonStateIndex][buttonIndex] == 0);
	return isPressed;
}

bool JoyMappedStateImpl::isButtonReleased(ButtonName name) const
{
	const unsigned int prevButtonStateIndex = (currentButtonStateIndex_ == 0 ? 1 : 0);
	const unsigned int buttonIndex = static_cast<unsigned int>(name);
	bool isReleased = false;
	if (name != ButtonName::UNKNOWN)
		isReleased = (buttons_[currentButtonStateIndex_][buttonIndex] == 0 && buttons_[prevButtonStateIndex][buttonIndex] != 0);
	return isReleased;
}

float JoyMappedStateImpl::axisValue(AxisName name) const
{
	float value = 0.0f;
	if (name != AxisName::UNKNOWN)
		value = axesValues_[static_cast<int>(name)];
	return value;
}

void JoyMappedStateImpl::copyButtonStateToPrev()
{
	const unsigned int prevButtonStateIndex = (currentButtonStateIndex_ == 0 ? 1 : 0);
	memcpy(buttons_[prevButtonStateIndex], buttons_[currentButtonStateIndex_], JoyMappedState::NumButtons * sizeof(unsigned char));
	currentButtonStateIndex_ = prevButtonStateIndex;
}

void JoyMappedStateImpl::resetPrevButtonState()
{
	const unsigned int prevButtonStateIndex = (currentButtonStateIndex_ == 0 ? 1 : 0);
	memset(buttons_[prevButtonStateIndex], 0, JoyMappedState::NumButtons);
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
///////////////////////////////////////////////////////////

JoyMapping::MappedJoystick::MappedJoystick()
{
	name[0] = '\0';

	for (unsigned int i = 0; i < MaxNumAxes; i++)
		axes[i].name = AxisName::UNKNOWN;
	for (unsigned int i = 0; i < MaxNumAxes; i++)
		buttonAxes[i] = AxisName::UNKNOWN;
	for (unsigned int i = 0; i < MaxNumButtons; i++)
		buttons[i] = ButtonName::UNKNOWN;
	for (unsigned int i = 0; i < MaxHatButtons; i++)
		hats[i] = ButtonName::UNKNOWN;
}

JoyMapping::MappedJoystick::Guid::Guid()
{
	for (unsigned int i = 0; i < 4; i++)
		array_[i] = 0;
}

JoyMapping::JoyMapping()
    : mappings_(1), inputManager_(nullptr), inputEventHandler_(nullptr)
{
	mappings_.emplaceBack();
	mappings_[0].axes[0].name = AxisName::LX;
	mappings_[0].axes[0].min = -1.0f;
	mappings_[0].axes[0].max = 1.0f;
	mappings_[0].axes[1].name = AxisName::LY;
	mappings_[0].axes[1].min = -1.0f;
	mappings_[0].axes[1].max = 1.0f;
	mappings_[0].axes[2].name = AxisName::RX;
	mappings_[0].axes[2].min = -1.0f;
	mappings_[0].axes[2].max = 1.0f;
	mappings_[0].axes[3].name = AxisName::RY;
	mappings_[0].axes[3].min = -1.0f;
	mappings_[0].axes[3].max = 1.0f;
	mappings_[0].axes[4].name = AxisName::LTRIGGER;
	mappings_[0].axes[4].min = 0.0f;
	mappings_[0].axes[4].max = 1.0f;
	mappings_[0].axes[5].name = AxisName::RTRIGGER;
	mappings_[0].axes[5].min = 0.0f;
	mappings_[0].axes[5].max = 1.0f;

	mappings_[0].buttons[0] = ButtonName::LBUMPER;
	mappings_[0].buttons[1] = ButtonName::LSTICK;
	mappings_[0].buttons[2] = ButtonName::RBUMPER;
	mappings_[0].buttons[3] = ButtonName::RSTICK;
	mappings_[0].buttons[4] = ButtonName::A;
	mappings_[0].buttons[5] = ButtonName::B;
	mappings_[0].buttons[6] = ButtonName::UNKNOWN;
	mappings_[0].buttons[7] = ButtonName::GUIDE;
	mappings_[0].buttons[8] = ButtonName::BACK;
	mappings_[0].buttons[9] = ButtonName::START;
	mappings_[0].buttons[10] = ButtonName::X;
	mappings_[0].buttons[11] = ButtonName::Y;

	mappings_[0].hats[0] = ButtonName::DPAD_UP;
	mappings_[0].hats[1] = ButtonName::DPAD_DOWN;
	mappings_[0].hats[2] = ButtonName::DPAD_RIGHT;
	mappings_[0].hats[3] = ButtonName::DPAD_LEFT;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void JoyMapping::MappedJoystick::Guid::fromString(const char *string)
{
}

bool JoyMapping::MappedJoystick::Guid::operator==(const Guid &guid) const
{
	return false;
}

void JoyMapping::init(const IInputManager *inputManager)
{
	ASSERT(inputManager);
	inputManager_ = inputManager;
}

bool JoyMapping::addMappingFromString(const char *mappingString)
{
	return false;
}

void JoyMapping::addMappingsFromStrings(const char **mappingStrings)
{
}

void JoyMapping::addMappingsFromFile(const char *filename)
{
}

void JoyMapping::onJoyButtonPressed(const JoyButtonEvent &event)
{
	if (inputEventHandler_ == nullptr)
		return;

	const int mappingIndex = mappingIndices_[event.joyId];
	const bool mappingIsValid = (mappingIndex != InvalidMappingIndex && event.buttonId >= 0 &&
	                             event.buttonId < static_cast<int>(MappedJoystick::MaxNumButtons));

	if (mappingIsValid)
	{
		mappedButtonEvent_.joyId = event.joyId;
		mappedButtonEvent_.buttonName = mappings_[mappingIndex].buttons[event.buttonId];
		if (mappedButtonEvent_.buttonName != ButtonName::UNKNOWN)
		{
			const unsigned int currentButtonStateIndex_ = mappedJoyStates_[event.joyId].currentButtonStateIndex_;
			const int buttonId = static_cast<int>(mappedButtonEvent_.buttonName);
			mappedJoyStates_[event.joyId].buttons_[currentButtonStateIndex_][buttonId] = true;
			inputEventHandler_->onJoyMappedButtonPressed(mappedButtonEvent_);
		}
		// There are no button axes mapped in the class constructor
	}
}

void JoyMapping::onJoyButtonReleased(const JoyButtonEvent &event)
{
	if (inputEventHandler_ == nullptr)
		return;

	const int mappingIndex = mappingIndices_[event.joyId];
	const bool mappingIsValid = (mappingIndex != InvalidMappingIndex && event.buttonId >= 0 &&
	                             event.buttonId < static_cast<int>(MappedJoystick::MaxNumButtons));

	if (mappingIsValid)
	{
		mappedButtonEvent_.joyId = event.joyId;
		mappedButtonEvent_.buttonName = mappings_[mappingIndex].buttons[event.buttonId];
		if (mappedButtonEvent_.buttonName != ButtonName::UNKNOWN)
		{
			const unsigned int currentButtonStateIndex_ = mappedJoyStates_[event.joyId].currentButtonStateIndex_;
			const int buttonId = static_cast<int>(mappedButtonEvent_.buttonName);
			mappedJoyStates_[event.joyId].buttons_[currentButtonStateIndex_][buttonId] = false;
			inputEventHandler_->onJoyMappedButtonReleased(mappedButtonEvent_);
		}
		// There are no button axes mapped in the class constructor
	}
}

void JoyMapping::onJoyHatMoved(const JoyHatEvent &event)
{
	if (inputEventHandler_ == nullptr)
		return;

	const int mappingIndex = mappingIndices_[event.joyId];
	// Only the first gamepad hat is mapped
	const bool mappingIsValid = (mappingIndex != InvalidMappingIndex && event.hatId == 0);

	const unsigned char oldHatState = mappedJoyStates_[event.joyId].lastHatState_;
	const unsigned char newHatState = event.hatState;
	if (mappingIsValid && oldHatState != newHatState)
	{
		mappedButtonEvent_.joyId = event.joyId;

		const unsigned char firstHatValue = HatState::UP;
		const unsigned char lastHatValue = HatState::LEFT;
		for (unsigned char hatValue = firstHatValue; hatValue <= lastHatValue; hatValue *= 2)
		{
			if ((oldHatState & hatValue) != (newHatState & hatValue))
			{
				const int hatIndex = hatStateToIndex(hatValue);

				mappedButtonEvent_.buttonName = mappings_[mappingIndex].hats[hatIndex];
				if (mappedButtonEvent_.buttonName != ButtonName::UNKNOWN)
				{
					const unsigned int currentButtonStateIndex_ = mappedJoyStates_[event.joyId].currentButtonStateIndex_;
					const int buttonId = static_cast<int>(mappedButtonEvent_.buttonName);
					if (newHatState & hatValue)
					{
						mappedJoyStates_[event.joyId].buttons_[currentButtonStateIndex_][buttonId] = true;
						inputEventHandler_->onJoyMappedButtonPressed(mappedButtonEvent_);
					}
					else
					{
						mappedJoyStates_[event.joyId].buttons_[currentButtonStateIndex_][buttonId] = false;
						inputEventHandler_->onJoyMappedButtonReleased(mappedButtonEvent_);
					}
				}
			}
			mappedJoyStates_[event.joyId].lastHatState_ = event.hatState;
		}
	}
}

void JoyMapping::onJoyAxisMoved(const JoyAxisEvent &event)
{
	if (inputEventHandler_ == nullptr)
		return;

	const int mappingIndex = mappingIndices_[event.joyId];
	const bool mappingIsValid = (mappingIndex != InvalidMappingIndex && event.axisId >= 0 &&
	                             event.axisId < static_cast<int>(MappedJoystick::MaxNumAxes));

	if (mappingIsValid)
	{
		const MappedJoystick::Axis &axis = mappings_[mappingIndex].axes[event.axisId];

		mappedAxisEvent_.joyId = event.joyId;
		mappedAxisEvent_.axisName = axis.name;
		if (mappedAxisEvent_.axisName != AxisName::UNKNOWN)
		{
			const float value = (event.normValue + 1.0f) * 0.5f;
			mappedAxisEvent_.value = axis.min + value * (axis.max - axis.min);
			mappedJoyStates_[event.joyId].axesValues_[static_cast<int>(axis.name)] = mappedAxisEvent_.value;
			inputEventHandler_->onJoyMappedAxisMoved(mappedAxisEvent_);
		}
	}
}

bool JoyMapping::onJoyConnected(const JoyConnectionEvent &event)
{
	const char *joyName = inputManager_->joyName(event.joyId);

	// There is only one mapping for QGamepad
	mappingIndices_[event.joyId] = 0;
	LOGI_X("Joystick mapping found for \"%s\" (%d)", joyName, event.joyId);

	return (mappingIndices_[event.joyId] != InvalidMappingIndex);
}

void JoyMapping::onJoyDisconnected(const JoyConnectionEvent &event)
{
	mappingIndices_[event.joyId] = InvalidMappingIndex;
	mappedJoyStates_[event.joyId].resetPrevButtonState();
}

bool JoyMapping::isJoyMapped(int joyId) const
{
	return true;
}

const JoyMappedStateImpl &JoyMapping::joyMappedState(int joyId) const
{
	if (joyId < 0 || joyId > MaxNumJoysticks)
		return nullMappedJoyState_;
	else
		return mappedJoyStates_[joyId];
}

void JoyMapping::deadZoneNormalize(Vector2f &joyVector, float deadZoneValue) const
{
	deadZoneValue = nctl::clamp(deadZoneValue, 0.0f, 1.0f);

	if (joyVector.length() <= deadZoneValue)
		joyVector = Vector2f::Zero;
	else
	{
		float normalizedLength = (joyVector.length() - deadZoneValue) / (1.0f - deadZoneValue);
		normalizedLength = nctl::clamp(normalizedLength, 0.0f, 1.0f);
		joyVector = joyVector.normalize() * normalizedLength;
	}
}

void JoyMapping::copyButtonStateToPrev()
{
	for (unsigned int i = 0; i < MaxNumJoysticks; i++)
		mappedJoyStates_[i].copyButtonStateToPrev();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void JoyMapping::checkConnectedJoystics()
{
}

int JoyMapping::findMappingByGuid(const MappedJoystick::Guid &guid) const
{
	return 0;
}

int JoyMapping::findMappingByName(const char *name) const
{
	return 0;
}

bool JoyMapping::parseMappingFromString(const char *mappingString, MappedJoystick &map)
{
	return false;
}

bool JoyMapping::parsePlatformKeyword(const char *start, const char *end) const
{
	return false;
}

bool JoyMapping::parsePlatformName(const char *start, const char *end) const
{
	return false;
}

int JoyMapping::parseAxisName(const char *start, const char *end) const
{
	return -1;
}

int JoyMapping::parseButtonName(const char *start, const char *end) const
{
	return -1;
}

int JoyMapping::parseAxisMapping(const char *start, const char *end, MappedJoystick::Axis &axis) const
{
	return -1;
}

int JoyMapping::parseButtonMapping(const char *start, const char *end) const
{
	return -1;
}

int JoyMapping::parseHatMapping(const char *start, const char *end) const
{
	return -1;
}

int JoyMapping::hatStateToIndex(unsigned char hatState) const
{
	switch (hatState)
	{
		case HatState::UP: return 0;
		case HatState::DOWN: return 1;
		case HatState::RIGHT: return 2;
		case HatState::LEFT: return 3;
		default: return 0;
	}
}

void JoyMapping::trimSpaces(const char **start, const char **end) const
{
}

}
