#ifndef CLASS_NCINE_LUAVECTOR2
#define CLASS_NCINE_LUAVECTOR2

#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaVector2Utils.h"
#include "LuaDebug.h"

namespace ncine {

namespace LuaNames {
namespace Vector2 {
	static const char *Vector2 = "vec2";
	static const char *create = "create";

	static const char *add = "add";
	static const char *subtract = "sub";
	static const char *multiply = "mul";
	static const char *divide = "div";

	static const char *length = "length";
	static const char *sqrLength = "sqrlength";
	static const char *normalized = "normalized";
	static const char *dot = "dot";
}}

/// Lua bindings around the `Vector2` template class
template <class T>
class LuaVector2
{
  public:
	static void expose(lua_State *L);

  private:
	static int create(lua_State *L);

	static int add(lua_State *L);
	static int subtract(lua_State *L);
	static int multiply(lua_State *L);
	static int divide(lua_State *L);

	static int length(lua_State *L);
	static int sqrLength(lua_State *L);
	static int normalized(lua_State *L);
	static int dot(lua_State *L);
};

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

template <class T>
void LuaVector2<T>::expose(lua_State *L)
{
	lua_newtable(L);

	LuaUtils::addFunction(L, LuaNames::Vector2::create, create);

	LuaUtils::addFunction(L, LuaNames::Vector2::add, add);
	LuaUtils::addFunction(L, LuaNames::Vector2::subtract, subtract);
	LuaUtils::addFunction(L, LuaNames::Vector2::multiply, multiply);
	LuaUtils::addFunction(L, LuaNames::Vector2::divide, divide);

	LuaUtils::addFunction(L, LuaNames::Vector2::length, length);
	LuaUtils::addFunction(L, LuaNames::Vector2::sqrLength, sqrLength);
	LuaUtils::addFunction(L, LuaNames::Vector2::normalized, normalized);
	LuaUtils::addFunction(L, LuaNames::Vector2::dot, dot);

	lua_setfield(L, -2, LuaNames::Vector2::Vector2);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

template <class T>
int LuaVector2<T>::create(lua_State *L)
{
	const float x = LuaUtils::retrieve<float>(L, -2);
	const float y = LuaUtils::retrieve<float>(L, -1);
	const Vector2<T> v(x, y);

	LuaVector2Utils<T>::push(L, v);

	return 1;
}

namespace {

	template <class T>
	bool retrieveVectorsOrScalar(lua_State *L, Vector2<T> &first, Vector2<T> &second, T &scalar)
	{
		bool scalarOp = true;

		if (lua_isnumber(L, -2) && lua_isnumber(L, -1))
			LuaDebug::traceError(L, "Expecting two vec2 tables or a vec2 table and a number");
		else if (lua_isnumber(L, -2) && lua_istable(L, -1))
		{
			scalar = LuaUtils::retrieve<T>(L, -2);
			first = LuaVector2Utils<T>::retrieveTable(L, -1);
		}
		else if (lua_istable(L, -2) && lua_isnumber(L, -1))
		{
			scalar = LuaUtils::retrieve<T>(L, -1);
			lua_pop(L, 1);
			first = LuaVector2Utils<T>::retrieveTable(L, -1);
		}
		else if (lua_istable(L, -2) && lua_istable(L, -1))
		{
			second = LuaVector2Utils<T>::retrieveTable(L, -1);
			lua_pop(L, 1);
			first = LuaVector2Utils<T>::retrieveTable(L, -1);
			lua_pop(L, 1);
			scalarOp = false;
		}
		else
			LuaDebug::traceError(L, "Expecting two vec2 tables or a vec2 table and a number");

		return scalarOp;
	}

}

template <class T>
int LuaVector2<T>::add(lua_State *L)
{
	T scalar = 0.0f;
	Vector2<T> first(0.0f, 0.0f);
	Vector2<T> second(0.0f, 0.0f);

	const bool scalarOp = retrieveVectorsOrScalar(L, first, second, scalar);

	if (scalarOp)
		first += scalar;
	else
		first += second;

	LuaVector2Utils<T>::push(L, first);

	return 1;
}

template <class T>
int LuaVector2<T>::subtract(lua_State *L)
{
	T scalar = 0.0f;
	Vector2<T> first(0.0f, 0.0f);
	Vector2<T> second(0.0f, 0.0f);

	const bool scalarOp = retrieveVectorsOrScalar(L, first, second, scalar);

	if (scalarOp)
		first -= scalar;
	else
		first -= second;

	LuaVector2Utils<T>::push(L, first);

	return 1;
}

template <class T>
int LuaVector2<T>::multiply(lua_State *L)
{
	T scalar = 1.0f;
	Vector2<T> first(1.0f, 1.0f);
	Vector2<T> second(1.0f, 1.0f);

	const bool scalarOp = retrieveVectorsOrScalar(L, first, second, scalar);

	if (scalarOp)
		first *= scalar;
	else
		first *= second;

	LuaVector2Utils<T>::push(L, first);

	return 1;
}

template <class T>
int LuaVector2<T>::divide(lua_State *L)
{
	T scalar = 1.0f;
	Vector2<T> first(1.0f, 1.0f);
	Vector2<T> second(1.0f, 1.0f);

	const bool scalarOp = retrieveVectorsOrScalar(L, first, second, scalar);

	if (scalarOp)
		first /= scalar;
	else
		first /= second;

	LuaVector2Utils<T>::push(L, first);

	return 1;
}

template <class T>
int LuaVector2<T>::length(lua_State *L)
{
	const Vector2<T> v = LuaVector2Utils<T>::retrieveTable(L, -1);

	LuaUtils::push(L, v.length());

	return 1;
}

template <class T>
int LuaVector2<T>::sqrLength(lua_State *L)
{
	const Vector2<T> v = LuaVector2Utils<T>::retrieveTable(L, -1);

	LuaUtils::push(L, v.sqrLength());

	return 1;
}

template <class T>
int LuaVector2<T>::normalized(lua_State *L)
{
	Vector2<T> v = LuaVector2Utils<T>::retrieveTable(L, -1);

	v.normalize();
	LuaVector2Utils<T>::push(L, v);

	return 1;
}

template <class T>
int LuaVector2<T>::dot(lua_State *L)
{
	const Vector2<T> second = LuaVector2Utils<T>::retrieveTable(L, -1);
	lua_pop(L, 1);
	const Vector2<T> first = LuaVector2Utils<T>::retrieveTable(L, -1);
	lua_pop(L, 1);

	const float product = ncine::dot(first, second);
	LuaUtils::push(L, product);

	return 1;
}

}

#endif
