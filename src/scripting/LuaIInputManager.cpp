#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaIInputManager.h"
#include "LuaUntrackedUserData.h"
#include "LuaMouseEvents.h"
#include "LuaVector2Utils.h"
#include "LuaNames.h"
#include "IInputManager.h"
#include "Application.h"

#include <nctl/Array.h>

namespace ncine {

namespace LuaNames {
namespace IInputManager {
	static const char *mouseState = "mouse_state";
	static const char *keyboardState = "key_state";

	static const char *isJoyPresent = "is_joy_present";
	static const char *joyName = "joy_name";
	static const char *joyGuid = "joy_guid";
	static const char *joyNumButtons = "joy_num_buttons";
	static const char *joyNumHats = "joy_num_hats";
	static const char *joyNumAxes = "joy_num_axes";
	static const char *joystickState = "joy_state";

	static const char *isJoyMapped = "is_joy_mapped";
	static const char *joyMappedState = "joymapped_state";
	static const char *deadZoneNormalize = "deadzone_normalize";

	static const char *addJoyMappingsFromFile = "add_joymappings_from_file";
	static const char *addJoyMappingsFromStrings = "add_joymappings_from_strings";
	static const char *numJoyMappings = "num_joymappings";

	static const char *mouseCursorMode = "get_mouse_cursor_mode";
	static const char *setMouseCursorMode = "set_mouse_cursor_mode";

	static const char *NORMAL = "NORMAL";
	static const char *HIDDEN = "HIDDEN";
	static const char *DISABLED = "DISABLED";
	static const char *MouseCursorMode = "mouse_cursor_mode";

	static const char *LEFT_STICK = "LEFT_STICK";
	static const char *RIGHT_STICK = "RIGHT_STICK";
	static const char *TRIGGER = "TRIGGER";
	static const char *DeadZone = "joy_dead_zone";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaIInputManager::expose(lua_State *L)
{
	// Multiple binding classes adding functions to the same field
	const int type = lua_getfield(L, -1, LuaNames::input);

	if (type == LUA_TNIL)
	{
		lua_pop(L, 1);
		lua_createtable(L, 0, 17);
	}

	LuaUtils::addFunction(L, LuaNames::IInputManager::mouseState, mouseState);
	LuaUtils::addFunction(L, LuaNames::IInputManager::keyboardState, keyboardState);

	LuaUtils::addFunction(L, LuaNames::IInputManager::isJoyPresent, isJoyPresent);
	LuaUtils::addFunction(L, LuaNames::IInputManager::joyName, joyName);
	LuaUtils::addFunction(L, LuaNames::IInputManager::joyGuid, joyGuid);
	LuaUtils::addFunction(L, LuaNames::IInputManager::joyNumButtons, joyNumButtons);
	LuaUtils::addFunction(L, LuaNames::IInputManager::joyNumHats, joyNumHats);
	LuaUtils::addFunction(L, LuaNames::IInputManager::joyNumAxes, joyNumAxes);
	LuaUtils::addFunction(L, LuaNames::IInputManager::joystickState, joystickState);

	LuaUtils::addFunction(L, LuaNames::IInputManager::isJoyMapped, isJoyMapped);
	LuaUtils::addFunction(L, LuaNames::IInputManager::joyMappedState, joyMappedState);
	LuaUtils::addFunction(L, LuaNames::IInputManager::deadZoneNormalize, deadZoneNormalize);

	LuaUtils::addFunction(L, LuaNames::IInputManager::addJoyMappingsFromFile, addJoyMappingsFromFile);
	LuaUtils::addFunction(L, LuaNames::IInputManager::addJoyMappingsFromStrings, addJoyMappingsFromStrings);
	LuaUtils::addFunction(L, LuaNames::IInputManager::numJoyMappings, numJoyMappings);

	LuaUtils::addFunction(L, LuaNames::IInputManager::mouseCursorMode, mouseCursorMode);
	LuaUtils::addFunction(L, LuaNames::IInputManager::setMouseCursorMode, setMouseCursorMode);

	lua_setfield(L, -2, LuaNames::input);
}

void LuaIInputManager::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 3);

	LuaUtils::pushField(L, LuaNames::IInputManager::NORMAL, static_cast<int64_t>(IInputManager::MouseCursorMode::NORMAL));
	LuaUtils::pushField(L, LuaNames::IInputManager::HIDDEN, static_cast<int64_t>(IInputManager::MouseCursorMode::HIDDEN));
	LuaUtils::pushField(L, LuaNames::IInputManager::DISABLED, static_cast<int64_t>(IInputManager::MouseCursorMode::DISABLED));

	lua_setfield(L, -2, LuaNames::IInputManager::MouseCursorMode);

	lua_createtable(L, 0, 3);

	LuaUtils::pushField(L, LuaNames::IInputManager::LEFT_STICK, IInputManager::LeftStickDeadZone);
	LuaUtils::pushField(L, LuaNames::IInputManager::RIGHT_STICK, IInputManager::RightStickDeadZone);
	LuaUtils::pushField(L, LuaNames::IInputManager::TRIGGER, IInputManager::TriggerDeadZone);

