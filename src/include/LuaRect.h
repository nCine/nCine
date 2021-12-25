#ifndef CLASS_NCINE_LUARECT
#define CLASS_NCINE_LUARECT

#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaRectUtils.h"
#include "LuaVector2Utils.h"

namespace ncine {

namespace LuaNames {
namespace Rect {
	static const char *Rect = "rect";

	static const char *create = "create";
	static const char *fromCenterSize = "create_center_size";
	static const char *fromMinMax = "create_min_max";

	static const char *center = "get_center";
	static const char *min = "get_min";
	static const char *max = "get_max";

	static const char *setCenter = "set_center";
	static const char *setMinMax = "set_min_max";

	static const char *invertSize = "invert_size";

	static const char *containsPoint = "contains_point";
	static const char *contains = "contains";

	static const char *overlaps = "overlaps";
	static const char *intersect = "intersect";
}}

/// Lua bindings around the `Rect` template class
template <class T>
class LuaRect
{
  public:
	static void expose(lua_State *L);

  private:
	static int create(lua_State *L);
	static int fromCenterSize(lua_State *L);
	static int fromMinMax(lua_State *L);

	static int center(lua_State *L);
	static int min(lua_State *L);
	static int max(lua_State *L);

	static int setCenter(lua_State *L);
	static int setMinMax(lua_State *L);

	static int invertSize(lua_State *L);

	static int containsPoint(lua_State *L);
	static int contains(lua_State *L);

