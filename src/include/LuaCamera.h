#ifndef CLASS_NCINE_LUACAMERA
#define CLASS_NCINE_LUACAMERA

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `Camera` class
class LuaCamera
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);

	static int projectionValues(lua_State *L);
	static int viewValues(lua_State *L);

	static int setOrthoProjection(lua_State *L);
	static int setView(lua_State *L);
};

}

#endif
