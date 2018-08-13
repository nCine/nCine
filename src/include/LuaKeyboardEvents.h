#ifndef CLASS_NCINE_LUAKEYBOARDEVENTS
#define CLASS_NCINE_LUAKEYBOARDEVENTS

struct lua_State;

namespace ncine {

class KeyboardEvent;

/// Wrapper around the `KeyboardEvent` and `KeyboardState` classes
class LuaKeyboardEvents
{
  public:
	static void expose(lua_State *L);

	static void pushKeyboardEvent(lua_State *L, const KeyboardEvent &event);

  private:
	static int isKeyDown(lua_State *L);
};

}

#endif
