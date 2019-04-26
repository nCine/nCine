#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaAndroidEvents.h"
#include "LuaUtils.h"
#include "InputEvents.h"

namespace ncine {

namespace LuaNames {
namespace LuaAndroidEvents {
	static const char *count = "count";
	static const char *actionIndex = "action_index";
	static const char *id = "id";

	static const char *x = "x";
	static const char *y = "y";
	static const char *z = "z";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaAndroidEvents::pushTouchEvent(lua_State *L, const TouchEvent &event)
{
	lua_newtable(L);
	LuaUtils::pushField(L, LuaNames::LuaAndroidEvents::count, event.count);
	LuaUtils::pushField(L, LuaNames::LuaAndroidEvents::actionIndex, event.actionIndex);
	for (unsigned int i = 0; i < event.count; i++)
	{
		lua_createtable(L, 3, 0);
		LuaUtils::pushField(L, LuaNames::LuaAndroidEvents::id, event.pointers[i].id);
		LuaUtils::pushField(L, LuaNames::LuaAndroidEvents::x, event.pointers[i].x);
		LuaUtils::pushField(L, LuaNames::LuaAndroidEvents::y, event.pointers[i].y);
		lua_rawseti(L, -2, i);
	}
}

void LuaAndroidEvents::pushAccelerometerEvent(lua_State *L, const AccelerometerEvent &event)
{
	lua_createtable(L, 3, 0);
	LuaUtils::pushField(L, LuaNames::LuaAndroidEvents::x, event.x);
	LuaUtils::pushField(L, LuaNames::LuaAndroidEvents::y, event.y);
	LuaUtils::pushField(L, LuaNames::LuaAndroidEvents::z, event.z);
}

}
