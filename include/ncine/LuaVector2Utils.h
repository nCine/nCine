#ifndef CLASS_NCINE_LUAVECTOR2UTILS
#define CLASS_NCINE_LUAVECTOR2UTILS

#include "LuaUtils.h"
#include "Vector2.h"

namespace ncine {

namespace LuaNames {
namespace Vector2 {
	static const char *x = "x";
	static const char *y = "y";
}}

/// Lua utilities for the `Vector2` template class
template <class T>
class LuaVector2Utils
{
  public:
	static void push(lua_State *L, const Vector2<T> &v);
	static Vector2<T> retrieve(lua_State *L, int index, int &newIndex);
	static Vector2<T> retrieveTable(lua_State *L, int index);
	static Vector2<T> retrieveArray(lua_State *L, int index);
	static Vector2<T> retrieveParams(lua_State *L, int index);
	static Vector2<T> retrieveTableField(lua_State *L, int index, const char *name);
	static Vector2<T> retrieveArrayField(lua_State *L, int index, const char *name);
};

using LuaVector2fUtils = LuaVector2Utils<float>;
using LuaVector2iUtils = LuaVector2Utils<int>;

template <class T>
void LuaVector2Utils<T>::push(lua_State *L, const Vector2<T> &v)
{
	LuaUtils::createTable(L, 2, 0);
	LuaUtils::pushField(L, LuaNames::Vector2::x, v.x);
	LuaUtils::pushField(L, LuaNames::Vector2::y, v.y);
}

template <class T>
Vector2<T> LuaVector2Utils<T>::retrieve(lua_State *L, int index, int &newIndex)
{
	if (LuaUtils::isTable(L, index))
	{
		newIndex = index;
		return retrieveTable(L, index);
	}
	else
	{
		newIndex = index - 1;
		return retrieveParams(L, newIndex);
	}
}

template <class T>
Vector2<T> LuaVector2Utils<T>::retrieveTable(lua_State *L, int index)
{
	const T x = LuaUtils::retrieveField<T>(L, index, LuaNames::Vector2::x);
	const T y = LuaUtils::retrieveField<T>(L, index, LuaNames::Vector2::y);
	return Vector2<T>(x, y);
}

template <class T>
Vector2<T> LuaVector2Utils<T>::retrieveArray(lua_State *L, int index)
{
	LuaUtils::rawGeti(L, index, 1);
	const T x = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, index, 2);
	const T y = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	return Vector2<T>(x, y);
}

template <class T>
Vector2<T> LuaVector2Utils<T>::retrieveParams(lua_State *L, int index)
{
	const T x = LuaUtils::retrieve<T>(L, index);
	const T y = LuaUtils::retrieve<T>(L, index + 1);
	return Vector2<T>(x, y);
}

template <class T>
Vector2<T> LuaVector2Utils<T>::retrieveTableField(lua_State *L, int index, const char *name)
{
	LuaUtils::retrieveFieldTable(L, index, name);
	const T x = LuaUtils::retrieveField<T>(L, -1, LuaNames::Vector2::x);
	const T y = LuaUtils::retrieveField<T>(L, -1, LuaNames::Vector2::y);
	LuaUtils::pop(L);

	return Vector2<T>(x, y);
}

template <class T>
Vector2<T> LuaVector2Utils<T>::retrieveArrayField(lua_State *L, int index, const char *name)
{
	LuaUtils::retrieveFieldTable(L, index, name);

	LuaUtils::rawGeti(L, -1, 1);
	const T x = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, -1, 2);
	const T y = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::pop(L);

	return Vector2<T>(x, y);
}

}

#endif
