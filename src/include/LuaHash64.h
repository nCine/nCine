#ifndef CLASS_NCINE_LUAHASH64
#define CLASS_NCINE_LUAHASH64

struct lua_State;

namespace ncine {

/// Lua bindings around the `Hash64` class
class LuaHash64
{
  public:
	static void expose(lua_State *L);

  private:
	static int hashString(lua_State *L);
	static int hashFileStat(lua_State *L);
	static int scanHashString(lua_State *L);
};

}

#endif
