#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaILogger.h"
#include "LuaUtils.h"
#include "common_macros.h"

namespace ncine {

namespace LuaNames {
namespace ILogger {
	static const char *ILogger = "log";

	static const char *logVerbose = "verbose";
	static const char *logDebug = "debug";
	static const char *logInfo = "info";
	static const char *logWarn = "warn";
	static const char *logError = "error";
	static const char *logFatal = "fatal";

	static const char *UNKNOWN = "UNKNOWN";
	static const char *VERBOSE = "VERBOSE";
	static const char *DEBUG = "DEBUG";
	static const char *INFO = "INFO";
	static const char *WARN = "WARN";
	static const char *ERROR = "ERROR";
	static const char *FATAL = "FATAL";
	static const char *OFF = "OFF";
	static const char *LogLevel = "log_level";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaILogger::expose(lua_State *L)
{
	lua_createtable(L, 0, 6);

	LuaUtils::addFunction(L, LuaNames::ILogger::logVerbose, logVerbose);
	LuaUtils::addFunction(L, LuaNames::ILogger::logDebug, logDebug);
	LuaUtils::addFunction(L, LuaNames::ILogger::logInfo, logInfo);
	LuaUtils::addFunction(L, LuaNames::ILogger::logWarn, logWarn);
	LuaUtils::addFunction(L, LuaNames::ILogger::logError, logError);
	LuaUtils::addFunction(L, LuaNames::ILogger::logFatal, logFatal);

	lua_setfield(L, -2, LuaNames::ILogger::ILogger);
}

void LuaILogger::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 8);

	LuaUtils::pushField(L, LuaNames::ILogger::UNKNOWN, static_cast<int64_t>(ILogger::LogLevel::UNKNOWN));
	LuaUtils::pushField(L, LuaNames::ILogger::VERBOSE, static_cast<int64_t>(ILogger::LogLevel::VERBOSE));
	LuaUtils::pushField(L, LuaNames::ILogger::DEBUG, static_cast<int64_t>(ILogger::LogLevel::DEBUG));
	LuaUtils::pushField(L, LuaNames::ILogger::INFO, static_cast<int64_t>(ILogger::LogLevel::INFO));
	LuaUtils::pushField(L, LuaNames::ILogger::WARN, static_cast<int64_t>(ILogger::LogLevel::WARN));
	LuaUtils::pushField(L, LuaNames::ILogger::ERROR, static_cast<int64_t>(ILogger::LogLevel::ERROR));
	LuaUtils::pushField(L, LuaNames::ILogger::FATAL, static_cast<int64_t>(ILogger::LogLevel::FATAL));
	LuaUtils::pushField(L, LuaNames::ILogger::OFF, static_cast<int64_t>(ILogger::LogLevel::OFF));

	lua_setfield(L, -2, LuaNames::ILogger::LogLevel);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaILogger::logVerbose(lua_State *L)
{
	const char *string = LuaUtils::retrieve<const char *>(L, -1);
	LOGV_X("%s", string);
	return 0;
}

int LuaILogger::logDebug(lua_State *L)
{
	const char *string = LuaUtils::retrieve<const char *>(L, -1);
	LOGD_X("%s", string);
	return 0;
}

int LuaILogger::logInfo(lua_State *L)
{
	const char *string = LuaUtils::retrieve<const char *>(L, -1);
	LOGI_X("%s", string);
	return 0;
}

int LuaILogger::logWarn(lua_State *L)
{
	const char *string = LuaUtils::retrieve<const char *>(L, -1);
	LOGW_X("%s", string);
	return 0;
}

int LuaILogger::logError(lua_State *L)
{
	const char *string = LuaUtils::retrieve<const char *>(L, -1);
	LOGE_X("%s", string);
	return 0;
}

int LuaILogger::logFatal(lua_State *L)
{
	const char *string = LuaUtils::retrieve<const char *>(L, -1);
	LOGF_X("%s", string);
	return 0;
}

}
