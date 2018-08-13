#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaDebug.h"

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaDebug::traceError(lua_State *L, const char *errorMsg)
{
	luaL_traceback(L, L, errorMsg, 0);
	lua_error(L);
}

void LuaDebug::assert(lua_State *L, bool condition, const char *errorMsg)
{
	if (condition == false)
	{
		luaL_traceback(L, L, errorMsg, 0);
		lua_error(L);
	}
}

const char *LuaDebug::errorToSting(int value)
{
	switch (value)
	{
		case LUA_OK: return "Success (LUA_OK)";
		case LUA_YIELD: return "Thread is suspended (LUA_YIELD)";
		case LUA_ERRRUN: return "Run-time error (LUA_ERRRUN)";
		case LUA_ERRSYNTAX: return "Syntax error during precompilation (LUA_ERRSYNTAX)";
		case LUA_ERRMEM: return "Memory allocation error (LUA_ERRMEM)";
		case LUA_ERRGCMM: return "Error while running a __gc metamethod (LUA_ERRGCMM)";
		case LUA_ERRERR: return "Error while running the message handler (LUA_ERRERR)";
		default:
			return "No error";
	}
}

void LuaDebug::printStackValue(lua_State *L, int index)
{
	const int type = lua_type(L, index);

	switch (type)
	{
		case LUA_TNIL:
			LOGI_X("Lua value at index %d is nil", index);
			break;
		case LUA_TBOOLEAN:
			LOGI_X("Lua value at index %d is a boolean: %s", index, lua_toboolean(L, index) ? "true" : "false");
			break;
		case LUA_TLIGHTUSERDATA:
			LOGI_X("Lua value at index %d is a light userdata: %s", index, lua_typename(L, index));
			break;
		case LUA_TNUMBER:
			if (lua_isinteger(L, index))
				LOGI_X("Lua value at index %d is an integer: %lld", index, lua_tointeger(L, index));
			else
				LOGI_X("Lua value at index %d is a number: %g", index, lua_tonumber(L, index));
			break;
		case LUA_TSTRING:
			LOGI_X("Lua value at index %d is a string: %s", index, lua_tostring(L, index));
			break;
		case LUA_TTABLE:
			LOGI_X("Lua value at index %d is a table: %s", index, lua_typename(L, index));
			break;
		case LUA_TFUNCTION:
			if (lua_iscfunction(L, index))
				LOGI_X("Lua value at index %d is a C function: %s", index, lua_typename(L, index));
			else
				LOGI_X("Lua value at index %d is a Lua function: %s", index, lua_typename(L, index));
			break;
		case LUA_TUSERDATA:
			LOGI_X("Lua value at index %d is an userdata: %s", index, lua_typename(L, index));
			break;
		case LUA_TTHREAD:
			LOGI_X("Lua value at index %d is a thread: %s", index, lua_typename(L, index));
			break;
		default:
			break;
	}
}

}
