#ifndef CLASS_NCINE_LUAVECTOR4UTILS
#define CLASS_NCINE_LUAVECTOR4UTILS

#include "LuaUtils.h"
#include "Vector4.h"

namespace ncine {

namespace LuaNames {
namespace Vector4 {
	static const char *x = "x";
	static const char *y = "y";
	static const char *z = "z";
	static const char *w = "w";
}}

/// Lua utilities for the `Vector4` template class
template <class T>
class LuaVector4Utils
{
  public:
	static void push(lua_State *L, const Vector4<T> &v);
	static void pushField(lua_State *L, const char *name, const Vector4<T> &v);
	static Vector4<T> retrieve(lua_State *L, int index, int &newIndex);
	static Vector4<T> retrieveTable(lua_State *L, int index);
	static Vector4<T> retrieveArray(lua_State *L, int index);
	static Vector4<T> retrieveParams(lua_State *L, int index);
	static Vector4<T> retrieveTableField(lua_State *L, int index, const char *name);
	static Vector4<T> retrieveArrayField(lua_State *L, int index, const char *name);
};

using LuaVector4fUtils = LuaVector4Utils<float>;
using LuaVector4iUtils = LuaVector4Utils<int>;

template <class T>
void LuaVector4Utils<T>::push(lua_State *L, const Vector4<T> &v)
{
	LuaUtils::createTable(L, 0, 4);
	LuaUtils::pushField(L, LuaNames::Vector4::x, v.x);
	LuaUtils::pushField(L, LuaNames::Vector4::y, v.y);
	LuaUtils::pushField(L, LuaNames::Vector4::z, v.z);
	LuaUtils::pushField(L, LuaNames::Vector4::w, v.w);
}

template <class T>
void LuaVector4Utils<T>::pushField(lua_State *L, const char *name, const Vector4<T> &v)
{
	push(L, v);
	LuaUtils::setField(L, -2, name);
}

template <class T>
Vector4<T> LuaVector4Utils<T>::retrieve(lua_State *L, int index, int &newIndex)
{
	if (LuaUtils::isTable(L, index))
	{
		newIndex = index;
		return retrieveTable(L, index);
	}
	else
	{
		newIndex = index - 3;
		return retrieveParams(L, newIndex);
	}
}

template <class T>
Vector4<T> LuaVector4Utils<T>::retrieveTable(lua_State *L, int index)
{
	const T x = LuaUtils::retrieveField<T>(L, index, LuaNames::Vector4::x);
	const T y = LuaUtils::retrieveField<T>(L, index, LuaNames::Vector4::y);
	const T z = LuaUtils::retrieveField<T>(L, index, LuaNames::Vector4::z);
	const T w = LuaUtils::retrieveField<T>(L, index, LuaNames::Vector4::w);
	return Vector4<T>(x, y, z, w);
}

template <class T>
Vector4<T> LuaVector4Utils<T>::retrieveArray(lua_State *L, int index)
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

	LuaUtils::rawGeti(L, index, 4);
	const T w = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	return Vector4<T>(x, y, z, w);
}

template <class T>
Vector4<T> LuaVector4Utils<T>::retrieveParams(lua_State *L, int index)
{
	const T x = LuaUtils::retrieve<T>(L, index);
	const T y = LuaUtils::retrieve<T>(L, index + 1);
	const T z = LuaUtils::retrieve<T>(L, index + 2);
	const T w = LuaUtils::retrieve<T>(L, index + 3);
	return Vector4<T>(x, y, z, w);
}

template <class T>
Vector4<T> LuaVector4Utils<T>::retrieveTableField(lua_State *L, int index, const char *name)
{
	LuaUtils::retrieveFieldTable(L, index, name);
	const T x = LuaUtils::retrieveField<T>(L, -1, LuaNames::Vector4::x);
	const T y = LuaUtils::retrieveField<T>(L, -1, LuaNames::Vector4::y);
	const T z = LuaUtils::retrieveField<T>(L, -1, LuaNames::Vector4::z);
	const T w = LuaUtils::retrieveField<T>(L, -1, LuaNames::Vector4::w);
	LuaUtils::pop(L);

	return Vector4<T>(x, y, z, w);
}

template <class T>
Vector4<T> LuaVector4Utils<T>::retrieveArrayField(lua_State *L, int index, const char *name)
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

	LuaUtils::rawGeti(L, -1, 4);
	const T w = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::pop(L);

	return Vector4<T>(x, y, z, w);
}

}

#endif
