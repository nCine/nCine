#include <cstring>

#define NCINE_INCLUDE_LUA
#include "common_headers.h"
#include "common_macros.h"

#include "LuaUtils.h"
#include "LuaDebug.h"

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaUtils::addFunction(lua_State *L, const char *name, int (*func) (lua_State *L))
{
	lua_pushcfunction(L, func);
	lua_setfield(L, -2, name);
}

void LuaUtils::createTable(lua_State *L, int narr, int nrec)
{
	lua_createtable(L, narr, nrec);
}

void LuaUtils::newTable(lua_State *L)
{
	lua_createtable(L, 0, 0);
}

void LuaUtils::call(lua_State *L, int nargs, int nresults)
{
	lua_call(L, nargs, nresults);
}

void LuaUtils::pop(lua_State *L, int n)
{
	lua_pop(L, n);
}

void LuaUtils::pop(lua_State *L)
{
	lua_pop(L, 1);
}

bool LuaUtils::isNil(lua_State *L, int index)
{
	return lua_isnil(L, index);
}

bool LuaUtils::isTable(int type)
{
	return (type == LUA_TTABLE);
}

bool LuaUtils::isTable(lua_State *L, int index)
{
	return lua_istable(L, index);
}

size_t LuaUtils::rawLen(lua_State *L, int index)
{
	return lua_rawlen(L, index);
}

int LuaUtils::rawGeti(lua_State *L, int index, int64_t n)
{
	return lua_rawgeti(L, index, n);
}

void LuaUtils::rawSeti(lua_State *L, int index, int64_t i)
{
	lua_rawseti(L, index, i);
}

///////////////////////////////////////////////////////////
// RETRIEVE FUNCTIONS
///////////////////////////////////////////////////////////

template <> double LuaUtils::retrieve<double>(lua_State *L, int index)
{
	if (lua_isnumber(L, index) == false)
		luaL_argerror(L, index, "Expecting a number");
	return static_cast<double>(lua_tonumber(L, index));
}

template <> float LuaUtils::retrieve<float>(lua_State *L, int index)
{
	if (lua_isnumber(L, index) == false)
		luaL_argerror(L, index, "Expecting a number");
	return static_cast<float>(lua_tonumber(L, index));
}

template <> int64_t LuaUtils::retrieve<int64_t>(lua_State *L, int index)
{
	if (lua_isinteger(L, index) == false)
		luaL_argerror(L, index, "Expecting an integer");
	return static_cast<int64_t>(lua_tointeger(L, index));
}

template <> uint64_t LuaUtils::retrieve<uint64_t>(lua_State *L, int index)
{
	if (lua_isinteger(L, index) == false)
		luaL_argerror(L, index, "Expecting an integer");

	lua_Integer luaInteger = lua_tointeger(L, index);
	LuaDebug::assert(L, luaInteger >= 0, "Integer number cannot be negative");

	return static_cast<uint64_t>(luaInteger);
}

template <> int32_t LuaUtils::retrieve<int32_t>(lua_State *L, int index)
{
	if (lua_isinteger(L, index) == false)
		luaL_argerror(L, index, "Expecting an integer");
	return static_cast<int32_t>(lua_tointeger(L, index));
}

template <> uint32_t LuaUtils::retrieve<uint32_t>(lua_State *L, int index)
{
	if (lua_isinteger(L, index) == false)
		luaL_argerror(L, index, "Expecting an integer");

	lua_Integer luaInteger = lua_tointeger(L, index);
	LuaDebug::assert(L, luaInteger >= 0, "Integer number cannot be negative");

	return static_cast<uint32_t>(luaInteger);
}

template <> const char *LuaUtils::retrieve<const char *>(lua_State *L, int index)
{
	if (lua_isstring(L, index) == false)
		luaL_argerror(L, index, "Expecting a string");
	return static_cast<const char *>(lua_tostring(L, index));
}

template <> bool LuaUtils::retrieve<bool>(lua_State *L, int index)
{
	if (lua_isboolean(L, index) == false)
		luaL_argerror(L, index, "Expecting a boolean");
	return static_cast<bool>(lua_toboolean(L, index));
}

