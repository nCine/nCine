#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaMouseEvents.h"
#include "LuaUntrackedUserData.h"
#include "LuaNames.h"
#include "InputEvents.h"

namespace ncine {

namespace LuaNames {
namespace LuaMouseEvents {
	static const char *LEFT = "LEFT";
	static const char *MIDDLE = "MIDDLE";
	static const char *RIGHT = "RIGHT";
	static const char *FOURTH = "FOURTH";
	static const char *FIFTH = "FIFTH";
	static const char *MouseButton = "mouse_button";

	static const char *x = "x";
	static const char *y = "y";
	static const char *button = "button";

	static const char *isLeftButtonDown = "is_left_down";
	static const char *isMiddleButtonDown = "is_middle_down";
	static const char *isRightButtonDown = "is_right_down";
	static const char *isFourthButtonDown = "is_fourth_down";
	static const char *isFifthButtonDown = "is_fifth_down";

	static const char *isButtonDown = "is_mouse_button_down";
	static const char *isButtonPressed = "is_mouse_button_pressed";
	static const char *isButtonReleased = "is_mouse_button_released";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

// TODO: Don't forget to commit the change in LuaStateManager.cpp
void LuaMouseEvents::expose(lua_State *L)
{
	// Multiple binding classes adding functions to the same field
	const int type = lua_getfield(L, -1, LuaNames::input);

	if (type == LUA_TNIL)
	{
		lua_pop(L, 1);
		lua_createtable(L, 0, 3);
	}

	LuaUtils::addFunction(L, LuaNames::LuaMouseEvents::isButtonDown, isButtonDown);
	LuaUtils::addFunction(L, LuaNames::LuaMouseEvents::isButtonPressed, isButtonPressed);
	LuaUtils::addFunction(L, LuaNames::LuaMouseEvents::isButtonReleased, isButtonReleased);

	lua_setfield(L, -2, LuaNames::input);
}

void LuaMouseEvents::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 5);

	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::LEFT, static_cast<int64_t>(MouseButton::LEFT));
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::MIDDLE, static_cast<int64_t>(MouseButton::MIDDLE));
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::RIGHT, static_cast<int64_t>(MouseButton::RIGHT));
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::FOURTH, static_cast<int64_t>(MouseButton::FOURTH));
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::FIFTH, static_cast<int64_t>(MouseButton::FIFTH));

	lua_setfield(L, -2, LuaNames::LuaMouseEvents::MouseButton);
}

void LuaMouseEvents::pushMouseEvent(lua_State *L, const MouseEvent &event)
{
	lua_createtable(L, 0, 3);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::x, event.x);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::y, event.y);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::button, static_cast<int64_t>(event.button));
}

/*! \note Used only by `LuaIInputEventHandler::onMouseMoved()` */
void LuaMouseEvents::pushMouseState(lua_State *L, const MouseState &state)
{
	// Only pushing the down state to not slow down the `onMouseMoved` event
	lua_createtable(L, 0, 7);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::x, state.x);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::y, state.y);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::isLeftButtonDown, state.isButtonDown(MouseButton::LEFT));
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::isMiddleButtonDown, state.isButtonDown(MouseButton::MIDDLE));
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::isRightButtonDown, state.isButtonDown(MouseButton::RIGHT));
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::isFourthButtonDown, state.isButtonDown(MouseButton::FOURTH));
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::isFifthButtonDown, state.isButtonDown(MouseButton::FIFTH));
}

void LuaMouseEvents::pushScrollEvent(lua_State *L, const ScrollEvent &event)
{
	lua_createtable(L, 0, 2);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::x, event.x);
	LuaUtils::pushField(L, LuaNames::LuaMouseEvents::y, event.y);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaMouseEvents::isButtonDown(lua_State *L)
{
	const MouseState *state = LuaUntrackedUserData<MouseState>::retrieve(L, -2);
	const MouseButton button = static_cast<MouseButton>(LuaUtils::retrieve<int64_t>(L, -1));

	if (state)
		LuaUtils::push(L, state->isButtonDown(button));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaMouseEvents::isButtonPressed(lua_State *L)
{
	const MouseState *state = LuaUntrackedUserData<MouseState>::retrieve(L, -2);
	const MouseButton button = static_cast<MouseButton>(LuaUtils::retrieve<int64_t>(L, -1));

	if (state)
		LuaUtils::push(L, state->isButtonPressed(button));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaMouseEvents::isButtonReleased(lua_State *L)
{
	const MouseState *state = LuaUntrackedUserData<MouseState>::retrieve(L, -2);
	const MouseButton button = static_cast<MouseButton>(LuaUtils::retrieve<int64_t>(L, -1));

	if (state)
		LuaUtils::push(L, state->isButtonReleased(button));
	else
		LuaUtils::pushNil(L);

	return 1;
}

}
