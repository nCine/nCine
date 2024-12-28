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
	/*! \note Returns true if the callback function has been found. */
	bool callFunction(lua_State *L, const char *functionName, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
	{
		lua_getglobal(L, LuaNames::ncine);
		const int type = lua_getfield(L, -1, functionName);

		bool functionFound = false;
		if (type == LUA_TFUNCTION)
		{
			const int callStatus = LuaUtils::pcall(L, 0, 0, runInfo);
			if (status)
				*status = callStatus;

			if (callStatus != LUA_OK)
			{
				LOGE_X("Error running Lua function \"%s\" (%s):\n%s", functionName, LuaDebug::statusToString(callStatus), lua_tostring(L, -1));
				if (errorMsg)
					*errorMsg = lua_tostring(L, -1);
				lua_pop(L, 1);
			}
			else if (errorMsg)
				errorMsg->clear();
			functionFound = true;
		}
		else
		{
			lua_pop(L, 2);
			if (cannotFindWarning)
				LOGD_X("Cannot find the Lua function \"%s\"", functionName);
		}
		return functionFound;
	}

	bool callFunction(lua_State *L, const char *functionName, bool cannotFindWarning, nctl::String *errorMsg)
	{
		return callFunction(L, functionName, cannotFindWarning, errorMsg, nullptr, nullptr);
	}

	bool callFunction(lua_State *L, const char *functionName, bool cannotFindWarning)
	{
		return callFunction(L, functionName, cannotFindWarning, nullptr, nullptr, nullptr);
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool LuaIAppEventHandler::onPreInit(lua_State *L, AppConfiguration &config, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
{
	ZoneScopedN("Lua onPreInit");
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onPreInit);

	bool functionFound = false;
	if (type == LUA_TFUNCTION)
	{
		LuaAppConfiguration::push(L, config);
		const int callStatus = LuaUtils::pcall(L, 1, 1, runInfo);
		if (status)
			*status = callStatus;

		if (callStatus != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::LuaIAppEventHandler::onPreInit, LuaDebug::statusToString(callStatus), lua_tostring(L, -1));
			if (errorMsg)
				*errorMsg = lua_tostring(L, -1);
			lua_pop(L, 1);
		}
		else
		{
			LuaAppConfiguration::retrieveAndSet(L, config);
			if (errorMsg)
				errorMsg->clear();
		}
		functionFound = true;
	}
	else
	{
		lua_pop(L, 2);
		if (cannotFindWarning)
			LOGD_X("Cannot find the Lua function \"%s\"", LuaNames::LuaIAppEventHandler::onPreInit);
	}
	return functionFound;
}

bool LuaIAppEventHandler::onInit(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
{
	ZoneScopedN("Lua onInit");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onInit, cannotFindWarning, errorMsg, status, runInfo);
}

bool LuaIAppEventHandler::onFrameStart(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
{
	ZoneScopedN("Lua onFrameStart");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onFrameStart, cannotFindWarning, errorMsg, status, runInfo);
}

bool LuaIAppEventHandler::onPostUpdate(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
{
	ZoneScopedN("Lua onPostUpdate");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onPostUpdate, cannotFindWarning, errorMsg, status, runInfo);
}

bool LuaIAppEventHandler::onDrawViewport(lua_State *L, Viewport &viewport, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
{
	ZoneScopedN("Lua onDrawViewport");
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onDrawViewport);

	bool functionFound = false;
	if (type == LUA_TFUNCTION)
	{
		LuaUtils::push(L, static_cast<void *>(&viewport));
		const int callStatus = LuaUtils::pcall(L, 1, 1, runInfo);
		if (status)
			*status = callStatus;

		if (callStatus != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::LuaIAppEventHandler::onDrawViewport, LuaDebug::statusToString(callStatus), lua_tostring(L, -1));
			if (errorMsg)
				*errorMsg = lua_tostring(L, -1);
			lua_pop(L, 1);
		}
		else if (errorMsg)
			errorMsg->clear();
		functionFound = true;
	}
	else
	{
		lua_pop(L, 2);
		if (cannotFindWarning)
			LOGD_X("Cannot find the Lua function \"%s\"", LuaNames::LuaIAppEventHandler::onDrawViewport);
	}
	return functionFound;
}

bool LuaIAppEventHandler::onFrameEnd(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
{
	ZoneScopedN("Lua onFrameEnd");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onFrameEnd, cannotFindWarning, errorMsg, status, runInfo);
}

bool LuaIAppEventHandler::onResizeWindow(lua_State *L, int width, int height, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
{
	ZoneScopedN("Lua onResizeWindow");
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onResizeWindow);

	bool functionFound = false;
	if (type == LUA_TFUNCTION)
	{
		LuaUtils::push(L, width);
		LuaUtils::push(L, height);
		const int callStatus = LuaUtils::pcall(L, 2, 1, runInfo);
		if (status)
			*status = callStatus;

		if (callStatus != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::LuaIAppEventHandler::onResizeWindow, LuaDebug::statusToString(callStatus), lua_tostring(L, -1));
			if (errorMsg)
				*errorMsg = lua_tostring(L, -1);
			lua_pop(L, 1);
		}
		else if (errorMsg)
			errorMsg->clear();
		functionFound = true;
	}
	else
	{
		lua_pop(L, 2);
		if (cannotFindWarning)
			LOGD_X("Cannot find the Lua function \"%s\"", LuaNames::LuaIAppEventHandler::onResizeWindow);
	}
	return functionFound;
}