void *LuaUtils::retrieveUserData(lua_State *L, int index)
{
	if (lua_islightuserdata(L, index) == false)
		luaL_argerror(L, index, "Expecting a light userdata");
	return lua_touserdata(L, index);
}

template <class T>
void LuaUtils::retrieveArray(lua_State *L, int index, int arrayIndex, int length, T *array)
{
	LuaDebug::assert(L, lua_rawlen(L, index) >= arrayIndex + length, "Expecting an array of a minimum length of %d", arrayIndex + length);
	for (int i = 0; i < length; i++)
	{
		lua_rawgeti(L, index, arrayIndex + i);
		array[i] = retrieve<T>(L, -1);
	}
}

///////////////////////////////////////////////////////////
// RETRIEVE FIELD FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

template <class T> void getFieldAndCheckType(lua_State *L, int index, const char *name) { }

template <> void getFieldAndCheckType<lua_Number>(lua_State *L, int index, const char *name)
{
	lua_getfield(L, index, name);
	LuaDebug::assert(L, lua_isnumber(L, -1), "Cannot retrieve a number in table field \"%s\"", name);
}

template <> void getFieldAndCheckType<lua_Integer>(lua_State *L, int index, const char *name)
{
	lua_getfield(L, index, name);
	LuaDebug::assert(L, lua_isinteger(L, -1), "Cannot retrieve an integer in table field \"%s\"", name);
}

template <> void getFieldAndCheckType<const char *>(lua_State *L, int index, const char *name)
{
	lua_getfield(L, index, name);
	LuaDebug::assert(L, lua_isstring(L, -1), "Cannot retrieve a string in table field \"%s\"", name);
}

}

template <> double LuaUtils::retrieveField<double>(lua_State *L, int index, const char *name)
{
	getFieldAndCheckType<lua_Number>(L, index, name);
	double number = static_cast<double>(lua_tonumber(L, -1));
	lua_pop(L, 1);

	return number;
}

template <> float LuaUtils::retrieveField<float>(lua_State *L, int index, const char *name)
{
	getFieldAndCheckType<lua_Number>(L, index, name);
	float number = static_cast<float>(lua_tonumber(L, -1));
	lua_pop(L, 1);

	return number;
}

template <> int64_t LuaUtils::retrieveField<int64_t>(lua_State *L, int index, const char *name)
{
	getFieldAndCheckType<lua_Integer>(L, index, name);
	int64_t integer = static_cast<int64_t>(lua_tointeger(L, -1));
	lua_pop(L, 1);

	return integer;
}

template <> uint64_t LuaUtils::retrieveField<uint64_t>(lua_State *L, int index, const char *name)
{
	getFieldAndCheckType<lua_Integer>(L, index, name);

	lua_Integer luaInteger = lua_tointeger(L, -1);
	LuaDebug::assert(L, luaInteger >= 0, "Integer number cannot be negative");

	uint64_t integer = static_cast<uint64_t>(luaInteger);
	lua_pop(L, 1);

	return integer;
}

template <> int32_t LuaUtils::retrieveField<int32_t>(lua_State *L, int index, const char *name)
{
	getFieldAndCheckType<lua_Integer>(L, index, name);
	int32_t integer = static_cast<int32_t>(lua_tointeger(L, -1));
	lua_pop(L, 1);

	return integer;
}

template <> uint32_t LuaUtils::retrieveField<uint32_t>(lua_State *L, int index, const char *name)
{
	getFieldAndCheckType<lua_Integer>(L, index, name);

	lua_Integer luaInteger = lua_tointeger(L, -1);
	LuaDebug::assert(L, luaInteger >= 0, "Integer number cannot be negative");

	uint32_t integer = static_cast<uint32_t>(luaInteger);
	lua_pop(L, 1);

	return integer;
}

template <> const char *LuaUtils::retrieveField(lua_State *L, int index, const char *name)
{
	getFieldAndCheckType<const char *>(L, index, name);
	const char *string = static_cast<const char *>(lua_tostring(L, -1));
	lua_pop(L, 1);

	return string;
}

