#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaRectAnimation.h"
#include "LuaRectUtils.h"
#include "RectAnimation.h"

namespace ncine {

namespace LuaNames {
namespace RectAnimation {
	static const char *DISABLED = "DISABLED";
	static const char *ENABLED = "ENABLED";
	static const char *LoopMode = "loop_mode";

	static const char *FROM_START = "FROM_START";
	static const char *BACKWARD = "BACKWARD";
	static const char *RewindMode = "rewind_mode";

	static const char *frameTime = "frame_time";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaRectAnimation::exposeConstants(lua_State *L)
{
	lua_createtable(L, 2, 0);

	LuaUtils::pushField(L, LuaNames::RectAnimation::DISABLED, static_cast<int64_t>(RectAnimation::LoopMode::DISABLED));
	LuaUtils::pushField(L, LuaNames::RectAnimation::ENABLED, static_cast<int64_t>(RectAnimation::LoopMode::ENABLED));

	lua_setfield(L, -2, LuaNames::RectAnimation::LoopMode);

	lua_createtable(L, 2, 0);

	LuaUtils::pushField(L, LuaNames::RectAnimation::FROM_START, static_cast<int64_t>(RectAnimation::RewindMode::FROM_START));
	LuaUtils::pushField(L, LuaNames::RectAnimation::BACKWARD, static_cast<int64_t>(RectAnimation::RewindMode::BACKWARD));

	lua_setfield(L, -2, LuaNames::RectAnimation::RewindMode);
}

nctl::UniquePtr<RectAnimation> LuaRectAnimation::retrieveTable(lua_State *L, int index)
{
	const float frameTime = LuaUtils::retrieveField<float>(L, index, LuaNames::RectAnimation::frameTime);
	const RectAnimation::LoopMode loopMode = static_cast<RectAnimation::LoopMode>(LuaUtils::retrieveField<int64_t>(L, index, LuaNames::RectAnimation::LoopMode));
	const RectAnimation::RewindMode rewindMode = static_cast<RectAnimation::RewindMode>(LuaUtils::retrieveField<int64_t>(L, index, LuaNames::RectAnimation::RewindMode));

	nctl::UniquePtr<RectAnimation> animation = nctl::makeUnique<RectAnimation>(frameTime, loopMode, rewindMode);

	const unsigned int length = lua_rawlen(L, index);
	for (unsigned int i = 0; i < length; i++)
	{
		const int type = lua_rawgeti(L, index, i + 1);
		if (type != LUA_TTABLE)
			luaL_argerror(L, -1, "Expecting a table");

		Recti rect = LuaRectiUtils::retrieveTable(L, -1);
		animation->addRect(rect);
		lua_pop(L, 1);
	}

	return nctl::move(animation);
}

}
