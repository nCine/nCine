#ifndef CLASS_NCINE_LUAIFRAMETIMER
#define CLASS_NCINE_LUAIFRAMETIMER

struct lua_State;

namespace ncine {

/// Lua bindings around the `IFrameTimer` class
class LuaIFrameTimer
{
  public:
	static void expose(lua_State *L);

  private:
	static int totalNumberFrames(lua_State *L);
	static int lastFrameTime(lua_State *L);
	static int currentFrameTime(lua_State *L);

	static int averageFps(lua_State *L);
	static int averageFrameTime(lua_State *L);

	static int averageEnabled(lua_State *L);
	static int loggingEnabled(lua_State *L);

	static int averageInterval(lua_State *L);
	static int setAverageInterval(lua_State *L);

	static int loggingInterval(lua_State *L);
	static int setLoggingInterval(lua_State *L);

	static int logLevel(lua_State *L);
	static int setLogLevel(lua_State *L);
};

}

#endif