void LuaUtils::retrieveField(lua_State *L, int index, const char *name, char *dest, size_t *length)
{
	getFieldAndCheckType<const char *>(L, index, name);
	const char *src = static_cast<const char *>(lua_tolstring(L, -1, length));
	strcpy(dest, src);
	lua_pop(L, 1);
}

template <> bool LuaUtils::retrieveField<bool>(lua_State *L, int index, const char *name)
{
	lua_getfield(L, index, name);
	LuaDebug::assert(L, lua_isboolean(L, -1), "Cannot retrieve a boolean in table field \"%s\"", name);

	bool boolean = static_cast<bool>(lua_toboolean(L, -1));
	lua_pop(L, 1);

	return boolean;
}

void LuaUtils::retrieveFieldTable(lua_State *L, int index, const char *name)
{
	lua_getfield(L, index, name);
	LuaDebug::assert(L, lua_istable(L, -1), "Cannot retrieve a table in table field \"%s\"", name);
}

void LuaUtils::retrieveFieldFunction(lua_State *L, int index, const char *name)
{
	lua_getfield(L, index, name);
	LuaDebug::assert(L, lua_istable(L, -1), "Cannot retrieve a function in table field \"%s\"", name);
}

void LuaUtils::retrieveFieldLightUserData(lua_State *L, int index, const char *name)
{
	lua_getfield(L, index, name);
	LuaDebug::assert(L, lua_islightuserdata(L, -1), "Cannot retrieve a light userdata in table field \"%s\"", name);
}

template <class T>
void LuaUtils::retrieveFieldArray(lua_State *L, int index, const char *name, int arrayIndex, int length, T *array)
{
	lua_getfield(L, index, name);
	LuaDebug::assert(L, lua_rawlen(L, index) >= arrayIndex + length, "Expecting an array of a minimum length of %d", arrayIndex + length);
	for (int i = 0; i < length; i++)
	{
		lua_rawgeti(L, index, arrayIndex + i);
		array[i] = retrieve<T>(L, -1);
	}
}

///////////////////////////////////////////////////////////
// TRY RETRIEVE FIELD FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

template <class T> bool tryGetField(lua_State *L, int index, const char *name, T &value) { return false; }

template <> bool tryGetField<lua_Number>(lua_State *L, int index, const char *name, lua_Number &value)
{
	lua_getfield(L, index, name);
	if (lua_isnumber(L, -1))
	{
		value = lua_tonumber(L, -1);
		lua_pop(L, 1);
		return true;
	}
	return false;
}

template <> bool tryGetField<lua_Integer>(lua_State *L, int index, const char *name, lua_Integer &value)
{
	lua_getfield(L, index, name);
	if (lua_isinteger(L, -1))
	{
		value = lua_tointeger(L, -1);
		lua_pop(L, 1);
		return true;
	}
	return false;
}

}

template <> bool LuaUtils::tryRetrieveField<double>(lua_State *L, int index, const char *name, double &value)
{
	lua_Number number;

	bool retrieved = tryGetField<lua_Number>(L, index, name, number);
	if (retrieved)
		value = static_cast<double>(number);

	return retrieved;
}

template <> bool LuaUtils::tryRetrieveField<float>(lua_State *L, int index, const char *name, float &value)
{
	lua_Number number;

	bool retrieved = tryGetField<lua_Number>(L, index, name, number);
	if (retrieved)
		value = static_cast<float>(number);

	return retrieved;
}

template <> bool LuaUtils::tryRetrieveField<int64_t>(lua_State *L, int index, const char *name, int64_t &value)
{
	lua_Integer integer;

	bool retrieved = tryGetField<lua_Integer>(L, index, name, integer);
	if (retrieved)
		value = static_cast<int64_t>(integer);

	return retrieved;
}

template <> bool LuaUtils::tryRetrieveField<uint64_t>(lua_State *L, int index, const char *name, uint64_t &value)
{
	lua_Integer integer;

	bool retrieved = tryGetField<lua_Integer>(L, index, name, integer);
	if (retrieved)
	{
		LuaDebug::assert(L, integer >= 0, "Integer number cannot be negative");
		value = static_cast<uint64_t>(integer);
	}

	return retrieved;
}

