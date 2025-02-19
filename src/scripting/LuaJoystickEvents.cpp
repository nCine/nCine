#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaJoystickEvents.h"
#include "LuaUntrackedUserData.h"
#include "LuaNames.h"
#include "InputEvents.h"

namespace ncine {

namespace LuaNames {
namespace LuaJoystickEvents {
	static const char *isButtonDown = "is_joy_button_down";
	static const char *isButtonPressed = "is_joy_button_pressed";
	static const char *isButtonReleased = "is_joy_button_released";

	static const char *joyHatState = "joy_hat_state";
	static const char *axisValue = "joy_axis_value";
	static const char *axisNormValue = "joy_axis_normvalue";

	static const char *CENTERED = "CENTERED";
	static const char *UP = "UP";
	static const char *RIGHT = "RIGHT";
	static const char *DOWN = "DOWN";
	static const char *LEFT = "LEFT";
	static const char *RIGHT_UP = "RIGHT_UP";
	static const char *RIGHT_DOWN = "RIGHT_DOWN";
	static const char *LEFT_UP = "LEFT_UP";
	static const char *LEFT_DOWN = "LEFT_DOWN";
	static const char *HatState = "joy_hat_state";

	static const char *UNKNOWN = "UNKNOWN";
	static const char *A = "A";
	static const char *B = "B";
	static const char *X = "X";
	static const char *Y = "Y";
	static const char *BACK = "BACK";
	static const char *GUIDE = "GUIDE";
	static const char *START = "START";
	static const char *LSTICK = "LSTICK";
	static const char *RSTICK = "RSTICK";
	static const char *LBUMPER = "LBUMPER";
	static const char *RBUMPER = "RBUMPER";
	static const char *DPAD_UP = "DPAD_UP";
	static const char *DPAD_DOWN = "DPAD_DOWN";
	static const char *DPAD_LEFT = "DPAD_LEFT";
	static const char *DPAD_RIGHT = "DPAD_RIGHT";
	static const char *MISC1 = "MISC1";
	static const char *PADDLE1 = "PADDLE1";
	static const char *PADDLE2 = "PADDLE2";
	static const char *PADDLE3 = "PADDLE3";
	static const char *PADDLE4 = "PADDLE4";
	static const char *ButtonName = "joy_button";

	static const char *LX = "LX";
	static const char *LY = "LY";
	static const char *RX = "RX";
	static const char *RY = "RY";
	static const char *LTRIGGER = "LTRIGGER";
	static const char *RTRIGGER = "RTRIGGER";
	static const char *AxisName = "joy_axis";

