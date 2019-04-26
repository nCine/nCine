#ifndef CLASS_NCINE_LUAVECTOR3
#define CLASS_NCINE_LUAVECTOR3

#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaVector3Utils.h"
#include "LuaDebug.h"

namespace ncine {

namespace LuaNames {
namespace Vector3 {
	static const char *Vector3 = "vec3";
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

/// Lua bindings around the `Vector3` template class
template <class T>
class LuaVector3
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
void LuaVector3<T>::expose(lua_State *L)
{
	lua_newtable(L);

	LuaUtils::addFunction(L, LuaNames::Vector3::create, create);

	LuaUtils::addFunction(L, LuaNames::Vector3::add, add);
	LuaUtils::addFunction(L, LuaNames::Vector3::subtract, subtract);
	LuaUtils::addFunction(L, LuaNames::Vector3::multiply, multiply);
	LuaUtils::addFunction(L, LuaNames::Vector3::divide, divide);

	LuaUtils::addFunction(L, LuaNames::Vector3::length, length);
	LuaUtils::addFunction(L, LuaNames::Vector3::sqrLength, sqrLength);
	LuaUtils::addFunction(L, LuaNames::Vector3::normalized, normalized);
	LuaUtils::addFunction(L, LuaNames::Vector3::dot, dot);

	lua_setfield(L, -2, LuaNames::Vector3::Vector3);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

template <class T>
int LuaVector3<T>::create(lua_State *L)
{
	const T x = LuaUtils::retrieve<T>(L, -3);
	const T y = LuaUtils::retrieve<T>(L, -2);
	const T z = LuaUtils::retrieve<T>(L, -1);
	const Vector3f v(x, y, z);

	LuaVector3Utils<T>::push(L, v);

	return 1;
}

namespace {

	template <class T>
	bool retrieveVectorsOrScalar(lua_State *L, Vector3<T> &first, Vector3<T> &second, T &scalar)
	{
		bool scalarOp = true;

		if (lua_isnumber(L, -2) && lua_isnumber(L, -1))
			LuaDebug::traceError(L, "Expecting two vec3 tables or a vec3 table and a number");
		else if (lua_isnumber(L, -2) && lua_istable(L, -1))
		{
			scalar = LuaUtils::retrieve<T>(L, -2);
			first = LuaVector3Utils<T>::retrieveTable(L, -1);
		}
		else if (lua_istable(L, -2) && lua_isnumber(L, -1))
		{
			scalar = LuaUtils::retrieve<T>(L, -1);
			lua_pop(L, 1);
			first = LuaVector3Utils<T>::retrieveTable(L, -1);
		}
		else if (lua_istable(L, -2) && lua_istable(L, -1))
		{
			second = LuaVector3Utils<T>::retrieveTable(L, -1);
			lua_pop(L, 1);
			first = LuaVector3Utils<T>::retrieveTable(L, -1);
			lua_pop(L, 1);
			scalarOp = false;
		}
		else
			LuaDebug::traceError(L, "Expecting two vec3 tables or a vec3 table and a number");

		return scalarOp;
	}

}

template <class T>
int LuaVector3<T>::add(lua_State *L)
{
	T scalar = 0.0f;
	Vector3f first(0.0f, 0.0f, 0.0f);
	Vector3f second(0.0f, 0.0f, 0.0f);

	const bool scalarOp = retrieveVectorsOrScalar(L, first, second, scalar);

	if (scalarOp)
		first += scalar;
	else
		first += second;

	LuaVector3Utils<T>::push(L, first);

	return 1;
}

template <class T>
int LuaVector3<T>::subtract(lua_State *L)
{
	T scalar = 0.0f;
	Vector3f first(0.0f, 0.0f, 0.0f);
	Vector3f second(0.0f, 0.0f, 0.0f);

	const bool scalarOp = retrieveVectorsOrScalar(L, first, second, scalar);

	if (scalarOp)
		first -= scalar;
	else
		first -= second;

	LuaVector3Utils<T>::push(L, first);

	return 1;
}

template <class T>
int LuaVector3<T>::multiply(lua_State *L)
{
	T scalar = 1.0f;
	Vector3f first(1.0f, 1.0f, 1.0f);
	Vector3f second(1.0f, 1.0f, 1.0f);

	const bool scalarOp = retrieveVectorsOrScalar(L, first, second, scalar);

	if (scalarOp)
		first *= scalar;
	else
		first *= second;

	LuaVector3Utils<T>::push(L, first);

	return 1;
}

template <class T>
int LuaVector3<T>::divide(lua_State *L)
{
	T scalar = 1.0f;
	Vector3f first(1.0f, 1.0f, 1.0f);
	Vector3f second(1.0f, 1.0f, 1.0f);

	const bool scalarOp = retrieveVectorsOrScalar(L, first, second, scalar);

	if (scalarOp)
		first /= scalar;
	else
		first /= second;

	LuaVector3Utils<T>::push(L, first);

	return 1;
}

template <class T>
int LuaVector3<T>::length(lua_State *L)
{
	const Vector3f v = LuaVector3Utils<T>::retrieveTable(L, -1);

	LuaUtils::push(L, v.length());

	return 1;
}

template <class T>
int LuaVector3<T>::sqrLength(lua_State *L)
{
	const Vector3f v = LuaVector3Utils<T>::retrieveTable(L, -1);

	LuaUtils::push(L, v.sqrLength());

	return 1;
}

template <class T>
int LuaVector3<T>::normalized(lua_State *L)
{
	Vector3f v = LuaVector3Utils<T>::retrieveTable(L, -1);

	v.normalize();
	LuaVector3Utils<T>::push(L, v);

	return 1;
}

template <class T>
int LuaVector3<T>::dot(lua_State *L)
{
	const Vector3f second = LuaVector3Utils<T>::retrieveTable(L, -1);
	lua_pop(L, 1);
	const Vector3f first = LuaVector3Utils<T>::retrieveTable(L, -1);
	lua_pop(L, 1);

	const T product = ncine::dot(first, second);
	LuaUtils::push(L, product);

	return 1;
}

}

#endif
