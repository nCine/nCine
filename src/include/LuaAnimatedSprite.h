#ifndef CLASS_NCINE_LUAANIMATEDSPRITE
#define CLASS_NCINE_LUAANIMATEDSPRITE

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `AnimatedSprite` class
class LuaAnimatedSprite
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);
	static int cloneNode(lua_State *L);

	static int isPaused(lua_State *L);
	static int setPaused(lua_State *L);

	static int addAnimation(lua_State *L);
	static int clearAnimations(lua_State *L);

	static int numAnimations(lua_State *L);
	static int animationIndex(lua_State *L);
	static int setAnimationIndex(lua_State *L);

	static int frame(lua_State *L);
	static int setFrame(lua_State *L);
};

}

#endif
