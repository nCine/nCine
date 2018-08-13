#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaJoystickEvents.h"
#include "LuaClassWrapper.h"
#include "LuaNames.h"
#include "InputEvents.h"

namespace ncine {

namespace LuaNames {
namespace LuaJoystickEvents
{
	static const char *isButtonPressed = "joy_button_pressed";
	static const char *axisValue = "joy_axis_value";
	static const char *axisNormValue = "joy_axis_normvalue";

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
	static const char *ButtonName = "joy_button";

	static const char *LX = "LX";
	static const char *LY = "LY";
	static const char *RX = "RX";
	static const char *RY = "RY";
	static const char *LTRIGGER = "LTRIGGER";
	static const char *RTRIGGER = "RTRIGGER";
	static const char *DPAD_RX = "DPAD_RX";
	static const char *DPAD_RY = "DPAD_RY";
	static const char *AxisName = "joy_axis";

	static const char *joyId = "joy_id";
	static const char *buttonId = "button_id";
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
		lua_newtable(L);
	}

	LuaUtils::addFunction(L, LuaNames::LuaJoystickEvents::isButtonPressed, isButtonPressed);
	LuaUtils::addFunction(L, LuaNames::LuaJoystickEvents::axisValue, axisValue);
	LuaUtils::addFunction(L, LuaNames::LuaJoystickEvents::axisNormValue, axisNormValue);

	lua_setfield(L, -2, LuaNames::input);
}

void LuaJoystickEvents::exposeConstants(lua_State *L)
{
	lua_createtable(L, 12, 0);

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

	lua_setfield(L, -2, LuaNames::LuaJoystickEvents::ButtonName);

	lua_createtable(L, 9, 0);

	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::UNKNOWN, static_cast<int64_t>(AxisName::UNKNOWN));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::LX, static_cast<int64_t>(AxisName::LX));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::LY, static_cast<int64_t>(AxisName::LY));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::RX, static_cast<int64_t>(AxisName::RX));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::RY, static_cast<int64_t>(AxisName::RY));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::LTRIGGER, static_cast<int64_t>(AxisName::LTRIGGER));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::RTRIGGER, static_cast<int64_t>(AxisName::RTRIGGER));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::DPAD_RX, static_cast<int64_t>(AxisName::DPAD_RX));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::DPAD_RY, static_cast<int64_t>(AxisName::DPAD_RY));

	lua_setfield(L, -2, LuaNames::LuaJoystickEvents::AxisName);
}

void LuaJoystickEvents::pushJoyButtonEvent(lua_State *L, const JoyButtonEvent &event)
{
	lua_createtable(L, 2, 0);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::joyId, event.joyId);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::buttonId, event.buttonId);
}

void LuaJoystickEvents::pushJoyAxisEvent(lua_State *L, const JoyAxisEvent &event)
{
	lua_createtable(L, 4, 0);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::joyId, event.joyId);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::axisId, event.axisId);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::value, event.value);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::normValue, event.normValue);
}

void LuaJoystickEvents::pushJoyMappedButtonEvent(lua_State *L, const JoyMappedButtonEvent &event)
{
	lua_createtable(L, 2, 0);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::joyId, event.joyId);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::buttonName, static_cast<int64_t>(event.buttonName));
}

void LuaJoystickEvents::pushJoyMappedAxisEvent(lua_State *L, const JoyMappedAxisEvent &event)
{
	lua_createtable(L, 3, 0);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::joyId, event.joyId);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::axisName, static_cast<int64_t>(event.axisName));
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::value, event.value);
}

void LuaJoystickEvents::pushJoyConnectionEvent(lua_State *L, const JoyConnectionEvent &event)
{
	lua_createtable(L, 1, 0);
	LuaUtils::pushField(L, LuaNames::LuaJoystickEvents::joyId, event.joyId);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaJoystickEvents::isButtonPressed(lua_State *L)
{
	bool isButtonPressed = false;

	LuaStateManager::UserDataWrapper *wrapper = LuaUtils::retrieveUserData<LuaStateManager::UserDataWrapper>(L, -2);
	if (wrapper->type == LuaTypes::JOYSTICKSTATE)
	{
		const JoystickState *state = LuaClassWrapper<JoystickState>::unwrapUserData(L, -2);
		const int buttonId = LuaUtils::retrieve<int32_t>(L, -1);
		isButtonPressed = state->isButtonPressed(buttonId);
	}
	else if (wrapper->type == LuaTypes::JOYMAPPEDSTATE)
	{
		const JoyMappedState *state = LuaClassWrapper<JoyMappedState>::unwrapUserData(L, -1);
		const ButtonName button = static_cast<ButtonName>(LuaUtils::retrieve<int64_t>(L, -1));
		isButtonPressed = state->isButtonPressed(button);
	}
	else
		LuaDebug::traceError(L, "Expecting a JoystickState or a JoyMappedState object instead of %s", LuaTypes::wrapperToName(wrapper->type));

	LuaUtils::push(L, isButtonPressed);

	return 1;
}

int LuaJoystickEvents::axisValue(lua_State *L)
{
	float axisValue = 0.0f;

	LuaStateManager::UserDataWrapper *wrapper = LuaUtils::retrieveUserData<LuaStateManager::UserDataWrapper>(L, -2);
	if (wrapper->type == LuaTypes::JOYSTICKSTATE)
	{
		const JoystickState *state = LuaClassWrapper<JoystickState>::unwrapUserData(L, -2);
		const int axisId = LuaUtils::retrieve<int32_t>(L, -1);
		axisValue = state->axisValue(axisId);
	}
	else if (wrapper->type == LuaTypes::JOYMAPPEDSTATE)
	{
		const JoyMappedState *state = LuaClassWrapper<JoyMappedState>::unwrapUserData(L, -1);
		const AxisName axis = static_cast<AxisName>(LuaUtils::retrieve<int64_t>(L, -1));
		axisValue = state->axisValue(axis);
	}
	else
		LuaDebug::traceError(L, "Expecting a JoystickState or a JoyMappedState object instead of %s", LuaTypes::wrapperToName(wrapper->type));

	LuaUtils::push(L, axisValue);

	return 1;
}

int LuaJoystickEvents::axisNormValue(lua_State *L)
{
	const JoystickState *state = LuaClassWrapper<JoystickState>::unwrapUserData(L, -2);
	const int axisId = LuaUtils::retrieve<int32_t>(L, -1);

	LuaUtils::push(L, state->axisNormValue(axisId));

	return 1;
}

}
