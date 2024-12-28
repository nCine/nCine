#ifndef CLASS_NCINE_LUAIAPPEVENTHANDLER
#define CLASS_NCINE_LUAIAPPEVENTHANDLER

#include "common_defines.h"

struct lua_State;

namespace nctl {
class String;
}

namespace ncine {

namespace LuaUtils {
	class RunInfo;
}

class AppConfiguration;
class Viewport;

/// Wrapper around the `IAppEventHandler` class
class DLL_PUBLIC LuaIAppEventHandler
{
  public:
	static bool onPreInit(lua_State *L, AppConfiguration &config, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo);
	static bool onInit(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo);
	static bool onFrameStart(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo);
	static bool onPostUpdate(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo);
	static bool onDrawViewport(lua_State *L, Viewport &viewport, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo);
	static bool onFrameEnd(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo);
	static bool onResizeWindow(lua_State *L, int width, int height, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo);
	static bool onChangeScalingFactor(lua_State *L, float factor, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo);
	static bool onShutdown(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo);
	static bool onSuspend(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo);
	static bool onResume(lua_State *L, bool cannotFindWarning, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo);

	static bool onPreInit(lua_State *L, AppConfiguration &config, nctl::String *errorMsg);
	static bool onInit(lua_State *L, nctl::String *errorMsg);
	static bool onFrameStart(lua_State *L, nctl::String *errorMsg);
	static bool onPostUpdate(lua_State *L, nctl::String *errorMsg);
	static bool onDrawViewport(lua_State *L, Viewport &viewport, nctl::String *errorMsg);
	static bool onFrameEnd(lua_State *L, nctl::String *errorMsg);
	static bool onResizeWindow(lua_State *L, int width, int height, nctl::String *errorMsg);
	static bool onChangeScalingFactor(lua_State *L, float factor, nctl::String *errorMsg);
	static bool onShutdown(lua_State *L, nctl::String *errorMsg);
	static bool onSuspend(lua_State *L, nctl::String *errorMsg);
	static bool onResume(lua_State *L, nctl::String *errorMsg);

	static bool onPreInit(lua_State *L, AppConfiguration &config);
	static bool onInit(lua_State *L);
	static bool onFrameStart(lua_State *L);
	static bool onPostUpdate(lua_State *L);
	static bool onDrawViewport(lua_State *L, Viewport &viewport);
	static bool onFrameEnd(lua_State *L);
	static bool onResizeWindow(lua_State *L, int width, int height);
	static bool onChangeScalingFactor(lua_State *L, float factor);
	static bool onShutdown(lua_State *L);
	static bool onSuspend(lua_State *L);
	static bool onResume(lua_State *L);
};

}

#endif