template <> bool LuaUtils::tryRetrieveField<int32_t>(lua_State *L, int index, const char *name, int32_t &value)
{
	lua_Integer integer;

	bool retrieved = tryGetField<lua_Integer>(L, index, name, integer);
	if (retrieved)
		value = static_cast<int32_t>(integer);

	return retrieved;
}

template <> bool LuaUtils::tryRetrieveField<uint32_t>(lua_State *L, int index, const char *name, uint32_t &value)
{
	lua_Integer integer;

	bool retrieved = tryGetField<lua_Integer>(L, index, name, integer);
	if (retrieved)
	{
		LuaDebug::assert(L, integer >= 0, "Integer number cannot be negative");
		value = static_cast<uint32_t>(integer);
	}

	return retrieved;
}

bool LuaUtils::tryRetrieveField(lua_State *L, int index, const char *name, const char *value)
{
	ASSERT(value);

	lua_getfield(L, index, name);
	if (lua_isstring(L, -1))
	{
		value = lua_tostring(L, -1);
		lua_pop(L, 1);
		return true;
	}
	return false;
}

template <> bool LuaUtils::tryRetrieveField<bool>(lua_State *L, int index, const char *name, bool &value)
{
	lua_getfield(L, index, name);
	if (lua_isboolean(L, -1))
	{
		value = lua_toboolean(L, -1);
		lua_pop(L, 1);
		return true;
	}
	return false;
}

bool LuaUtils::tryRetrieveFieldTable(lua_State *L, int index, const char *name)
{
	lua_getfield(L, index, name);
	if (lua_istable(L, -1))
		return true;
	return false;
}

bool LuaUtils::tryRetrieveFieldFunction(lua_State *L, int index, const char *name)
{
	lua_getfield(L, index, name);
	if (lua_isfunction(L, -1))
		return true;
	return false;
}

bool LuaUtils::tryRetrieveFieldLightUserData(lua_State *L, int index, const char *name)
{
	lua_getfield(L, index, name);
	if (lua_islightuserdata(L, -1))
		return true;
	return false;
}

