#ifndef CLASS_NCINE_LUAIINPUTMANAGER
#define CLASS_NCINE_LUAIINPUTMANAGER

struct lua_State;

namespace ncine {

/// Lua bindings around the `IInputManager` class
class LuaIInputManager
{
  public:
	static void expose(lua_State *L);
	static void exposeConstants(lua_State *L);

  private:
	static int mouseState(lua_State *L);
	static int keyboardState(lua_State *L);

	static int isJoyPresent(lua_State *L);
	static int joyName(lua_State *L);
	static int joyGuid(lua_State *L);
	static int joyNumButtons(lua_State *L);
	static int joyNumHats(lua_State *L);
	static int joyNumAxes(lua_State *L);
	static int joystickState(lua_State *L);

	static int isJoyMapped(lua_State *L);
	static int joyMappedState(lua_State *L);
	static int deadZoneNormalize(lua_State *L);

	static int addJoyMappingsFromFile(lua_State *L);
	static int addJoyMappingsFromStrings(lua_State *L);
	static int numJoyMappings(lua_State *L);

	static int hasJoyVibration(lua_State *L);
	static int joyVibrate(lua_State *L);

	static int mouseCursorMode(lua_State *L);
	static int setMouseCursorMode(lua_State *L);
};

}

#endif
