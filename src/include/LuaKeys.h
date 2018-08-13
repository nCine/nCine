#ifndef CLASS_NCINE_LUAKEYS
#define CLASS_NCINE_LUAKEYS

struct lua_State;

namespace ncine {

/// Lua bindings around the `KeySym` and `KeyMod` enums
class LuaKeys
{
  public:
	static void exposeConstants(lua_State *L);
};

}

#endif