	lua_setfield(L, -2, LuaNames::IInputManager::DeadZone);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaIInputManager::mouseState(lua_State *L)
{
	MouseState *mouseState = const_cast<MouseState *>(&theApplication().inputManager().mouseState());
	LuaUntrackedUserData<MouseState>::push(L, mouseState);
	return 1;
}

int LuaIInputManager::keyboardState(lua_State *L)
{
	KeyboardState *keyboardState = const_cast<KeyboardState *>(&theApplication().inputManager().keyboardState());
	LuaUntrackedUserData<KeyboardState>::push(L, keyboardState);
	return 1;
}

int LuaIInputManager::isJoyPresent(lua_State *L)
{
	const int joyId = LuaUtils::retrieve<int32_t>(L, -1);

	const bool isJoyPresent = theApplication().inputManager().isJoyPresent(joyId);
	LuaUtils::push(L, isJoyPresent);

	return 1;
}

int LuaIInputManager::joyName(lua_State *L)
{
	const int joyId = LuaUtils::retrieve<int32_t>(L, -1);

	const char *joyName = theApplication().inputManager().joyName(joyId);
	LuaUtils::push(L, joyName);

	return 1;
}

int LuaIInputManager::joyGuid(lua_State *L)
{
	const int joyId = LuaUtils::retrieve<int32_t>(L, -1);

	const char *joyGuid = theApplication().inputManager().joyGuid(joyId);
	LuaUtils::push(L, joyGuid);

	return 1;
}

int LuaIInputManager::joyNumButtons(lua_State *L)
{
	const int joyId = LuaUtils::retrieve<int32_t>(L, -1);

	const int numButtons = theApplication().inputManager().joyNumButtons(joyId);
	LuaUtils::push(L, numButtons);

	return 1;
}

int LuaIInputManager::joyNumHats(lua_State *L)
{
	const int joyId = LuaUtils::retrieve<int32_t>(L, -1);

	const int numHats = theApplication().inputManager().joyNumHats(joyId);
	LuaUtils::push(L, numHats);

	return 1;
}

int LuaIInputManager::joyNumAxes(lua_State *L)
{
	const int joyId = LuaUtils::retrieve<int32_t>(L, -1);

	const int numAxes = theApplication().inputManager().joyNumAxes(joyId);
	LuaUtils::push(L, numAxes);

	return 1;
}

int LuaIInputManager::joystickState(lua_State *L)
{
	const int joyId = LuaUtils::retrieve<int32_t>(L, -1);

	JoystickState *joystickState = const_cast<JoystickState *>(&theApplication().inputManager().joystickState(joyId));
	LuaUntrackedUserData<JoystickState>::push(L, joystickState);

	return 1;
}

int LuaIInputManager::isJoyMapped(lua_State *L)
{
	const int joyId = LuaUtils::retrieve<int32_t>(L, -1);

	const bool isJoyMapped = theApplication().inputManager().isJoyMapped(joyId);
	LuaUtils::push(L, isJoyMapped);

	return 1;
}

int LuaIInputManager::joyMappedState(lua_State *L)
{
	const int joyId = LuaUtils::retrieve<int32_t>(L, -1);

	JoyMappedState *joyMappedState = const_cast<JoyMappedState *>(&theApplication().inputManager().joyMappedState(joyId));
	LuaUntrackedUserData<JoyMappedState>::push(L, joyMappedState);

	return 1;
}

int LuaIInputManager::deadZoneNormalize(lua_State *L)
{
	Vector2f joyVector = LuaVector2fUtils::retrieveTable(L, -2);
	const float deadZoneValue = LuaUtils::retrieve<float>(L, -1);

	theApplication().inputManager().deadZoneNormalize(joyVector, deadZoneValue);
	LuaVector2fUtils::push(L, joyVector);

	return 1;
}

int LuaIInputManager::addJoyMappingsFromFile(lua_State *L)
{
	const char *filename = LuaUtils::retrieve<const char *>(L, -1);

	theApplication().inputManager().addJoyMappingsFromFile(filename);

	return 0;
}

int LuaIInputManager::addJoyMappingsFromStrings(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	const unsigned int length = lua_rawlen(L, -1);
	nctl::Array<const char *> mappingStrings(length);

	for (unsigned int i = 0; i < length; i++)
	{
		const int type = lua_rawgeti(L, -1, i + 1);
		if (type != LUA_TSTRING)
		{
			LOGW_X("Expecting a string at index %u of table at index -1", i + 1);
			continue;
		}

		mappingStrings.pushBack(LuaUtils::retrieve<const char *>(L, -1));
		lua_pop(L, 1);
	}
	mappingStrings.pushBack(nullptr);

	theApplication().inputManager().addJoyMappingsFromStrings(mappingStrings.data());

	return 0;
}

int LuaIInputManager::numJoyMappings(lua_State *L)
{
	const int numJoyMappings = theApplication().inputManager().numJoyMappings();
	LuaUtils::push(L, numJoyMappings);

	return 1;
}

int LuaIInputManager::mouseCursorMode(lua_State *L)
{
	const IInputManager::MouseCursorMode mode = theApplication().inputManager().mouseCursorMode();
	LuaUtils::push(L, static_cast<int64_t>(mode));

	return 1;
}

int LuaIInputManager::setMouseCursorMode(lua_State *L)
{
	const IInputManager::MouseCursorMode mode = static_cast<IInputManager::MouseCursorMode>(LuaUtils::retrieve<int64_t>(L, -1));

	theApplication().inputManager().setMouseCursorMode(mode);

	return 0;
}

}