template <class T>
bool LuaUtils::tryRetrieveFieldArray(lua_State *L, int index, const char *name, int arrayIndex, int length, T *array)
{
	lua_getfield(L, index, name);
	if (lua_istable(L, -1))
	{
		retrieveArray<T>(L, -1, arrayIndex, length, array);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////
// RETRIEVE GLOBAL FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

template <class T> void getGlobalAndCheckType(lua_State *L, const char *name) { }

template <> void getGlobalAndCheckType<lua_Number>(lua_State *L, const char *name)
{
	lua_getglobal(L, name);
	LuaDebug::assert(L, lua_isnumber(L, -1), "Cannot retrieve a number with global name \"%s\"", name);
}

template <> void getGlobalAndCheckType<lua_Integer>(lua_State *L, const char *name)
{
	lua_getglobal(L, name);
	LuaDebug::assert(L, lua_isinteger(L, -1), "Cannot retrieve an integer with global name \"%s\"", name);
}

template <> void getGlobalAndCheckType<const char *>(lua_State *L, const char *name)
{
	lua_getglobal(L, name);
	LuaDebug::assert(L, lua_isstring(L, -1), "Cannot retrieve a string with global name \"%s\"", name);
}

}

template <> double LuaUtils::retrieveGlobal<double>(lua_State *L, const char *name)
{
	getGlobalAndCheckType<lua_Number>(L, name);
	double number = static_cast<double>(lua_tonumber(L, -1));
	lua_pop(L, 1);

	return number;
}

template <> float LuaUtils::retrieveGlobal<float>(lua_State *L, const char *name)
{
	getGlobalAndCheckType<lua_Number>(L, name);
	float number = static_cast<float>(lua_tonumber(L, -1));
	lua_pop(L, 1);

	return number;
}

template <> int64_t LuaUtils::retrieveGlobal<int64_t>(lua_State *L, const char *name)
{
	getGlobalAndCheckType<lua_Integer>(L, name);
	int64_t integer = static_cast<int64_t>(lua_tointeger(L, -1));
	lua_pop(L, 1);

	return integer;
}

template <> uint64_t LuaUtils::retrieveGlobal<uint64_t>(lua_State *L, const char *name)
{
	getGlobalAndCheckType<lua_Integer>(L, name);

	lua_Integer luaInteger = lua_tointeger(L, -1);
	LuaDebug::assert(L, luaInteger >= 0, "Integer number cannot be negative");

	uint64_t integer = static_cast<uint64_t>(luaInteger);
	lua_pop(L, 1);

	return integer;
}

template <> int32_t LuaUtils::retrieveGlobal<int32_t>(lua_State *L, const char *name)
{
	getGlobalAndCheckType<lua_Integer>(L, name);
	int32_t integer = static_cast<int32_t>(lua_tointeger(L, -1));
	lua_pop(L, 1);

	return integer;
}

template <> uint32_t LuaUtils::retrieveGlobal<uint32_t>(lua_State *L, const char *name)
{
	getGlobalAndCheckType<lua_Integer>(L, name);

	lua_Integer luaInteger = lua_tointeger(L, -1);
	LuaDebug::assert(L, luaInteger >= 0, "Integer number cannot be negative");

	uint32_t integer = static_cast<uint32_t>(luaInteger);
	lua_pop(L, 1);

	return integer;
}

template <> const char *LuaUtils::retrieveGlobal(lua_State *L, const char *name)
{
	getGlobalAndCheckType<const char *>(L, name);
	const char *string = static_cast<const char *>(lua_tostring(L, -1));
	lua_pop(L, 1);

	return string;
}

void LuaUtils::retrieveGlobal(lua_State *L, const char *name, char *dest, size_t *length)
{
	getGlobalAndCheckType<const char *>(L, name);
	const char *src = static_cast<const char *>(lua_tolstring(L, -1, length));
	strcpy(dest, src);
	lua_pop(L, 1);
}

template <> bool LuaUtils::retrieveGlobal<bool>(lua_State *L, const char *name)
{
	lua_getglobal(L, name);
	LuaDebug::assert(L, lua_isboolean(L, -1), "Cannot retrieve a boolean with global name \"%s\"", name);

	bool boolean = static_cast<bool>(lua_toboolean(L, -1));
	lua_pop(L, 1);

	return boolean;
}

void LuaUtils::retrieveGlobalTable(lua_State *L, const char *name)
{
	lua_getglobal(L, name);
	LuaDebug::assert(L, lua_istable(L, -1), "Cannot retrieve a table with global name \"%s\"", name);
}

void LuaUtils::retrieveGlobalFunction(lua_State *L, const char *name)
{
	lua_getglobal(L, name);
	LuaDebug::assert(L, lua_isfunction(L, -1), "Cannot retrieve a function with global name \"%s\"", name);
}

void LuaUtils::retrieveGlobalLightUserData(lua_State *L, const char *name)
{
	lua_getglobal(L, name);
	LuaDebug::assert(L, lua_islightuserdata(L, -1), "Cannot retrieve a light userdata with global name \"%s\"", name);
}

template <class T>
void LuaUtils::retrieveGlobalArray(lua_State *L, const char *name, int arrayIndex, int length, T *array)
{
	lua_getglobal(L, name);
	LuaDebug::assert(L, lua_istable(L, -1), "Cannot retrieve a table with global name \"%s\"", name);
	retrieveArray<T>(L, -1, arrayIndex, length, array);
}

///////////////////////////////////////////////////////////
// TRY RETRIEVE GLOBAL FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

template <class T> bool tryGetGlobal(lua_State *L, const char *name, T &value) { return false; }

template <> bool tryGetGlobal<lua_Number>(lua_State *L, const char *name, lua_Number &value)
{
	lua_getglobal(L, name);
	if (lua_isnumber(L, -1))
	{
		value = lua_tonumber(L, -1);
		lua_pop(L, 1);
		return true;
	}
	return false;
}

template <> bool tryGetGlobal<lua_Integer>(lua_State *L, const char *name, lua_Integer &value)
{
	lua_getglobal(L, name);
	if (lua_isinteger(L, -1))
	{
		value = lua_tointeger(L, -1);
		lua_pop(L, 1);
		return true;
	}
	return false;
}

}