bool LuaIAppEventHandler::onChangeScalingFactor(lua_State *L, float factor, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
{
	ZoneScopedN("Lua onChangeScalingFactor");
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::LuaIAppEventHandler::onChangeScalingFactor);

	bool functionFound = false;
	if (type == LUA_TFUNCTION)
	{
		LuaUtils::push(L, factor);
		const int callStatus = LuaUtils::pcall(L, 1, 1, runInfo);
		if (status)
			*status = callStatus;

		if (callStatus != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::LuaIAppEventHandler::onChangeScalingFactor, LuaDebug::statusToString(callStatus), lua_tostring(L, -1));
			if (errorMsg)
				*errorMsg = lua_tostring(L, -1);
			lua_pop(L, 1);
		}
		else if (errorMsg)
			errorMsg->clear();
		functionFound = true;
	}
	else
	{
		lua_pop(L, 2);
		if (cannotFindWarning)
			LOGI_X("Cannot find the Lua function \"%s\"", LuaNames::LuaIAppEventHandler::onChangeScalingFactor);
	}
	return functionFound;
}

bool LuaIAppEventHandler::onShutdown(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
{
	ZoneScopedN("Lua onShutdown");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onShutdown, cannotFindWarning, errorMsg, status, runInfo);
}

bool LuaIAppEventHandler::onSuspend(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
{
	ZoneScopedN("Lua onSuspend");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onSuspend, cannotFindWarning, errorMsg, status, runInfo);
}

bool LuaIAppEventHandler::onResume(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
{
	ZoneScopedN("Lua onResume");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onResume, cannotFindWarning, errorMsg, status, runInfo);
}

bool LuaIAppEventHandler::onPreInit(lua_State *L, AppConfiguration &config, nctl::String *errorMsg)
{
	return onPreInit(L, config, true, errorMsg, nullptr, nullptr);
}

bool LuaIAppEventHandler::onInit(lua_State *L, nctl::String *errorMsg)
{
	ZoneScopedN("Lua onInit");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onInit, true, errorMsg);
}

bool LuaIAppEventHandler::onFrameStart(lua_State *L, nctl::String *errorMsg)
{
	ZoneScopedN("Lua onFrameStart");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onFrameStart, false, errorMsg);
}

bool LuaIAppEventHandler::onPostUpdate(lua_State *L, nctl::String *errorMsg)
{
	ZoneScopedN("Lua onPostUpdate");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onPostUpdate, false, errorMsg);
}

bool LuaIAppEventHandler::onDrawViewport(lua_State *L, Viewport &viewport, nctl::String *errorMsg)
{
	return onDrawViewport(L, viewport, true, errorMsg, nullptr, nullptr);
}

bool LuaIAppEventHandler::onFrameEnd(lua_State *L, nctl::String *errorMsg)
{
	ZoneScopedN("Lua onFrameEnd");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onFrameEnd, false, errorMsg);
}

bool LuaIAppEventHandler::onResizeWindow(lua_State *L, int width, int height, nctl::String *errorMsg)
{
	return onResizeWindow(L, width, height, true, errorMsg, nullptr, nullptr);
}

bool LuaIAppEventHandler::onChangeScalingFactor(lua_State *L, float factor, nctl::String *errorMsg)
{
	return onChangeScalingFactor(L, factor, true, errorMsg, nullptr, nullptr);
}

bool LuaIAppEventHandler::onShutdown(lua_State *L, nctl::String *errorMsg)
{
	ZoneScopedN("Lua onShutdown");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onShutdown, true, errorMsg);
}

bool LuaIAppEventHandler::onSuspend(lua_State *L, nctl::String *errorMsg)
{
	ZoneScopedN("Lua onSuspend");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onSuspend, true, errorMsg);
}

bool LuaIAppEventHandler::onResume(lua_State *L, nctl::String *errorMsg)
{
	ZoneScopedN("Lua onResume");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onResume, true, errorMsg);
}

bool LuaIAppEventHandler::onPreInit(lua_State *L, AppConfiguration &config)
{
	return onPreInit(L, config, true, nullptr, nullptr, nullptr);
}

bool LuaIAppEventHandler::onInit(lua_State *L)
{
	ZoneScopedN("Lua onInit");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onInit, true);
}

bool LuaIAppEventHandler::onFrameStart(lua_State *L)
{
	ZoneScopedN("Lua onFrameStart");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onFrameStart, false);
}

bool LuaIAppEventHandler::onPostUpdate(lua_State *L)
{
	ZoneScopedN("Lua onPostUpdate");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onPostUpdate, false);
}

bool LuaIAppEventHandler::onDrawViewport(lua_State *L, Viewport &viewport)
{
	return onDrawViewport(L, viewport, true, nullptr, nullptr, nullptr);
}

bool LuaIAppEventHandler::onFrameEnd(lua_State *L)
{
	ZoneScopedN("Lua onFrameEnd");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onFrameEnd, false);
}

bool LuaIAppEventHandler::onResizeWindow(lua_State *L, int width, int height)
{
	return onResizeWindow(L, width, height, true, nullptr, nullptr, nullptr);
}

bool LuaIAppEventHandler::onChangeScalingFactor(lua_State *L, float factor)
{
	return onChangeScalingFactor(L, factor, true, nullptr, nullptr, nullptr);
}

bool LuaIAppEventHandler::onShutdown(lua_State *L)
{
	ZoneScopedN("Lua onShutdown");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onShutdown, true);
}

bool LuaIAppEventHandler::onSuspend(lua_State *L)
{
	ZoneScopedN("Lua onSuspend");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onSuspend, true);
}

bool LuaIAppEventHandler::onResume(lua_State *L)
{
	ZoneScopedN("Lua onResume");
	return callFunction(L, LuaNames::LuaIAppEventHandler::onResume, true);
}

}
