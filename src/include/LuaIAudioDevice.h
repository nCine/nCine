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
	static int hasEfxExtension(lua_State *L);

	static int gain(lua_State *L);
	static int setGain(lua_State *L);

	static int position(lua_State *L);
	static int setPosition(lua_State *L);

	static int velocity(lua_State *L);
	static int setVelocity(lua_State *L);

	static int maxNumSources(lua_State *L);
	static int numAvailableSources(lua_State *L);

	static int numPlayers(lua_State *L);
	static int player(lua_State *L);

	static int pausePlayers(lua_State *L);
	static int stopPlayers(lua_State *L);
	static int resumePlayers(lua_State *L);

	static int pauseDevice(lua_State *L);
	static int resumeDevice(lua_State *L);
};

}

#endif
