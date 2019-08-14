#ifndef CLASS_NCINE_LUATIMESTAMP
#define CLASS_NCINE_LUATIMESTAMP

struct lua_State;

namespace ncine {

/// Lua bindings around the `TimeStamp` class
class LuaTimeStamp
{
  public:
	static void expose(lua_State *L);

  private:
	static int now(lua_State *L);

	static int ticksSince(lua_State *L);
	static int secondsSince(lua_State *L);
	static int millisecondsSince(lua_State *L);
	static int microsecondsSince(lua_State *L);
	static int nanosecondsSince(lua_State *L);

	static int seconds(lua_State *L);
	static int milliseconds(lua_State *L);
	static int microseconds(lua_State *L);
	static int nanoseconds(lua_State *L);
};

}

#endif