	static int overlaps(lua_State *L);
	static int intersect(lua_State *L);
};

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

template <class T>
void LuaRect<T>::expose(lua_State *L)
{
	lua_createtable(L, 0, 13);

	LuaUtils::addFunction(L, LuaNames::Rect::create, create);
	LuaUtils::addFunction(L, LuaNames::Rect::fromCenterSize, fromCenterSize);
	LuaUtils::addFunction(L, LuaNames::Rect::fromMinMax, fromMinMax);

	LuaUtils::addFunction(L, LuaNames::Rect::center, center);
	LuaUtils::addFunction(L, LuaNames::Rect::min, min);
	LuaUtils::addFunction(L, LuaNames::Rect::max, max);

	LuaUtils::addFunction(L, LuaNames::Rect::setCenter, setCenter);
	LuaUtils::addFunction(L, LuaNames::Rect::setMinMax, setMinMax);

	LuaUtils::addFunction(L, LuaNames::Rect::invertSize, invertSize);

	LuaUtils::addFunction(L, LuaNames::Rect::containsPoint, containsPoint);
	LuaUtils::addFunction(L, LuaNames::Rect::contains, contains);

	LuaUtils::addFunction(L, LuaNames::Rect::overlaps, overlaps);
	LuaUtils::addFunction(L, LuaNames::Rect::intersect, intersect);

	lua_setfield(L, -2, LuaNames::Rect::Rect);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

template <class T>
int LuaRect<T>::create(lua_State *L)
{
	const T x = LuaUtils::retrieve<T>(L, -4);
	const T y = LuaUtils::retrieve<T>(L, -3);
	const T w = LuaUtils::retrieve<T>(L, -2);
	const T h = LuaUtils::retrieve<T>(L, -1);

	const Rect<T> rect(x, y, w, h);

	LuaRectUtils<T>::push(L, rect);

	return 1;
}

template <class T>
int LuaRect<T>::fromCenterSize(lua_State *L)
{
	const T x = LuaUtils::retrieve<T>(L, -4);
	const T y = LuaUtils::retrieve<T>(L, -3);
	const T w = LuaUtils::retrieve<T>(L, -2);
	const T h = LuaUtils::retrieve<T>(L, -1);

	const Rect<T> rect = Rect<T>::fromCenterSize(x, y, w, h);

	LuaRectUtils<T>::push(L, rect);

	return 1;
}

template <class T>
int LuaRect<T>::fromMinMax(lua_State *L)
{
	const T minX = LuaUtils::retrieve<T>(L, -4);
	const T minY = LuaUtils::retrieve<T>(L, -3);
	const T maxX = LuaUtils::retrieve<T>(L, -2);
	const T maxY = LuaUtils::retrieve<T>(L, -1);

	const Rect<T> rect = Rect<T>::fromMinMax(minX, minY, maxX, maxY);

	LuaRectUtils<T>::push(L, rect);

	return 1;
}

template <class T>
int LuaRect<T>::center(lua_State *L)
{
	int rectIndex = 0;
	const Rect<T> rect = LuaRectUtils<T>::retrieve(L, -1, rectIndex);
	const Vector2<T> center = rect.center();

	LuaVector2Utils<T>::push(L, center);

	return 1;
}

template <class T>
int LuaRect<T>::min(lua_State *L)
{
	int rectIndex = 0;
	const Rect<T> rect = LuaRectUtils<T>::retrieve(L, -1, rectIndex);
	const Vector2<T> min = rect.min();

	LuaVector2Utils<T>::push(L, min);

	return 1;
}

template <class T>
int LuaRect<T>::max(lua_State *L)
{
	int rectIndex = 0;
	const Rect<T> rect = LuaRectUtils<T>::retrieve(L, -1, rectIndex);
	const Vector2<T> max = rect.max();

	LuaVector2Utils<T>::push(L, max);

	return 1;
}

template <class T>
int LuaRect<T>::setCenter(lua_State *L)
{
	Rect<T> rect = LuaRectUtils<T>::retrieveTable(L, -3);
	const Vector2<T> newCenter = LuaVector2Utils<T>::retrieveParams(L, -2);

	rect.setCenter(newCenter);
	LuaRectUtils<T>::push(L, rect);

	return 1;
}

template <class T>
int LuaRect<T>::setMinMax(lua_State *L)
{
	Rect<T> rect = LuaRectUtils<T>::retrieveTable(L, -5);
	const Vector2<T> newMin = LuaVector2Utils<T>::retrieveParams(L, -4);
	const Vector2<T> newMax = LuaVector2Utils<T>::retrieveParams(L, -2);

	rect.setMinMax(newMin, newMax);
	LuaRectUtils<T>::push(L, rect);

	return 1;
}

template <class T>
int LuaRect<T>::invertSize(lua_State *L)
{
	Rect<T> rect = LuaRectUtils<T>::retrieveTable(L, -1);

	rect.invertSize();
	LuaRectUtils<T>::push(L, rect);

	return 1;
}

template <class T>
int LuaRect<T>::containsPoint(lua_State *L)
{
	const Rect<T> rect = LuaRectUtils<T>::retrieveTable(L, -3);
	const Vector2<T> point = LuaVector2Utils<T>::retrieveParams(L, -2);

	const bool contains = rect.contains(point);
	LuaUtils::push(L, contains);

	return 1;
}

template <class T>
int LuaRect<T>::contains(lua_State *L)
{
	const Rect<T> rect = LuaRectUtils<T>::retrieveTable(L, -2);
	const Rect<T> otherRect = LuaRectUtils<T>::retrieveTable(L, -1);

	const bool contains = rect.contains(otherRect);
	LuaUtils::push(L, contains);

	return 1;
}

template <class T>
int LuaRect<T>::overlaps(lua_State *L)
{
	const Rect<T> rect = LuaRectUtils<T>::retrieveTable(L, -2);
	const Rect<T> otherRect = LuaRectUtils<T>::retrieveTable(L, -1);

	const bool overlaps = rect.overlaps(otherRect);
	LuaUtils::push(L, overlaps);

	return 1;
}

template <class T>
int LuaRect<T>::intersect(lua_State *L)
{
	Rect<T> rect = LuaRectUtils<T>::retrieveTable(L, -2);
	const Rect<T> otherRect = LuaRectUtils<T>::retrieveTable(L, -1);

	rect.intersect(otherRect);
	LuaRectUtils<T>::push(L, rect);

	return 1;
}

}

#endif
