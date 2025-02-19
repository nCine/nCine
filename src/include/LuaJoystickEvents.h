#ifndef CLASS_NCINE_LUAJOYSTICKEVENTS
#define CLASS_NCINE_LUAJOYSTICKEVENTS

struct lua_State;

namespace ncine {

class JoyButtonEvent;
class JoyHatEvent;
class JoyAxisEvent;
class JoyMappedButtonEvent;
class JoyMappedAxisEvent;
class JoyConnectionEvent;

/// Wrapper around the `JoystickEvent` and `JoystickState` classes
class LuaJoystickEvents
{
  public:
	static void expose(lua_State *L);
	static void exposeConstants(lua_State *L);

	static void pushJoyButtonEvent(lua_State *L, const JoyButtonEvent &event);
	static void pushJoyHatEvent(lua_State *L, const JoyHatEvent &event);
	static void pushJoyAxisEvent(lua_State *L, const JoyAxisEvent &event);
	static void pushJoyMappedButtonEvent(lua_State *L, const JoyMappedButtonEvent &event);
	static void pushJoyMappedAxisEvent(lua_State *L, const JoyMappedAxisEvent &event);
	static void pushJoyConnectionEvent(lua_State *L, const JoyConnectionEvent &event);

  private:
	static int isButtonDown(lua_State *L);
	static int isButtonPressed(lua_State *L);
	static int isButtonReleased(lua_State *L);

	static int hatState(lua_State *L);
	static int axisValue(lua_State *L);
	static int axisNormValue(lua_State *L);
};

}

#endif
