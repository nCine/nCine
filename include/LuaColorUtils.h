#ifndef CLASS_NCINE_LUACOLORUTILS
#define CLASS_NCINE_LUACOLORUTILS

#include "common_macros.h"

struct lua_State;

namespace ncine {

class Colorf;

/// Lua utilities for the `Color` class
class DLL_PUBLIC LuaColorUtils
{
  public:
	static void push(lua_State *L, const Colorf &color);
	static Colorf retrieve(lua_State *L, int index, int &newIndex);
	static Colorf retrieveTable(lua_State *L, int index);
	static Colorf retrieveArray(lua_State *L, int index);
	static Colorf retrieveParams(lua_State *L, int index);
	static Colorf retrieveTableField(lua_State *L, int index, const char *name);
	static Colorf retrieveArrayField(lua_State *L, int index, const char *name);
};

}

#endif
