#ifndef CLASS_NCINE_LUAIGFXDEVICE
#define CLASS_NCINE_LUAIGFXDEVICE

struct lua_State;

namespace ncine {

/// Lua bindings around the `IGfxDevice` class
class LuaIGfxDevice
{
  public:
	static void expose(lua_State *L);

  private:
	static int setSwapInterval(lua_State *L);

	static int isFullScreen(lua_State *L);
	static int setFullScreen(lua_State *L);

	static int isResizable(lua_State *L);

	static int windowPositionX(lua_State *L);
	static int windowPositionY(lua_State *L);
	static int windowPosition(lua_State *L);
	static int setWindowPosition(lua_State *L);

	static int width(lua_State *L);
	static int height(lua_State *L);
	static int resolution(lua_State *L);
	static int aspect(lua_State *L);
	static int setWindowSize(lua_State *L);

	static int drawableWidth(lua_State *L);
	static int drawableHeight(lua_State *L);
	static int drawableResolution(lua_State *L);

	static int setWindowTitle(lua_State *L);
	static int setWindowIcon(lua_State *L);
	static int flashWindow(lua_State *L);

	static int numMonitors(lua_State *L);
	static int primaryMonitorIndex(lua_State *L);
	static int windowMonitorIndex(lua_State *L);
	static int monitor(lua_State *L);

	static int currentVideoMode(lua_State *L);
	static int setVideoMode(lua_State *L);

	static int windowScalingFactor(lua_State *L);
};

}

#endif
