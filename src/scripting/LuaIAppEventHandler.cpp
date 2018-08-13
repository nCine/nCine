#include "LuaIAppEventHandler.h"
#include "common_macros.h"

#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaUtils.h"
#include "LuaAppConfiguration.h"
#include "LuaNames.h"

namespace ncine {

namespace LuaNames {
namespace LuaIAppEventHandler
{
	static const char *onPreInit = "on_pre_init";
	static const char *onInit = "on_init";
	static const char *onFrameStart = "on_frame_start";
	static const char *onShutdown = "on_shutdown";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaIAppEventHandler::onPreInit(lua_State *L, AppConfiguration &config)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onPreInit);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaAppConfiguration::push(L, config);
		lua_call(L, 1, 1);
		LuaAppConfiguration::retrieveAndSet(L, config);
	}
	else
		LOGW("Cannot find the Lua function");
}

void LuaIAppEventHandler::onInit(lua_State *L)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onInit);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		lua_call(L, 0, 0);
	}
	else
		LOGW("Cannot find the Lua function");
}

void LuaIAppEventHandler::onFrameStart(lua_State *L)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onFrameStart);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		lua_call(L, 0, 0);
	}
}

void LuaIAppEventHandler::onShutdown(lua_State *L)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onShutdown);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		lua_call(L, 0, 0);
	}
	else
		LOGW("Cannot find the Lua function");
}

}