template <> bool LuaUtils::tryRetrieveGlobal<double>(lua_State *L, const char *name, double &value)
{
	lua_Number number;

	bool retrieved = tryGetGlobal<lua_Number>(L, name, number);
	if (retrieved)
		value = static_cast<double>(number);

	return retrieved;
}

template <> bool LuaUtils::tryRetrieveGlobal<float>(lua_State *L, const char *name, float &value)
{
	lua_Number number;

	bool retrieved = tryGetGlobal<lua_Number>(L, name, number);
	if (retrieved)
		value = static_cast<float>(number);

	return retrieved;
}

template <> bool LuaUtils::tryRetrieveGlobal<int64_t>(lua_State *L, const char *name, int64_t &value)
{
	lua_Integer integer;

	bool retrieved = tryGetGlobal<lua_Integer>(L, name, integer);
	if (retrieved)
		value = static_cast<int64_t>(integer);

	return retrieved;
}

template <> bool LuaUtils::tryRetrieveGlobal<uint64_t>(lua_State *L, const char *name, uint64_t &value)
{
	lua_Integer integer;

	bool retrieved = tryGetGlobal<lua_Integer>(L, name, integer);
	if (retrieved)
	{
		LuaDebug::assert(L, integer >= 0, "Integer number cannot be negative");
		value = static_cast<uint64_t>(integer);
	}

	return retrieved;
}

template <> bool LuaUtils::tryRetrieveGlobal<int32_t>(lua_State *L, const char *name, int32_t &value)
{
	lua_Integer integer;

	bool retrieved = tryGetGlobal<lua_Integer>(L, name, integer);
	if (retrieved)
		value = static_cast<int32_t>(integer);

	return retrieved;
}

template <> bool LuaUtils::tryRetrieveGlobal<uint32_t>(lua_State *L, const char *name, uint32_t &value)
{
	lua_Integer integer;

	bool retrieved = tryGetGlobal<lua_Integer>(L, name, integer);
	if (retrieved)
	{
		LuaDebug::assert(L, integer >= 0, "Integer number cannot be negative");
		value = static_cast<uint32_t>(integer);
	}

	return retrieved;
}

bool LuaUtils::tryRetrieveGlobal(lua_State *L, const char *name, const char *value)
{
	ASSERT(value);

	lua_getglobal(L, name);
	if (lua_isstring(L, -1))
	{
		value = lua_tostring(L, -1);
		lua_pop(L, 1);
		return true;
	}
	return false;
}

template <> bool LuaUtils::tryRetrieveGlobal<bool>(lua_State *L, const char *name, bool &value)
{
	lua_getglobal(L, name);
	if (lua_isboolean(L, -1))
	{
		value = lua_toboolean(L, -1);
		lua_pop(L, 1);
		return true;
	}
	return false;
}

bool LuaUtils::tryRetrieveGlobalTable(lua_State *L, const char *name)
{
	lua_getglobal(L, name);
	if (lua_istable(L, -1))
		return true;
	return false;
}

bool LuaUtils::tryRetrieveGlobalFunction(lua_State *L, const char *name)
{
	lua_getglobal(L, name);
	if (lua_isfunction(L, -1))
		return true;
	return false;
}

bool LuaUtils::tryRetrieveGlobalLightUserData(lua_State *L, const char *name)
{
	lua_getglobal(L, name);
	if (lua_islightuserdata(L, -1))
		return true;
	return false;
}

