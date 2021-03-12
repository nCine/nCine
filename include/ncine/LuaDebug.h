#ifndef CLASS_NCINE_LUADEBUG
#define CLASS_NCINE_LUADEBUG

#include <nctl/String.h>

struct lua_State;

namespace ncine {

namespace LuaUtils {
	class RunInfo;
}

namespace LuaDebug {

	DLL_PUBLIC void traceError(lua_State *L, const char *errorMsg);
	DLL_PUBLIC void assert(lua_State *L, bool condition, const char *errorMsg);

	// Varargs version of `traceError` must follow non-varargs one in order to find it
	template <typename... Args>
	void traceError(lua_State *L, const char *fmt, Args... args)
	{
		nctl::String errorMsg(128);
		errorMsg.format(fmt, args...);
		traceError(L, errorMsg.data());
	}

	template <typename... Args>
	void assert(lua_State *L, bool condition, const char *fmt, Args... args)
	{
		if (condition == false)
			traceError(L, fmt, args...);
	}

	DLL_PUBLIC const char *statusToString(int status);
	DLL_PUBLIC const char *statusToDescription(int status);
	void printStackValue(lua_State *L, int index);

	int traceMessageHandler(lua_State *L);
	int debugInfoMessageHandler(lua_State *L);

	LuaUtils::RunInfo *retrieveRunInfo(lua_State *L);
	void pushRunInfo(lua_State *L, LuaUtils::RunInfo *runInfo);
}

}

#endif
