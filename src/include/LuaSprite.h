#ifndef CLASS_NCINE_LUASPRITE
#define CLASS_NCINE_LUASPRITE

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `Sprite` class
class LuaSprite
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void release(void *object);

  private:
	static void exposeFunctions(lua_State *L);
	static int newObject(lua_State *L);

	friend class LuaAnimatedSprite;
};

}

#endif
