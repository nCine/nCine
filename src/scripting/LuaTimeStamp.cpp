#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaTimeStamp.h"
#include "TimeStamp.h"
#include "LuaUtils.h"

namespace ncine {

namespace LuaNames {
namespace TimeStamp {
	static const char *TimeStamp = "timestamp";

	static const char *now = "now";

	static const char *ticksSince = "ticks_since";
	static const char *secondsSince = "seconds_since";
	static const char *millisecondsSince = "milliseconds_since";
	static const char *microsecondsSince = "microseconds_since";
	static const char *nanosecondsSince = "nanoseconds_since";

	static const char *seconds = "seconds";
	static const char *milliseconds = "milliseconds";
	static const char *microseconds = "microseconds";
	static const char *nanoseconds = "nanoseconds";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaTimeStamp::expose(lua_State *L)
{
	lua_createtable(L, 0, 10);

	LuaUtils::addFunction(L, LuaNames::TimeStamp::now, now);

	LuaUtils::addFunction(L, LuaNames::TimeStamp::ticksSince, ticksSince);
	LuaUtils::addFunction(L, LuaNames::TimeStamp::secondsSince, secondsSince);
	LuaUtils::addFunction(L, LuaNames::TimeStamp::millisecondsSince, millisecondsSince);
	LuaUtils::addFunction(L, LuaNames::TimeStamp::microsecondsSince, microsecondsSince);
	LuaUtils::addFunction(L, LuaNames::TimeStamp::nanosecondsSince, nanosecondsSince);

	LuaUtils::addFunction(L, LuaNames::TimeStamp::seconds, seconds);
	LuaUtils::addFunction(L, LuaNames::TimeStamp::milliseconds, milliseconds);
	LuaUtils::addFunction(L, LuaNames::TimeStamp::microseconds, microseconds);
	LuaUtils::addFunction(L, LuaNames::TimeStamp::nanoseconds, nanoseconds);

	lua_setfield(L, -2, LuaNames::TimeStamp::TimeStamp);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaTimeStamp::now(lua_State *L)
{
	LuaUtils::push(L, TimeStamp().ticks());
	return 1;
}

int LuaTimeStamp::ticksSince(lua_State *L)
{
	const uint64_t counter = LuaUtils::retrieve<uint64_t>(L, -1);
	LuaUtils::push(L, TimeStamp(counter).timeSince().ticks());
	return 1;
}

int LuaTimeStamp::secondsSince(lua_State *L)
{
	const uint64_t counter = LuaUtils::retrieve<uint64_t>(L, -1);
	LuaUtils::push(L, TimeStamp(counter).secondsDoubleSince());
	return 1;
}

int LuaTimeStamp::millisecondsSince(lua_State *L)
{
	const uint64_t counter = LuaUtils::retrieve<uint64_t>(L, -1);
	LuaUtils::push(L, TimeStamp(counter).millisecondsDoubleSince());
	return 1;
}

int LuaTimeStamp::microsecondsSince(lua_State *L)
{
	const uint64_t counter = LuaUtils::retrieve<uint64_t>(L, -1);
	LuaUtils::push(L, TimeStamp(counter).microsecondsDoubleSince());
	return 1;
}

int LuaTimeStamp::nanosecondsSince(lua_State *L)
{
	const uint64_t counter = LuaUtils::retrieve<uint64_t>(L, -1);
	LuaUtils::push(L, TimeStamp(counter).nanosecondsDoubleSince());
	return 1;
}

int LuaTimeStamp::seconds(lua_State *L)
{
	const uint64_t counter = LuaUtils::retrieve<uint64_t>(L, -1);
	LuaUtils::push(L, TimeStamp(counter).secondsDouble());
	return 1;
}

int LuaTimeStamp::milliseconds(lua_State *L)
{
	const uint64_t counter = LuaUtils::retrieve<uint64_t>(L, -1);
	LuaUtils::push(L, TimeStamp(counter).millisecondsDouble());
	return 1;
}

int LuaTimeStamp::microseconds(lua_State *L)
{
	const uint64_t counter = LuaUtils::retrieve<uint64_t>(L, -1);
	LuaUtils::push(L, TimeStamp(counter).microsecondsDouble());
	return 1;
}

int LuaTimeStamp::nanoseconds(lua_State *L)
{
	const uint64_t counter = LuaUtils::retrieve<uint64_t>(L, -1);
	LuaUtils::push(L, TimeStamp(counter).nanosecondsDouble());
	return 1;
}

}
