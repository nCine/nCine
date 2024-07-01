#ifndef CLASS_NCINE_LUAAUDIOEFFECT
#define CLASS_NCINE_LUAAUDIOEFFECT

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `AudioEffect` class
class LuaAudioEffect
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void exposeConstants(lua_State *L);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);

	static int type(lua_State *L);
	static int effectId(lua_State *L);
	static int applyProperties(lua_State *L);
};

}

#endif
