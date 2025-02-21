#include <nctl/algorithms.h>
#include "LuaIFrameTimer.h"
#include "LuaUtils.h"
#include "IFrameTimer.h"
#include "Application.h"
#include "ILogger.h"

namespace ncine {

namespace LuaNames {
namespace IFrameTimer {
	static const char *IFrameTimer = "frame_timer";

	static const char *totalNumberFrames = "total_number_frames";
	static const char *lastFrameTime = "last_frame_time";
	static const char *currentFrameTime = "current_frame_time";

	static const char *averageFps = "average_fps";
	static const char *averageFrameTime = "average_frame_time";

	static const char *averageEnabled = "is_average_enabled";
	static const char *loggingEnabled = "is_logging_enabled";

	static const char *averageInterval = "get_average_interval";
	static const char *setAverageInterval = "set_average_interval";

	static const char *loggingInterval = "get_logging_interval";
	static const char *setLoggingInterval = "set_logging_interval";

	static const char *logLevel = "get_log_level";
	static const char *setLogLevel = "set_log_level";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaIFrameTimer::expose(lua_State *L)
{
	LuaUtils::createTable(L, 0, 13);

	LuaUtils::addFunction(L, LuaNames::IFrameTimer::totalNumberFrames, totalNumberFrames);
	LuaUtils::addFunction(L, LuaNames::IFrameTimer::lastFrameTime, lastFrameTime);
	LuaUtils::addFunction(L, LuaNames::IFrameTimer::currentFrameTime, currentFrameTime);

	LuaUtils::addFunction(L, LuaNames::IFrameTimer::averageFps, averageFps);
	LuaUtils::addFunction(L, LuaNames::IFrameTimer::averageFrameTime, averageFrameTime);

	LuaUtils::addFunction(L, LuaNames::IFrameTimer::averageEnabled, averageEnabled);
	LuaUtils::addFunction(L, LuaNames::IFrameTimer::loggingEnabled, loggingEnabled);

	LuaUtils::addFunction(L, LuaNames::IFrameTimer::averageInterval, averageInterval);
	LuaUtils::addFunction(L, LuaNames::IFrameTimer::setAverageInterval, setAverageInterval);

	LuaUtils::addFunction(L, LuaNames::IFrameTimer::loggingInterval, loggingInterval);
	LuaUtils::addFunction(L, LuaNames::IFrameTimer::setLoggingInterval, setLoggingInterval);

	LuaUtils::addFunction(L, LuaNames::IFrameTimer::logLevel, logLevel);
	LuaUtils::addFunction(L, LuaNames::IFrameTimer::setLogLevel, setLogLevel);

	LuaUtils::setField(L, -2, LuaNames::IFrameTimer::IFrameTimer);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaIFrameTimer::totalNumberFrames(lua_State *L)
{
	const IFrameTimer &frameTimer = theApplication().frameTimer();
	// The cast disambiguate the `push` function on Emscripten
	LuaUtils::push(L, static_cast<uint64_t>(frameTimer.totalNumberFrames()));
	return 1;
}

int LuaIFrameTimer::lastFrameTime(lua_State *L)
{
	const IFrameTimer &frameTimer = theApplication().frameTimer();
	LuaUtils::push(L, frameTimer.lastFrameTime());
	return 1;
}

int LuaIFrameTimer::currentFrameTime(lua_State *L)
{
	const IFrameTimer &frameTimer = theApplication().frameTimer();
	LuaUtils::push(L, frameTimer.currentFrameTime());
	return 1;
}

int LuaIFrameTimer::averageFps(lua_State *L)
{
	const IFrameTimer &frameTimer = theApplication().frameTimer();
	LuaUtils::push(L, frameTimer.averageFps());
	return 1;
}

int LuaIFrameTimer::averageFrameTime(lua_State *L)
{
	const IFrameTimer &frameTimer = theApplication().frameTimer();
	LuaUtils::push(L, frameTimer.averageFrameTime());
	return 1;
}

int LuaIFrameTimer::averageEnabled(lua_State *L)
{
	const IFrameTimer &frameTimer = theApplication().frameTimer();
	LuaUtils::push(L, frameTimer.averageEnabled());
	return 1;
}

int LuaIFrameTimer::loggingEnabled(lua_State *L)
{
	const IFrameTimer &frameTimer = theApplication().frameTimer();
	LuaUtils::push(L, frameTimer.loggingEnabled());
	return 1;
}

int LuaIFrameTimer::averageInterval(lua_State *L)
{
	const IFrameTimer &frameTimer = theApplication().frameTimer();
	LuaUtils::push(L, frameTimer.averageInterval());
	return 1;
}

int LuaIFrameTimer::setAverageInterval(lua_State *L)
{
	const float averageInterval = LuaUtils::retrieve<float>(L, -1);

	IFrameTimer &frameTimer = theApplication().frameTimer();
	frameTimer.setAverageInterval(averageInterval);

	return 0;
}

int LuaIFrameTimer::loggingInterval(lua_State *L)
{
	const IFrameTimer &frameTimer = theApplication().frameTimer();
	LuaUtils::push(L, frameTimer.loggingInterval());
	return 1;
}

int LuaIFrameTimer::setLoggingInterval(lua_State *L)
{
	const float loggingInterval = LuaUtils::retrieve<float>(L, -1);

	IFrameTimer &frameTimer = theApplication().frameTimer();
	frameTimer.setLoggingInterval(loggingInterval);

	return 0;
}

int LuaIFrameTimer::logLevel(lua_State *L)
{
	const IFrameTimer &frameTimer = theApplication().frameTimer();
	LuaUtils::push(L, static_cast<int64_t>(frameTimer.logLevel()));
	return 1;
}

int LuaIFrameTimer::setLogLevel(lua_State *L)
{
	const int64_t logLevelInt = LuaUtils::retrieve<int64_t>(L, -1);
	const ILogger::LogLevel logLevel = static_cast<ILogger::LogLevel>(nctl::clamp(logLevelInt,
	                                               static_cast<int64_t>(ILogger::LogLevel::OFF),
	                                               static_cast<int64_t>(ILogger::LogLevel::FATAL)));

	IFrameTimer &frameTimer = theApplication().frameTimer();
	frameTimer.setLogLevel(logLevel);

	return 0;
}

}
