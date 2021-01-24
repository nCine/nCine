#ifndef CLASS_NCINE_LUAIINPUTEVENTHANDLER
#define CLASS_NCINE_LUAIINPUTEVENTHANDLER

#include "common_defines.h"

struct lua_State;

namespace ncine {

class KeyboardEvent;
class TextInputEvent;
class MouseEvent;
class MouseState;
class ScrollEvent;
class JoyButtonEvent;
class JoyHatEvent;
class JoyAxisEvent;
class JoyMappedButtonEvent;
class JoyMappedAxisEvent;
class JoyConnectionEvent;
class TouchEvent;
class AccelerometerEvent;

/// Wrapper around the `IInputEventHandler` class
class DLL_PUBLIC LuaIInputEventHandler
{
  public:
	static void onKeyPressed(lua_State *L, const KeyboardEvent &event);
	static void onKeyReleased(lua_State *L, const KeyboardEvent &event);
	static void onTextInput(lua_State *L, const TextInputEvent &event);

#ifdef __ANDROID__
	static void onTouchDown(lua_State *L, const TouchEvent &event);
	static void onTouchUp(lua_State *L, const TouchEvent &event);
	static void onTouchMove(lua_State *L, const TouchEvent &event);
	static void onPointerDown(lua_State *L, const TouchEvent &event);
	static void onPointerUp(lua_State *L, const TouchEvent &event);
	static void onAcceleration(lua_State *L, const AccelerometerEvent &event);
#endif

	static void onMouseButtonPressed(lua_State *L, const MouseEvent &event);
	static void onMouseButtonReleased(lua_State *L, const MouseEvent &event);
	static void onMouseMoved(lua_State *L, const MouseState &state);
	static void onScrollInput(lua_State *L, const ScrollEvent &event);

	static void onJoyButtonPressed(lua_State *L, const JoyButtonEvent &event);
	static void onJoyButtonReleased(lua_State *L, const JoyButtonEvent &event);
	static void onJoyHatMoved(lua_State *L, const JoyHatEvent &event);
	static void onJoyAxisMoved(lua_State *L, const JoyAxisEvent &event);

	static void onJoyMappedButtonPressed(lua_State *L, const JoyMappedButtonEvent &event);
	static void onJoyMappedButtonReleased(lua_State *L, const JoyMappedButtonEvent &event);
	static void onJoyMappedAxisMoved(lua_State *L, const JoyMappedAxisEvent &event);

	static void onJoyConnected(lua_State *L, const JoyConnectionEvent &event);
	static void onJoyDisconnected(lua_State *L, const JoyConnectionEvent &event);
};

}

#endif
