#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaIInputEventHandler.h"
#include "LuaClassWrapper.h"
#include "LuaKeyboardEvents.h"
#include "LuaMouseEvents.h"
#include "LuaJoystickEvents.h"
#include "LuaTouchEvents.h"
#include "LuaNames.h"
#include "InputEvents.h"

namespace ncine {

namespace LuaNames {
namespace IInputEventHandler {
	static const char *onKeyPressed = "on_key_pressed";
	static const char *onKeyReleased = "on_key_released";
	static const char *onTextInput = "on_text_input";

	static const char *onTouchDown = "on_touch_down";
	static const char *onTouchUp = "on_touch_up";
	static const char *onTouchMove = "on_touch_move";
	static const char *onPointerDown = "on_pointer_down";
	static const char *onPointerUp = "on_pointer_up";
#ifdef __ANDROID__
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

	static const char *onQuitRequest = "on_quit_request";
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
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onKeyPressed, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onKeyReleased(lua_State *L, const KeyboardEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onKeyReleased);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaKeyboardEvents::pushKeyboardEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onKeyReleased, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onTextInput(lua_State *L, const TextInputEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onTextInput);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaKeyboardEvents::pushTextInputEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onTextInput, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onTouchDown(lua_State *L, const TouchEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onTouchDown);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaTouchEvents::pushTouchEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onTouchDown, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onTouchUp(lua_State *L, const TouchEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onTouchUp);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaTouchEvents::pushTouchEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onTouchUp, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onTouchMove(lua_State *L, const TouchEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onTouchMove);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaTouchEvents::pushTouchEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onTouchMove, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onPointerDown(lua_State *L, const TouchEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onPointerDown);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaTouchEvents::pushTouchEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onPointerDown, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onPointerUp(lua_State *L, const TouchEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onPointerUp);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaTouchEvents::pushTouchEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onPointerUp, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

#ifdef __ANDROID__
void LuaIInputEventHandler::onAcceleration(lua_State *L, const AccelerometerEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onAcceleration);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaTouchEvents::pushAccelerometerEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onAcceleration, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
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
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onMouseButtonPressed, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onMouseButtonReleased(lua_State *L, const MouseEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onMouseButtonReleased);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaMouseEvents::pushMouseEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onMouseButtonReleased, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onMouseMoved(lua_State *L, const MouseState &state)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onMouseMoved);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaMouseEvents::pushMouseState(L, state);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onMouseMoved, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onScrollInput(lua_State *L, const ScrollEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onScrollInput);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaMouseEvents::pushScrollEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onScrollInput, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onJoyButtonPressed(lua_State *L, const JoyButtonEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyButtonPressed);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyButtonEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onJoyButtonPressed, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onJoyButtonReleased(lua_State *L, const JoyButtonEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyButtonReleased);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyButtonEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onJoyButtonReleased, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onJoyHatMoved(lua_State *L, const JoyHatEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyHatMoved);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyHatEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onJoyHatMoved, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onJoyAxisMoved(lua_State *L, const JoyAxisEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyAxisMoved);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyAxisEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onJoyAxisMoved, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onJoyMappedButtonPressed(lua_State *L, const JoyMappedButtonEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyMappedButtonPressed);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyMappedButtonEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onJoyMappedButtonPressed, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onJoyMappedButtonReleased(lua_State *L, const JoyMappedButtonEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyMappedButtonReleased);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyMappedButtonEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onJoyMappedButtonReleased, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onJoyMappedAxisMoved(lua_State *L, const JoyMappedAxisEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyMappedAxisMoved);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyMappedAxisEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onJoyMappedAxisMoved, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onJoyConnected(lua_State *L, const JoyConnectionEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyConnected);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyConnectionEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onJoyConnected, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

void LuaIInputEventHandler::onJoyDisconnected(lua_State *L, const JoyConnectionEvent &event)
{
	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onJoyDisconnected);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		LuaJoystickEvents::pushJoyConnectionEvent(L, event);
		const int status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onJoyDisconnected, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 2);
}

bool LuaIInputEventHandler::onQuitRequest(lua_State *L)
{
	bool shouldQuit = true;

	lua_getglobal(L, LuaNames::ncine);
	const int type = lua_getfield(L, -1, LuaNames::IInputEventHandler::onQuitRequest);

	if (type != LUA_TNIL)
	{
		ASSERT(type == LUA_TFUNCTION);
		const int status = lua_pcall(L, 0, 1, 0);
		if (status != LUA_OK)
		{
			LOGE_X("Error running Lua function \"%s\" (%s):\n%s", LuaNames::IInputEventHandler::onQuitRequest, LuaDebug::statusToString(status), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
		else
		{
			if (lua_isboolean(L, -1) == false)
				LOGW("Expecting a boolean at index -1");
			else
				shouldQuit = LuaUtils::retrieve<bool>(L, -1);
		}
	}
	else
		lua_pop(L, 2);

	return shouldQuit;
}

}
