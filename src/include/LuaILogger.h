#ifndef CLASS_NCINE_LUAILOGGER
#define CLASS_NCINE_LUAILOGGER

struct lua_State;

namespace ncine {

/// Lua bindings around the `ILogger` class
class LuaILogger
{
  public:
	static void expose(lua_State *L);
	static void exposeConstants(lua_State *L);

  private:
	static int logVerbose(lua_State *L);
	static int logDebug(lua_State *L);
	static int logInfo(lua_State *L);
	static int logWarn(lua_State *L);
	static int logError(lua_State *L);
	static int logFatal(lua_State *L);
};

}

#endif
