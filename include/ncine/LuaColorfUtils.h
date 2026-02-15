#ifndef CLASS_NCINE_LUACOLORFUTILS
#define CLASS_NCINE_LUACOLORFUTILS

#include "common_defines.h"

struct lua_State;

namespace ncine {

class Colorf;

/// Lua utilities for the `Colorf` class
class DLL_PUBLIC LuaColorfUtils
{
  public:
	static void push(lua_State *L, const Colorf &color);
	static void pushField(lua_State *L, const char *name, const Colorf &color);
	static Colorf retrieve(lua_State *L, int index, int &newIndex);
	static Colorf retrieveTable(lua_State *L, int index);
	static Colorf retrieveArray(lua_State *L, int index);
	static Colorf retrieveParams(lua_State *L, int index);
	static Colorf retrieveTableField(lua_State *L, int index, const char *name);
	static Colorf retrieveArrayField(lua_State *L, int index, const char *name);
};

}

#endif
