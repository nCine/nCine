#ifndef CLASS_NCINE_LUARECTUTILS
#define CLASS_NCINE_LUARECTUTILS

#include "LuaUtils.h"
#include "Rect.h"

namespace ncine {

namespace LuaNames {
namespace Rect {
	static const char *x = "x";
	static const char *y = "y";
	static const char *w = "w";
	static const char *h = "h";
}}

/// Lua utilities for the `Rect` template class
template <class T>
class LuaRectUtils
{
  public:
	static void push(lua_State *L, const Rect<T> &rect);
	static void pushField(lua_State *L, const char *name, const Rect<T> &rect);
	static Rect<T> retrieve(lua_State *L, int index, int &newIndex);
	static Rect<T> retrieveTable(lua_State *L, int index);
	static Rect<T> retrieveArray(lua_State *L, int index);
	static Rect<T> retrieveParams(lua_State *L, int index);
	static Rect<T> retrieveTableField(lua_State *L, int index, const char *name);
	static Rect<T> retrieveArrayField(lua_State *L, int index, const char *name);
};

using LuaRectfUtils = LuaRectUtils<float>;
using LuaRectiUtils = LuaRectUtils<int>;

template <class T>
void LuaRectUtils<T>::push(lua_State *L, const Rect<T> &rect)
{
	LuaUtils::createTable(L, 4, 0);
	LuaUtils::pushField(L, LuaNames::Rect::x, rect.x);
	LuaUtils::pushField(L, LuaNames::Rect::y, rect.y);
	LuaUtils::pushField(L, LuaNames::Rect::w, rect.w);
	LuaUtils::pushField(L, LuaNames::Rect::h, rect.h);
}

template <class T>
void LuaRectUtils<T>::pushField(lua_State *L, const char *name, const Rect<T> &rect)
{
	push(L, rect);
	LuaUtils::setField(L, -2, name);
}

template <class T>
Rect<T> LuaRectUtils<T>::retrieve(lua_State *L, int index, int &newIndex)
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
Rect<T> LuaRectUtils<T>::retrieveTable(lua_State *L, int index)
{
	const T x = LuaUtils::retrieveField<T>(L, index, LuaNames::Rect::x);
	const T y = LuaUtils::retrieveField<T>(L, index, LuaNames::Rect::y);
	const T w = LuaUtils::retrieveField<T>(L, index, LuaNames::Rect::w);
	const T h = LuaUtils::retrieveField<T>(L, index, LuaNames::Rect::h);
	return Rect<T>(x, y, w, h);
}

template <class T>
Rect<T> LuaRectUtils<T>::retrieveArray(lua_State *L, int index)
{
	LuaUtils::rawGeti(L, index, 1);
	const T x = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, index, 2);
	const T y = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, index, 3);
	const T w = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, index, 4);
	const T h = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	return Rect<T>(x, y, w, h);
}

template <class T>
Rect<T> LuaRectUtils<T>::retrieveParams(lua_State *L, int index)
{
	const T x = LuaUtils::retrieve<T>(L, index);
	const T y = LuaUtils::retrieve<T>(L, index + 1);
	const T w = LuaUtils::retrieve<T>(L, index + 2);
	const T h = LuaUtils::retrieve<T>(L, index + 3);
	return Rect<T>(x, y, w, h);
}

template <class T>
Rect<T> LuaRectUtils<T>::retrieveTableField(lua_State *L, int index, const char *name)
{
	LuaUtils::retrieveFieldTable(L, index, name);
	const T x = LuaUtils::retrieveField<T>(L, -1, LuaNames::Rect::x);
	const T y = LuaUtils::retrieveField<T>(L, -1, LuaNames::Rect::y);
	const T w = LuaUtils::retrieveField<T>(L, -1, LuaNames::Rect::w);
	const T h = LuaUtils::retrieveField<T>(L, -1, LuaNames::Rect::h);
	LuaUtils::pop(L);

	return Rect<T>(x, y, w, h);
}

template <class T>
Rect<T> LuaRectUtils<T>::retrieveArrayField(lua_State *L, int index, const char *name)
{
	LuaUtils::retrieveFieldTable(L, index, name);

	LuaUtils::rawGeti(L, -1, 1);
	const T x = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, -1, 2);
	const T y = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, -1, 3);
	const T w = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, -1, 4);
	const T h = LuaUtils::retrieve<T>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::pop(L);

	return Rect<T>(x, y, w, h);
}

}

#endif
