#ifndef CLASS_NCINE_LUAAUDIOFILTER
#define CLASS_NCINE_LUAAUDIOFILTER

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `AudioFilter` class
class LuaAudioFilter
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void exposeConstants(lua_State *L);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);

	static int type(lua_State *L);
	static int filterId(lua_State *L);
	static int applyProperties(lua_State *L);

	static int defaultProperties(lua_State *L);
};

}

#endif
