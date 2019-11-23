#include "LuaIAppEventHandler.h"
#include "common_macros.h"

#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaUtils.h"
#include "LuaAppConfiguration.h"
#include "LuaNames.h"

#include "tracy.h"

namespace ncine {

namespace LuaNames {
namespace LuaIAppEventHandler {
	static const char *onPreInit = "on_pre_init";
	static const char *onInit = "on_init";
	static const char *onFrameStart = "on_frame_start";
	static const char *onShutdown = "on_shutdown";
	static const char *onSuspend = "on_suspend";
	static const char *onResume = "on_resume";
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
		LOGW_X("Cannot find the Lua \"%s\" function", LuaNames::LuaIAppEventHandler::onPreInit);
}

void LuaIAppEventHandler::onInit(lua_State *L)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onInit);

	if (type != LUA_TNIL)
	{
		ZoneScopedN("Lua onInit");
		ASSERT(type == LUA_TFUNCTION);
		lua_call(L, 0, 0);
	}
	else
		LOGW_X("Cannot find the Lua \"%s\" function", LuaNames::LuaIAppEventHandler::onInit);
}

void LuaIAppEventHandler::onFrameStart(lua_State *L)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onFrameStart);

	if (type != LUA_TNIL)
	{
		ZoneScopedN("Lua onFrameStart");
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
		ZoneScopedN("Lua onShutdown");
		ASSERT(type == LUA_TFUNCTION);
		lua_call(L, 0, 0);
	}
	else
		LOGW_X("Cannot find the Lua \"%s\" function", LuaNames::LuaIAppEventHandler::onShutdown);
}

void LuaIAppEventHandler::onSuspend(lua_State *L)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onSuspend);

	if (type != LUA_TNIL)
	{
		ZoneScopedN("Lua onSuspend");
		ASSERT(type == LUA_TFUNCTION);
		lua_call(L, 0, 0);
	}
	else
		LOGW_X("Cannot find the Lua \"%s\" function", LuaNames::LuaIAppEventHandler::onSuspend);
}

void LuaIAppEventHandler::onResume(lua_State *L)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onResume);

	if (type != LUA_TNIL)
	{
		ZoneScopedN("Lua onResume");
		ASSERT(type == LUA_TFUNCTION);
		lua_call(L, 0, 0);
	}
	else
		LOGW_X("Cannot find the Lua \"%s\" function", LuaNames::LuaIAppEventHandler::onResume);
}

}
