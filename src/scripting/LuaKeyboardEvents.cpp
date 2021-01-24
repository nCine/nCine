#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaKeyboardEvents.h"
#include "LuaClassWrapper.h"
#include "LuaNames.h"
#include "InputEvents.h"

namespace ncine {

namespace LuaNames {
namespace LuaKeyboardEvents {
	static const char *isKeyDown = "key_down";

	static const char *scancode = "scancode";
	static const char *sym = "sym";
	static const char *mod = "mod";

	static const char *text = "text";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaKeyboardEvents::expose(lua_State *L)
{
	// Multiple binding classes adding functions to the same field
	const int type = lua_getfield(L, -1, LuaNames::input);

	if (type == LUA_TNIL)
	{
		lua_pop(L, 1);
		lua_newtable(L);
	}

	LuaUtils::addFunction(L, LuaNames::LuaKeyboardEvents::isKeyDown, isKeyDown);

	lua_setfield(L, -2, LuaNames::input);
}

void LuaKeyboardEvents::pushKeyboardEvent(lua_State *L, const KeyboardEvent &event)
{
	lua_createtable(L, 3, 0);
	LuaUtils::pushField(L, LuaNames::LuaKeyboardEvents::scancode, event.scancode);
	LuaUtils::pushField(L, LuaNames::LuaKeyboardEvents::sym, static_cast<int64_t>(event.sym));
	LuaUtils::pushField(L, LuaNames::LuaKeyboardEvents::mod, event.mod);
}

void LuaKeyboardEvents::pushTextInputEvent(lua_State *L, const TextInputEvent &event)
{
	lua_createtable(L, 1, 0);
	LuaUtils::pushField(L, LuaNames::LuaKeyboardEvents::text, event.text);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaKeyboardEvents::isKeyDown(lua_State *L)
{
	const KeyboardState *state = LuaClassWrapper<KeyboardState>::unwrapUserData(L, -2);
	const KeySym key = static_cast<KeySym>(LuaUtils::retrieve<int64_t>(L, -1));

	lua_pushboolean(L, state->isKeyDown(key));

	return 1;
}

}