	static const char *joyId = "joy_id";
	static const char *buttonId = "button_id";
	static const char *hatId = "hat_id";
	static const char *hatState = "hat_state";
	static const char *axisId = "axis_id";
	static const char *value = "value";
	static const char *normValue = "norm_value";
	static const char *buttonName = "button";
	static const char *axisName = "axis";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaJoystickEvents::expose(lua_State *L)
{
	// Multiple binding classes adding functions to the same field
	const int type = lua_getfield(L, -1, LuaNames::input);

	if (type == LUA_TNIL)
	{
		lua_pop(L, 1);
		lua_createtable(L, 0, 6);
	}

	LuaUtils::addFunction(L, LuaNames::LuaJoystickEvents::isButtonDown, isButtonDown);
	LuaUtils::addFunction(L, LuaNames::LuaJoystickEvents::isButtonPressed, isButtonPressed);
	LuaUtils::addFunction(L, LuaNames::LuaJoystickEvents::isButtonReleased, isButtonReleased);

	LuaUtils::addFunction(L, LuaNames::LuaJoystickEvents::joyHatState, hatState);
	LuaUtils::addFunction(L, LuaNames::LuaJoystickEvents::axisValue, axisValue);
	LuaUtils::addFunction(L, LuaNames::LuaJoystickEvents::axisNormValue, axisNormValue);

	lua_setfield(L, -2, LuaNames::input);
}

void LuaJoystickEvents::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 9);

	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::CENTERED, static_cast<int64_t>(HatState::CENTERED));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::UP, static_cast<int64_t>(HatState::UP));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::RIGHT, static_cast<int64_t>(HatState::RIGHT));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::DOWN, static_cast<int64_t>(HatState::DOWN));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::LEFT, static_cast<int64_t>(HatState::LEFT));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::RIGHT_UP, static_cast<int64_t>(HatState::RIGHT_UP));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::RIGHT_DOWN, static_cast<int64_t>(HatState::RIGHT_DOWN));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::LEFT_UP, static_cast<int64_t>(HatState::LEFT_UP));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::LEFT_DOWN, static_cast<int64_t>(HatState::LEFT_DOWN));

	lua_setfield(L, -2, LuaNames::LuaJoystickEvents::HatState);

	lua_createtable(L, 0, 21);

	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::UNKNOWN, static_cast<int64_t>(ButtonName::UNKNOWN));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::A, static_cast<int64_t>(ButtonName::A));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::B, static_cast<int64_t>(ButtonName::B));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::X, static_cast<int64_t>(ButtonName::X));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::Y, static_cast<int64_t>(ButtonName::Y));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::BACK, static_cast<int64_t>(ButtonName::BACK));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::GUIDE, static_cast<int64_t>(ButtonName::GUIDE));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::START, static_cast<int64_t>(ButtonName::START));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::LSTICK, static_cast<int64_t>(ButtonName::LSTICK));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::RSTICK, static_cast<int64_t>(ButtonName::RSTICK));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::LBUMPER, static_cast<int64_t>(ButtonName::LBUMPER));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::RBUMPER, static_cast<int64_t>(ButtonName::RBUMPER));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::DPAD_UP, static_cast<int64_t>(ButtonName::DPAD_UP));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::DPAD_DOWN, static_cast<int64_t>(ButtonName::DPAD_DOWN));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::DPAD_LEFT, static_cast<int64_t>(ButtonName::DPAD_LEFT));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::DPAD_RIGHT, static_cast<int64_t>(ButtonName::DPAD_RIGHT));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::MISC1, static_cast<int64_t>(ButtonName::MISC1));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::PADDLE1, static_cast<int64_t>(ButtonName::PADDLE1));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::PADDLE2, static_cast<int64_t>(ButtonName::PADDLE2));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::PADDLE3, static_cast<int64_t>(ButtonName::PADDLE3));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::PADDLE4, static_cast<int64_t>(ButtonName::PADDLE4));

	lua_setfield(L, -2, LuaNames::LuaJoystickEvents::ButtonName);

	lua_createtable(L, 0, 7);

	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::UNKNOWN, static_cast<int64_t>(AxisName::UNKNOWN));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::LX, static_cast<int64_t>(AxisName::LX));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::LY, static_cast<int64_t>(AxisName::LY));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::RX, static_cast<int64_t>(AxisName::RX));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::RY, static_cast<int64_t>(AxisName::RY));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::LTRIGGER, static_cast<int64_t>(AxisName::LTRIGGER));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::RTRIGGER, static_cast<int64_t>(AxisName::RTRIGGER));

	lua_setfield(L, -2, LuaNames::LuaJoystickEvents::AxisName);
}

void LuaJoystickEvents::pushJoyButtonEvent(lua_State *L, const JoyButtonEvent &event)
{
	lua_createtable(L, 0, 2);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::joyId, event.joyId);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::buttonId, event.buttonId);
}

void LuaJoystickEvents::pushJoyHatEvent(lua_State *L, const JoyHatEvent &event)
{
	lua_createtable(L, 0, 3);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::joyId, event.joyId);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::hatId, event.hatId);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::hatState, event.hatState);
}

void LuaJoystickEvents::pushJoyAxisEvent(lua_State *L, const JoyAxisEvent &event)
{
	lua_createtable(L, 0, 4);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::joyId, event.joyId);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::axisId, event.axisId);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::value, event.value);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::normValue, event.normValue);
}

void LuaJoystickEvents::pushJoyMappedButtonEvent(lua_State *L, const JoyMappedButtonEvent &event)
{
	lua_createtable(L, 0, 2);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::joyId, event.joyId);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::buttonName, static_cast<int64_t>(event.buttonName));
}

void LuaJoystickEvents::pushJoyMappedAxisEvent(lua_State *L, const JoyMappedAxisEvent &event)
{
	lua_createtable(L, 0, 3);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::joyId, event.joyId);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::axisName, static_cast<int64_t>(event.axisName));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::value, event.value);
}

void LuaJoystickEvents::pushJoyConnectionEvent(lua_State *L, const JoyConnectionEvent &event)
{
	lua_createtable(L, 0, 1);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::joyId, event.joyId);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaJoystickEvents::isButtonDown(lua_State *L)
{
	bool isButtonDown = false;

	void *pointer = LuaUtils::retrieveUserData<void *>(L, -2);
	const LuaTypes::UserDataType type = LuaStateManager::manager(L)->untrackedType(pointer);

	if (type == LuaTypes::JOYSTICKSTATE)
	{
		const JoystickState *state = reinterpret_cast<JoystickState *>(pointer);
		const int buttonId = LuaUtils::retrieve<int32_t>(L, -1);
		if (state)
			isButtonDown = state->isButtonDown(buttonId);
	}
	else if (type == LuaTypes::JOYMAPPEDSTATE)
	{
		const JoyMappedState *state = reinterpret_cast<JoyMappedState *>(pointer);
		const ButtonName button = static_cast<ButtonName>(LuaUtils::retrieve<int64_t>(L, -1));
		if (state)
			isButtonDown = state->isButtonDown(button);
	}
	else
		LuaDebug::traceError(L, "Expecting a JoystickState or a JoyMappedState object instead of %s", LuaTypes::userDataTypeToName(type));

	LuaUtils::push(L, isButtonDown);

	return 1;
}

