#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaTouchEvents.h"
#include "LuaUtils.h"
#include "InputEvents.h"

namespace ncine {

namespace LuaNames {
namespace LuaTouchEvents {
	static const char *count = "count";
	static const char *actionIndex = "action_index";
	static const char *id = "id";

	static const char *x = "x";
	static const char *y = "y";
#ifdef __ANDROID__
	static const char *z = "z";
#endif
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaTouchEvents::pushTouchEvent(lua_State *L, const TouchEvent &event)
{
	lua_newtable(L);
	LuaUtils::pushField(L, LuaNames::LuaTouchEvents::count, event.count);
	LuaUtils::pushField(L, LuaNames::LuaTouchEvents::actionIndex, event.actionIndex);
	for (unsigned int i = 0; i < event.count; i++)
	{
		lua_createtable(L, 3, 0);
		LuaUtils::pushField(L, LuaNames::LuaTouchEvents::id, event.pointers[i].id);
		LuaUtils::pushField(L, LuaNames::LuaTouchEvents::x, event.pointers[i].x);
		LuaUtils::pushField(L, LuaNames::LuaTouchEvents::y, event.pointers[i].y);
		lua_rawseti(L, -2, i);
	}
}

#ifdef __ANDROID__
void LuaTouchEvents::pushAccelerometerEvent(lua_State *L, const AccelerometerEvent &event)
{
	lua_createtable(L, 3, 0);
	LuaUtils::pushField(L, LuaNames::LuaTouchEvents::x, event.x);
	LuaUtils::pushField(L, LuaNames::LuaTouchEvents::y, event.y);
	LuaUtils::pushField(L, LuaNames::LuaTouchEvents::z, event.z);
}
#endif

}
