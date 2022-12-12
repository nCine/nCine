#include "LuaIAppEventHandler.h"
#include "common_macros.h"

#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaDebug.h"
#include "LuaAppConfiguration.h"
#include "LuaNames.h"
#include "LuaUtils.h"

#include "tracy.h"

namespace ncine {

namespace LuaNames {
namespace LuaIAppEventHandler {
	static const char *onPreInit = "on_pre_init";
	static const char *onInit = "on_init";
	static const char *onFrameStart = "on_frame_start";
	static const char *onPostUpdate = "on_post_update";
	static const char *onDrawViewport = "on_draw_viewport";
	static const char *onFrameEnd = "on_frame_end";
	static const char *onResizeWindow = "on_resize_window";
	static const char *onChangeScalingFactor = "on_change_scaling_factor";
	static const char *onShutdown = "on_shutdown";
	static const char *onSuspend = "on_suspend";
	static const char *onResume = "on_resume";
}}

namespace {
	void callFunction(lua_State *L, const char *functionName, bool cannotFindWarning)
	{
		lua_getglobal(L, LuaNames::ncine);
		const int type = lua_getfield(L, -1, functionName);

		if (type == LUA_TFUNCTION)
		{
			const int status = lua_pcall(L, 0, 0, 0);
			if (status != LUA_OK)
			{
				LOGE_X("Error running Lua function \"%s\" (%s):\n%s", functionName, LuaDebug::statusToString(status), lua_tostring(L, -1));
				lua_pop(L, 1);
			}
		}
		else
		{
			lua_pop(L, 2);
			if (cannotFindWarning)
				LOGW_X("Cannot find the Lua function \"%s\"", functionName);
		}
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaIAppEventHandler::onPreInit(lua_State *L, AppConfiguration &config)
{
	ZoneScopedN("Lua onPreInit");
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onPreInit);

	if (type == LUA_TFUNCTION)
	{
		LuaAppConfiguration::push(L, config);
		const int status = lua_pcall(L, 1, 1, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::LuaIAppEventHandler::onPreInit, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
		else
			LuaAppConfiguration::retrieveAndSet(L, config);
	}
	else
	{
		lua_pop(L, 2);
		LOGW_X("Cannot find the Lua function \"%s\"", LuaNames::LuaIAppEventHandler::onPreInit);
	}
}

void LuaIAppEventHandler::onInit(lua_State *L)
{
	ZoneScopedN("Lua onInit");
	callFunction(L, LuaNames::LuaIAppEventHandler::onInit, true);
}

void LuaIAppEventHandler::onFrameStart(lua_State *L)
{
	ZoneScopedN("Lua onFrameStart");
	callFunction(L, LuaNames::LuaIAppEventHandler::onFrameStart, false);
}

void LuaIAppEventHandler::onPostUpdate(lua_State *L)
{
	ZoneScopedN("Lua onPostUpdate");
	callFunction(L, LuaNames::LuaIAppEventHandler::onPostUpdate, false);
}

void LuaIAppEventHandler::onDrawViewport(lua_State *L, Viewport &viewport)
{
	ZoneScopedN("Lua onDrawViewport");
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onDrawViewport);

	if (type == LUA_TFUNCTION)
	{
		LuaUtils::push(L, static_cast<void *>(&viewport));
		const int status = lua_pcall(L, 1, 1, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::LuaIAppEventHandler::onDrawViewport, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
	{
		lua_pop(L, 2);
		LOGW_X("Cannot find the Lua function \"%s\"", LuaNames::LuaIAppEventHandler::onDrawViewport);
	}
}

void LuaIAppEventHandler::onFrameEnd(lua_State *L)
{
	ZoneScopedN("Lua onFrameEnd");
	callFunction(L, LuaNames::LuaIAppEventHandler::onFrameEnd, false);
}

void LuaIAppEventHandler::onResizeWindow(lua_State *L, int width, int height)
{
	ZoneScopedN("Lua onResizeWindow");
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onResizeWindow);

	if (type == LUA_TFUNCTION)
	{
		LuaUtils::push(L, width);
		LuaUtils::push(L, height);
		const int status = lua_pcall(L, 2, 1, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::LuaIAppEventHandler::onResizeWindow, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
	{
		lua_pop(L, 2);
		LOGW_X("Cannot find the Lua function \"%s\"", LuaNames::LuaIAppEventHandler::onResizeWindow);
	}
}

void LuaIAppEventHandler::onChangeScalingFactor(lua_State *L, float factor)
{
	ZoneScopedN("Lua onChangeScalingFactor");
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onChangeScalingFactor);

	if (type == LUA_TFUNCTION)
	{
		LuaUtils::push(L, factor);
		const int status = lua_pcall(L, 1, 1, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::LuaIAppEventHandler::onChangeScalingFactor, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
	{
		lua_pop(L, 2);
		LOGW_X("Cannot find the Lua function \"%s\"", LuaNames::LuaIAppEventHandler::onChangeScalingFactor);
	}
}

void LuaIAppEventHandler::onShutdown(lua_State *L)
{
	ZoneScopedN("Lua onShutdown");
	callFunction(L, LuaNames::LuaIAppEventHandler::onShutdown, true);
}

void LuaIAppEventHandler::onSuspend(lua_State *L)
{
	ZoneScopedN("Lua onSuspend");
	callFunction(L, LuaNames::LuaIAppEventHandler::onSuspend, true);
}

void LuaIAppEventHandler::onResume(lua_State *L)
{
	ZoneScopedN("Lua onResume");
	callFunction(L, LuaNames::LuaIAppEventHandler::onResume, true);
}

}
