#ifndef CLASS_NCINE_LUAAPPLICATION
#define CLASS_NCINE_LUAAPPLICATION

struct lua_State;

namespace ncine {

/// Lua bindings around the `Application` class
class LuaApplication
{
  public:
	static void expose(lua_State *L);

  private:
	static int appConfiguration(lua_State *L);

	static int renderingSettings(lua_State *L);
	static int setRenderingSettings(lua_State *L);

	static int guiSettings(lua_State *L);
	static int setGuiSettings(lua_State *L);

	static int debugOverlaySettings(lua_State *L);
	static int setDebugOverlaySettings(lua_State *L);

	static int rootNode(lua_State *L);
	static int screenViewport(lua_State *L);
	static int numFrames(lua_State *L);
	static int frameTime(lua_State *L);

	static int width(lua_State *L);
	static int height(lua_State *L);
	static int resolution(lua_State *L);

	static int isSuspended(lua_State *L);
	static int setSuspended(lua_State *L);

	static int autoSuspension(lua_State *L);
	static int setAutoSuspension(lua_State *L);

	static int quit(lua_State *L);
};

}

#endif