template <class T>
bool LuaUtils::tryRetrieveGlobalArray(lua_State *L, const char *name, int arrayIndex, int length, T *array)
{
	lua_getglobal(L, name);
	if (lua_istable(L, -1))
	{
		retrieveArray<T>(L, -1, arrayIndex, length, array);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////
// PUSH FUNCTIONS
///////////////////////////////////////////////////////////

void LuaUtils::push(lua_State *L)
{
	lua_pushnil(L);
}

void LuaUtils::push(lua_State *L, double number)
{
	lua_pushnumber(L, number);
}

void LuaUtils::push(lua_State *L, float number)
{
	lua_pushnumber(L, static_cast<lua_Number>(number));
}

void LuaUtils::push(lua_State *L, int64_t integer)
{
	lua_pushinteger(L, integer);
}

void LuaUtils::push(lua_State *L, uint64_t integer)
{
	lua_pushinteger(L, static_cast<lua_Integer>(integer));
}

void LuaUtils::push(lua_State *L, int32_t integer)
{
	lua_pushinteger(L, integer);
}

void LuaUtils::push(lua_State *L, uint32_t integer)
{
	lua_pushinteger(L, static_cast<lua_Integer>(integer));
}

void LuaUtils::push(lua_State *L, const char *string)
{
	lua_pushstring(L, string);
}

void LuaUtils::push(lua_State *L, int (*func) (lua_State *L))
{
	lua_pushcfunction(L, func);
}

void LuaUtils::push(lua_State *L, bool boolean)
{
	lua_pushboolean(L, boolean);
}

void LuaUtils::push(lua_State *L, void *lightuserdata)
{
	lua_pushlightuserdata(L, lightuserdata);
}

template <class T>
void LuaUtils::pushArray(lua_State *L, int index, int arrayIndex, int length, T *array)
{
	for (int i = 0; i < length; i++)
	{
		push(L, array[i]);
		lua_rawseti(L, index, arrayIndex + i);
	}
}

///////////////////////////////////////////////////////////
// PUSH FIELD FUNCTIONS
///////////////////////////////////////////////////////////

void LuaUtils::pushField(lua_State *L, const char *name)
{
	lua_pushnil(L);
	lua_setfield(L, -2, name);
}

void LuaUtils::pushField(lua_State *L, const char *name, double number)
{
	lua_pushnumber(L, number);
	lua_setfield(L, -2, name);
}

void LuaUtils::pushField(lua_State *L, const char *name, float number)
{
	lua_pushnumber(L, static_cast<lua_Number>(number));
	lua_setfield(L, -2, name);
}

void LuaUtils::pushField(lua_State *L, const char *name, int64_t integer)
{
	lua_pushinteger(L, integer);
	lua_setfield(L, -2, name);
}

void LuaUtils::pushField(lua_State *L, const char *name, uint64_t integer)
{
	lua_pushinteger(L, static_cast<lua_Integer>(integer));
	lua_setfield(L, -2, name);
}

void LuaUtils::pushField(lua_State *L, const char *name, int32_t integer)
{
	lua_pushinteger(L, integer);
	lua_setfield(L, -2, name);
}

void LuaUtils::pushField(lua_State *L, const char *name, uint32_t integer)
{
	lua_pushinteger(L, static_cast<lua_Integer>(integer));
	lua_setfield(L, -2, name);
}

void LuaUtils::pushField(lua_State *L, const char *name, const char *string)
{
	lua_pushstring(L, string);
	lua_setfield(L, -2, name);
}

void LuaUtils::pushField(lua_State *L, const char *name, int (*func) (lua_State *L))
{
	lua_pushcfunction(L, func);
	lua_setfield(L, -2, name);
}

void LuaUtils::pushField(lua_State *L, const char *name, bool boolean)
{
	lua_pushboolean(L, boolean);
	lua_setfield(L, -2, name);
}

void LuaUtils::pushField(lua_State *L, const char *name, void *lightuserdata)
{
	lua_pushlightuserdata(L, lightuserdata);
	lua_setfield(L, -2, name);
}

template <class T>
void LuaUtils::pushArrayField(lua_State *L, const char *name, int arrayIndex, int length, T *array)
{
	const bool found = tryRetrieveFieldTable(L, -1, name);
	if (found == false)
		lua_createtable(L, length, 0);
	pushArray<T>(L, -2, arrayIndex, length, array);

}

}
