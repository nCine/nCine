#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaIInputEventHandler.h"
#include "LuaClassWrapper.h"
#include "LuaKeyboardEvents.h"
#include "LuaMouseEvents.h"
#include "LuaJoystickEvents.h"
#include "LuaUtils.h"
#include "LuaNames.h"
#include "InputEvents.h"

#ifdef __ANDROID__
	#include "LuaAndroidEvents.h"
#endif

namespace ncine {

namespace LuaNames {
namespace IInputEventHandler {
	static const char *onKeyPressed = "on_key_pressed";
	static const char *onKeyReleased = "on_key_released";

#ifdef __ANDROID__
	static const char *onTouchDown = "on_touch_down";
	static const char *onTouchUp = "on_touch_up";
	static const char *onTouchMove = "on_touch_move";
	static const char *onPointerDown = "on_pointer_down";
	static const char *onPointerUp = "on_pointer_up";
	static const char *onAcceleration = "on_acceleration";
#endif

	static const char *onMouseButtonPressed = "on_mouse_button_pressed";
	static const char *onMouseButtonReleased = "on_mouse_button_released";
	static const char *onMouseMoved = "on_mouse_moved";
	static const char *onScrollInput = "on_scroll_input";

	static const char *onJoyButtonPressed = "on_joy_button_pressed";
	static const char *onJoyButtonReleased = "on_joy_button_released";
	static const char *onJoyHatMoved = "on_joy_hat_moved";
	static const char *onJoyAxisMoved = "on_joy_axis_moved";
	static const char *onJoyMappedButtonPressed = "on_joymapped_button_pressed";
	static const char *onJoyMappedButtonReleased = "on_joymapped_button_released";
	static const char *onJoyMappedAxisMoved = "on_joymapped_axis_moved";
	static const char *onJoyConnected = "on_joy_connected";
	static const char *onJoyDisconnected = "on_joy_disconnected";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaIInputEventHandler::onKeyPressed(lua_State *L, const KeyboardEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onKeyPressed);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaKeyboardEvents::pushKeyboardEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onKeyReleased(lua_State *L, const KeyboardEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onKeyReleased);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaKeyboardEvents::pushKeyboardEvent(L, event);
		lua_call(L, 1, 0);
	}
}

#ifdef __ANDROID__
void LuaIInputEventHandler::onTouchDown(lua_State *L, const TouchEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onTouchDown);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaAndroidEvents::pushTouchEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onTouchUp(lua_State *L, const TouchEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onTouchUp);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaAndroidEvents::pushTouchEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onTouchMove(lua_State *L, const TouchEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onTouchMove);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaAndroidEvents::pushTouchEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onPointerDown(lua_State *L, const TouchEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onPointerDown);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaAndroidEvents::pushTouchEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onPointerUp(lua_State *L, const TouchEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onPointerUp);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaAndroidEvents::pushTouchEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onAcceleration(lua_State *L, const AccelerometerEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onAcceleration);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaAndroidEvents::pushAccelerometerEvent(L, event);
		lua_call(L, 1, 0);
	}
}
#endif

void LuaIInputEventHandler::onMouseButtonPressed(lua_State *L, const MouseEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onMouseButtonPressed);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaMouseEvents::pushMouseEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onMouseButtonReleased(lua_State *L, const MouseEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onMouseButtonReleased);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaMouseEvents::pushMouseEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onMouseMoved(lua_State *L, const MouseState &state)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onMouseMoved);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaMouseEvents::pushMouseState(L, state);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onScrollInput(lua_State *L, const ScrollEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onScrollInput);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaMouseEvents::pushScrollEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onJoyButtonPressed(lua_State *L, const JoyButtonEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyButtonPressed);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyButtonEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onJoyButtonReleased(lua_State *L, const JoyButtonEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyButtonReleased);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyButtonEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onJoyHatMoved(lua_State *L, const JoyHatEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyHatMoved);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyHatEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onJoyAxisMoved(lua_State *L, const JoyAxisEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyAxisMoved);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyAxisEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onJoyMappedButtonPressed(lua_State *L, const JoyMappedButtonEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyMappedButtonPressed);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyMappedButtonEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onJoyMappedButtonReleased(lua_State *L, const JoyMappedButtonEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyMappedButtonReleased);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyMappedButtonEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onJoyMappedAxisMoved(lua_State *L, const JoyMappedAxisEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyMappedAxisMoved);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyMappedAxisEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onJoyConnected(lua_State *L, const JoyConnectionEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyConnected);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyConnectionEvent(L, event);
		lua_call(L, 1, 0);
	}
}

void LuaIInputEventHandler::onJoyDisconnected(lua_State *L, const JoyConnectionEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyDisconnected);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyConnectionEvent(L, event);
		lua_call(L, 1, 0);
	}
}

}
