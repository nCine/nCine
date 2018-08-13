#ifndef CLASS_NCINE_LUACOLOR
#define CLASS_NCINE_LUACOLOR

struct lua_State;

namespace ncine {

class Color;

/// Lua bindings around the `Color` class
class LuaColor
{
  public:
	static void expose(lua_State *L);

  private:
	static int create(lua_State *L);

	static int multiply(lua_State *L);
};

}

#endif
