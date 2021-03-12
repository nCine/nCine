#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include <nctl/CString.h>
#include "LuaDebug.h"
#include "LuaUtils.h"

namespace ncine {

namespace {
const char *debugInfoKey = "k";
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaDebug::traceError(lua_State *L, const char *errorMsg)
{
	luaL_traceback(L, L, errorMsg, 0);
	LOGE_X("Lua trace error: %s", lua_tostring(L, -1));
	lua_pop(L, 1);
}

void LuaDebug::assert(lua_State *L, bool condition, const char *errorMsg)
{
	if (condition == false)
	{
		luaL_traceback(L, L, errorMsg, 0);
		LOGE_X("Lua assert: %s", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

const char *LuaDebug::statusToString(int status)
{
	switch (status)
	{
		case LUA_OK: return "LUA_OK";
		case LUA_YIELD: return "LUA_YIELD";
		case LUA_ERRRUN: return "LUA_ERRRUN";
		case LUA_ERRSYNTAX: return "LUA_ERRSYNTAX";
		case LUA_ERRMEM: return "LUA_ERRMEM";
#if LUA_VERSION_NUM <= 503
		case LUA_ERRGCMM: return "LUA_ERRGCMM";
#endif
		case LUA_ERRERR: return "LUA_ERRERR";
		default:
			return "No error";
	}
}

const char *LuaDebug::statusToDescription(int status)
{
	switch (status)
	{
		case LUA_OK: return "Success (LUA_OK)";
		case LUA_YIELD: return "Thread is suspended (LUA_YIELD)";
		case LUA_ERRRUN: return "Run-time error (LUA_ERRRUN)";
		case LUA_ERRSYNTAX: return "Syntax error during precompilation (LUA_ERRSYNTAX)";
		case LUA_ERRMEM: return "Memory allocation error (LUA_ERRMEM)";
#if LUA_VERSION_NUM <= 503
		case LUA_ERRGCMM: return "Error while running a __gc metamethod (LUA_ERRGCMM)";
#endif
		case LUA_ERRERR: return "Error while running the message handler (LUA_ERRERR)";
		default:
			return "No error";
	}
}

void LuaDebug::printStackValue(lua_State *L, int index)
{
	const int type = lua_type(L, index);

	switch (type)
	{
		case LUA_TNIL:
			LOGI_X("Lua value at index %d is nil", index);
			break;
		case LUA_TBOOLEAN:
			LOGI_X("Lua value at index %d is a boolean: %s", index, lua_toboolean(L, index) ? "true" : "false");
			break;
		case LUA_TLIGHTUSERDATA:
			LOGI_X("Lua value at index %d is a light userdata: %s", index, lua_typename(L, index));
			break;
		case LUA_TNUMBER:
			if (lua_isinteger(L, index))
				LOGI_X("Lua value at index %d is an integer: %lld", index, lua_tointeger(L, index));
			else
				LOGI_X("Lua value at index %d is a number: %g", index, lua_tonumber(L, index));
			break;
		case LUA_TSTRING:
			LOGI_X("Lua value at index %d is a string: %s", index, lua_tostring(L, index));
			break;
		case LUA_TTABLE:
			LOGI_X("Lua value at index %d is a table: %s", index, lua_typename(L, index));
			break;
		case LUA_TFUNCTION:
			if (lua_iscfunction(L, index))
				LOGI_X("Lua value at index %d is a C function: %s", index, lua_typename(L, index));
			else
				LOGI_X("Lua value at index %d is a Lua function: %s", index, lua_typename(L, index));
			break;
		case LUA_TUSERDATA:
			LOGI_X("Lua value at index %d is an userdata: %s", index, lua_typename(L, index));
			break;
		case LUA_TTHREAD:
			LOGI_X("Lua value at index %d is a thread: %s", index, lua_typename(L, index));
			break;
		default:
			break;
	}
}

int LuaDebug::traceMessageHandler(lua_State *L)
{
	const char *msg = lua_tostring(L, -1);
	luaL_traceback(L, L, msg, 1);
	return 1;
}

int LuaDebug::debugInfoMessageHandler(lua_State *L)
{
	LuaUtils::RunInfo *runInfo_ = retrieveRunInfo(L);
	if (runInfo_ == nullptr)
		return 1;

	lua_Debug ar;
	unsigned int &numLevels = runInfo_->numLevels_;
	numLevels = 0;
	while (lua_getstack(L, numLevels + 1, &ar) && numLevels < LuaUtils::RunInfo::MaxLevels)
	{
		lua_getinfo(L, "nSltu", &ar);
		LuaUtils::RunInfo::DebugInfo &debugInfo = runInfo_->debugInfo_[numLevels];

		debugInfo.source = ar.source;
#if LUA_VERSION_NUM <= 503
		debugInfo.srcLen = 0;
#else
		debugInfo.srcLen = ar.srclen;
#endif
		nctl::strncpy(debugInfo.shortSrc, ar.short_src, LuaUtils::RunInfo::DebugInfo::MaxShortSrcLength);
		debugInfo.lineDefined = ar.linedefined;
		debugInfo.lastLineDefined = ar.lastlinedefined;

		if (strncmp(ar.what, "Lua", 3) == 0)
			debugInfo.what = LuaUtils::RunInfo::DebugInfo::WhatType::LUA;
		else if (strncmp(ar.what, "C", 1) == 0)
			debugInfo.what = LuaUtils::RunInfo::DebugInfo::WhatType::C;
		else if (strncmp(ar.what, "main", 4) == 0)
			debugInfo.what = LuaUtils::RunInfo::DebugInfo::WhatType::MAIN;

		debugInfo.currentLine = ar.currentline;
		debugInfo.name = ar.name;

		if (strncmp(ar.namewhat, "global", 6) == 0)
			debugInfo.nameWhat = LuaUtils::RunInfo::DebugInfo::NameWhatType::GLOBAL;
		else if (strncmp(ar.namewhat, "local", 5) == 0)
			debugInfo.nameWhat = LuaUtils::RunInfo::DebugInfo::NameWhatType::LOCAL;
		else if (strncmp(ar.namewhat, "method", 6) == 0)
			debugInfo.nameWhat = LuaUtils::RunInfo::DebugInfo::NameWhatType::METHOD;
		else if (strncmp(ar.namewhat, "field", 5) == 0)
			debugInfo.nameWhat = LuaUtils::RunInfo::DebugInfo::NameWhatType::FIELD;
		else if (strncmp(ar.namewhat, "upvalue", 7) == 0)
			debugInfo.nameWhat = LuaUtils::RunInfo::DebugInfo::NameWhatType::UPVALUE;
		else if (*ar.namewhat == '\0')
			debugInfo.nameWhat = LuaUtils::RunInfo::DebugInfo::NameWhatType::EMPTY;

		debugInfo.isTailCall = ar.istailcall;
		debugInfo.nUps = ar.nups;
		debugInfo.nParams = ar.nparams;
		debugInfo.isVarArg = ar.isvararg;

		numLevels++;
	}

	return 1;
}

LuaUtils::RunInfo *LuaDebug::retrieveRunInfo(lua_State *L)
{
	LuaUtils::RunInfo *runInfo = nullptr;

	lua_pushlightuserdata(L, reinterpret_cast<void *>(&debugInfoKey));
	const int type = lua_gettable(L, LUA_REGISTRYINDEX);
	if (type == LUA_TLIGHTUSERDATA)
		runInfo =  reinterpret_cast<LuaUtils::RunInfo *>(lua_touserdata(L, -1));
	lua_pop(L, 1);

	return runInfo;
}

void LuaDebug::pushRunInfo(lua_State *L, LuaUtils::RunInfo *runInfo)
{
	lua_pushlightuserdata(L, reinterpret_cast<void *>(&debugInfoKey));
	lua_pushlightuserdata(L, runInfo);
	lua_settable(L, LUA_REGISTRYINDEX);
}

}
