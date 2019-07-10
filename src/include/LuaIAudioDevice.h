#ifndef CLASS_NCINE_LUAIAUDIODEVICE
#define CLASS_NCINE_LUAIAUDIODEVICE

struct lua_State;

namespace ncine {

/// Lua bindings around the `IAudioDevice` class
class LuaIAudioDevice
{
  public:
	static void expose(lua_State *L);

  private:
	static int name(lua_State *L);

	static int gain(lua_State *L);
	static int setGain(lua_State *L);

	static int numPlayers(lua_State *L);
	static int player(lua_State *L);

	static int stopPlayers(lua_State *L);
	static int pausePlayers(lua_State *L);

	static int freezePlayers(lua_State *L);
	static int unfreezePlayers(lua_State *L);
};

}

#endif
