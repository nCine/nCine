#ifndef CLASS_NCINE_LUAAPPCONFIGURATION
#define CLASS_NCINE_LUAAPPCONFIGURATION

struct lua_State;

namespace ncine {

class AppConfiguration;

/// Lua bindings around the `AppConfiguration` class
class LuaAppConfiguration
{
  public:
	static void exposeConstants(lua_State *L);
	static void push(lua_State *L, const AppConfiguration &appCfg);
	static void retrieveAndSet(lua_State *L, AppConfiguration &appCfg);
};

}

#endif