int LuaJoystickEvents::isButtonPressed(lua_State *L)
{
	bool isButtonPressed = false;

	void *pointer = LuaUtils::retrieveUserData<void *>(L, -2);
	const LuaTypes::UserDataType type = LuaStateManager::manager(L)->untrackedType(pointer);

	if (type == LuaTypes::JOYSTICKSTATE)
	{
		const JoystickState *state = reinterpret_cast<JoystickState *>(pointer);
		const int buttonId = LuaUtils::retrieve<int32_t>(L, -1);
		if (state)
			isButtonPressed = state->isButtonPressed(buttonId);
	}
	else if (type == LuaTypes::JOYMAPPEDSTATE)
	{
		const JoyMappedState *state = reinterpret_cast<JoyMappedState *>(pointer);
		const ButtonName button = static_cast<ButtonName>(LuaUtils::retrieve<int64_t>(L, -1));
		if (state)
			isButtonPressed = state->isButtonPressed(button);
	}
	else
		LuaDebug::traceError(L, "Expecting a JoystickState or a JoyMappedState object instead of %s", LuaTypes::userDataTypeToName(type));

	LuaUtils::push(L, isButtonPressed);

	return 1;
}

int LuaJoystickEvents::isButtonReleased(lua_State *L)
{
	bool isButtonReleased = false;

	void *pointer = LuaUtils::retrieveUserData<void *>(L, -2);
	const LuaTypes::UserDataType type = LuaStateManager::manager(L)->untrackedType(pointer);

	if (type == LuaTypes::JOYSTICKSTATE)
	{
		const JoystickState *state = reinterpret_cast<JoystickState *>(pointer);
		const int buttonId = LuaUtils::retrieve<int32_t>(L, -1);
		if (state)
			isButtonReleased = state->isButtonReleased(buttonId);
	}
	else if (type == LuaTypes::JOYMAPPEDSTATE)
	{
		const JoyMappedState *state = reinterpret_cast<JoyMappedState *>(pointer);
		const ButtonName button = static_cast<ButtonName>(LuaUtils::retrieve<int64_t>(L, -1));
		if (state)
			isButtonReleased = state->isButtonReleased(button);
	}
	else
		LuaDebug::traceError(L, "Expecting a JoystickState or a JoyMappedState object instead of %s", LuaTypes::userDataTypeToName(type));

	LuaUtils::push(L, isButtonReleased);

	return 1;
}

int LuaJoystickEvents::hatState(lua_State *L)
{
	unsigned char hatState = HatState::CENTERED;

	void *pointer = LuaUtils::retrieveUserData<void *>(L, -2);
	const LuaTypes::UserDataType type = LuaStateManager::manager(L)->untrackedType(pointer);

	if (type == LuaTypes::JOYSTICKSTATE)
	{
		const JoystickState *state = reinterpret_cast<JoystickState *>(pointer);
		const int hatId = LuaUtils::retrieve<int32_t>(L, -1);
		if (state)
			hatState = state->hatState(hatId);
	}
	else
		LuaDebug::traceError(L, "Expecting a JoystickState object instead of %s", LuaTypes::userDataTypeToName(type));

	LuaUtils::push(L, hatState);

	return 1;
}

int LuaJoystickEvents::axisValue(lua_State *L)
{
	float axisValue = 0.0f;

	void *pointer = LuaUtils::retrieveUserData<void *>(L, -2);
	const LuaTypes::UserDataType type = LuaStateManager::manager(L)->untrackedType(pointer);

	if (type == LuaTypes::JOYSTICKSTATE)
	{
		const JoystickState *state = reinterpret_cast<JoystickState *>(pointer);
		const int axisId = LuaUtils::retrieve<int32_t>(L, -1);
		if (state)
			axisValue = state->axisValue(axisId);
	}
	else if (type == LuaTypes::JOYMAPPEDSTATE)
	{
		const JoyMappedState *state = reinterpret_cast<JoyMappedState *>(pointer);
		const AxisName axis = static_cast<AxisName>(LuaUtils::retrieve<int64_t>(L, -1));
		if (state)
			axisValue = state->axisValue(axis);
	}
	else
		LuaDebug::traceError(L, "Expecting a JoystickState or a JoyMappedState object instead of %s", LuaTypes::userDataTypeToName(type));

	LuaUtils::push(L, axisValue);

	return 1;
}

int LuaJoystickEvents::axisNormValue(lua_State *L)
{
	const JoystickState *state = LuaUntrackedUserData<JoystickState>::retrieve(L, -2);
	const int axisId = LuaUtils::retrieve<int32_t>(L, -1);

	if (state)
		LuaUtils::push(L, state->axisNormValue(axisId));
	else
		LuaUtils::pushNil(L);

	return 1;
}

}
