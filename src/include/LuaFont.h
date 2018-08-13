#ifndef CLASS_NCINE_LUAFONT
#define CLASS_NCINE_LUAFONT

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `Font` class
class LuaFont
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);

	static int texture(lua_State *L);

	static int lineHeight(lua_State *L);
	static int base(lua_State *L);
	static int textureSize(lua_State *L);
	static int numGlyphs(lua_State *L);
};

}

#endif
