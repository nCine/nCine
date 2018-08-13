#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaMouseEvents.h"
#include "LuaUtils.h"
#include "InputEvents.h"

namespace ncine {

namespace LuaNames {
namespace LuaMouseEvents
{
	static const char *LEFT = "LEFT";
	static const char *MIDDLE = "MIDDLE";
	static const char *RIGHT = "RIGHT";
	static const char *FOURTH = "FOURTH";
	static const char *FIFTH = "FIFTH";
	static const char *MouseButton = "mouse_button";

	static const char *x = "x";
	static const char *y = "y";
	static const char *button = "button";
	static const char *isLeftButtonDown = "left_pressed";
	static const char *isMiddleButtonDown = "middle_pressed";
	static const char *isRightButtonDown = "right_pressed";
	static const char *isFourthButtonDown = "fourth_pressed";
	static const char *isFifthButtonDown = "fifth_pressed";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

enum class MouseButton
{
	LEFT = 0,
	MIDDLE,
	RIGHT,
	FOURTH,
	FIFTH
};

}

void LuaMouseEvents::exposeConstants(lua_State *L)
{
	lua_createtable(L, 5, 0);

	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::LEFT, static_cast<int64_t>(MouseButton::LEFT));
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::MIDDLE, static_cast<int64_t>(MouseButton::MIDDLE));
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::RIGHT, static_cast<int64_t>(MouseButton::RIGHT));
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::FOURTH, static_cast<int64_t>(MouseButton::FOURTH));
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::FIFTH, static_cast<int64_t>(MouseButton::FIFTH));

	lua_setfield(L, -2, LuaNames::LuaMouseEvents::MouseButton);
}

void LuaMouseEvents::pushMouseEvent(lua_State *L, const MouseEvent &event)
{
	lua_createtable(L, 3, 0);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::x, event.x);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::y, event.y);
	if (event.isLeftButton())
		LuaUtils::pushField(L, LuaNames::LuaMouseEvents::button, static_cast<int64_t>(MouseButton::LEFT));
	else if (event.isMiddleButton())
		LuaUtils::pushField(L, LuaNames::LuaMouseEvents::button, static_cast<int64_t>(MouseButton::MIDDLE));
	else if (event.isRightButton())
		LuaUtils::pushField(L, LuaNames::LuaMouseEvents::button, static_cast<int64_t>(MouseButton::RIGHT));
	else if (event.isFourthButton())
		LuaUtils::pushField(L, LuaNames::LuaMouseEvents::button, static_cast<int64_t>(MouseButton::FOURTH));
	else if (event.isFifthButton())
		LuaUtils::pushField(L, LuaNames::LuaMouseEvents::button, static_cast<int64_t>(MouseButton::FIFTH));
}

void LuaMouseEvents::pushMouseState(lua_State *L, const MouseState &state)
{
	lua_createtable(L, 7, 0);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::x, state.x);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::y, state.y);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::isLeftButtonDown, state.isLeftButtonDown());
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::isMiddleButtonDown, state.isMiddleButtonDown());
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::isRightButtonDown, state.isRightButtonDown());
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::isFourthButtonDown, state.isFourthButtonDown());
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::isFifthButtonDown, state.isFifthButtonDown());
}

void LuaMouseEvents::pushScrollEvent(lua_State *L, const ScrollEvent &event)
{
	lua_createtable(L, 2, 0);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::x, event.x);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::y, event.y);
}

}
