#ifndef CLASS_NCINE_LUAVECTOR3UTILS
#define CLASS_NCINE_LUAVECTOR3UTILS

#include "LuaUtils.h"
#include "Vector3.h"

namespace ncine {

namespace LuaNames {
namespace Vector3 {
	static const char *x = "x";
	static const char *y = "y";
	static const char *z = "z";
}}

/// Lua utilities for the `Vector3` template class
template <class T>
class LuaVector3Utils
{
  public:
	static void push(lua_State *L, const Vector3<T> &v);
	static Vector3<T> retrieve(lua_State *L, int index, int &newIndex);
	static Vector3<T> retrieveTable(lua_State *L, int index);
	static Vector3<T> retrieveArray(lua_State *L, int index);
	static Vector3<T> retrieveParams(lua_State *L, int index);
	static Vector3<T> retrieveTableField(lua_State *L, int index, const char *name);
	static Vector3<T> retrieveArrayField(lua_State *L, int index, const char *name);
};

using LuaVector3fUtils = LuaVector3Utils<float>;
using LuaVector3iUtils = LuaVector3Utils<int>;

template <class T>
void LuaVector3Utils<T>::push(lua_State *L, const Vector3<T> &v)
{
	LuaUtils::createTable(L, 3, 0);
	LuaUtils::pushField(L, LuaNames::Vector3::x, v.x);
	LuaUtils::pushField(L, LuaNames::Vector3::y, v.y);
	LuaUtils::pushField(L, LuaNames::Vector3::z, v.z);
}

template <class T>
Vector3<T> LuaVector3Utils<T>::retrieve(lua_State *L, int index, int &newIndex)
{
	if (LuaUtils::isTable(L, index))
	{
		newIndex = index;
		return retrieveTable(L, index);
	}
	else
	{
		newIndex = index - 2;
		return retrieveParams(L, newIndex);
	}
}

template <class T>
Vector3<T> LuaVector3Utils<T>::retrieveTable(lua_State *L, int index)
{
	const T x = LuaUtils::retrieveField<T>(L, index, LuaNames::Vector3::x);
	const T y = LuaUtils::retrieveField<T>(L, index, LuaNames::Vector3::y);
	const T z = LuaUtils::retrieveField<T>(L, index, LuaNames::Vector3::z);
	return Vector3<T>(x, y, z);
}

template <class T>
Vector3<T> LuaVector3Utils<T>::retrieveArray(lua_State *L, int index)
{
	LuaUtils::rawGeti(L, index, 1);
	const T x = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, index, 2);
	const T y = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, index, 3);
	const T z = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	return Vector3<T>(x, y, z);
}

template <class T>
Vector3<T> LuaVector3Utils<T>::retrieveParams(lua_State *L, int index)
{
	const T x = LuaUtils::retrieve<T>(L, index);
	const T y = LuaUtils::retrieve<T>(L, index + 1);
	const T z = LuaUtils::retrieve<T>(L, index + 2);
	return Vector3<T>(x, y, z);
}

template <class T>
Vector3<T> LuaVector3Utils<T>::retrieveTableField(lua_State *L, int index, const char *name)
{
	LuaUtils::retrieveFieldTable(L, index, name);
	const T x = LuaUtils::retrieveField<T>(L, -1, LuaNames::Vector3::x);
	const T y = LuaUtils::retrieveField<T>(L, -1, LuaNames::Vector3::y);
	const T z = LuaUtils::retrieveField<T>(L, -1, LuaNames::Vector3::z);
	LuaUtils::pop(L);

	return Vector3<T>(x, y, z);
}

template <class T>
Vector3<T> LuaVector3Utils<T>::retrieveArrayField(lua_State *L, int index, const char *name)
{
	LuaUtils::retrieveFieldTable(L, index, name);

	LuaUtils::rawGeti(L, -1, 1);
	const T x = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, -1, 2);
	const T y = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, -1, 3);
	const T z = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::pop(L);

	return Vector3<T>(x, y, z);
}

}

#endif
