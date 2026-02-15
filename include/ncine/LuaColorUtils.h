#ifndef CLASS_NCINE_LUACOLORUTILS
#define CLASS_NCINE_LUACOLORUTILS

#include "common_defines.h"

struct lua_State;

namespace ncine {

class Color;

/// Lua utilities for the `Color` class
class DLL_PUBLIC LuaColorUtils
{
  public:
	static void push(lua_State *L, const Color &color);
	static void pushField(lua_State *L, const char *name, const Color &color);
	static Color retrieve(lua_State *L, int index, int &newIndex);
	static Color retrieveTable(lua_State *L, int index);
	static Color retrieveArray(lua_State *L, int index);
	static Color retrieveParams(lua_State *L, int index);
	static Color retrieveTableField(lua_State *L, int index, const char *name);
	static Color retrieveArrayField(lua_State *L, int index, const char *name);
};

}

#endif
