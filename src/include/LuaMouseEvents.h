#ifndef CLASS_NCINE_LUAMOUSEEVENTS
#define CLASS_NCINE_LUAMOUSEEVENTS

struct lua_State;

namespace ncine {

class MouseEvent;
class MouseState;
class ScrollEvent;

/// Wrapper around the `MouseEvent`, `MouseState` and `ScrollEvent` classes
class LuaMouseEvents
{
  public:
	static void expose(lua_State *L);
	static void exposeConstants(lua_State *L);

	static void pushMouseEvent(lua_State *L, const MouseEvent &event);
	static void pushMouseState(lua_State *L, const MouseState &state);
	static void pushScrollEvent(lua_State *L, const ScrollEvent &event);

  private:
	static int isButtonDown(lua_State *L);
	static int isButtonPressed(lua_State *L);
	static int isButtonReleased(lua_State *L);
};

}

#endif
