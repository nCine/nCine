#ifndef CLASS_NCINE_LUAVECTOR4
#define CLASS_NCINE_LUAVECTOR4

#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaVector4Utils.h"
#include "LuaDebug.h"

namespace ncine {

namespace LuaNames {
namespace Vector4 {
	static const char *Vector4 = "vec4";
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

/// Lua bindings around the `Vector4` template class
template <class T>
class LuaVector4
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
void LuaVector4<T>::expose(lua_State *L)
{
	lua_createtable(L, 0, 9);

	LuaUtils::addFunction(L, LuaNames::Vector4::create, create);

	LuaUtils::addFunction(L, LuaNames::Vector4::add, add);
	LuaUtils::addFunction(L, LuaNames::Vector4::subtract, subtract);
	LuaUtils::addFunction(L, LuaNames::Vector4::multiply, multiply);
	LuaUtils::addFunction(L, LuaNames::Vector4::divide, divide);

	LuaUtils::addFunction(L, LuaNames::Vector4::length, length);
	LuaUtils::addFunction(L, LuaNames::Vector4::sqrLength, sqrLength);
	LuaUtils::addFunction(L, LuaNames::Vector4::normalized, normalized);
	LuaUtils::addFunction(L, LuaNames::Vector4::dot, dot);

	lua_setfield(L, -2, LuaNames::Vector4::Vector4);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

template <class T>
int LuaVector4<T>::create(lua_State *L)
{
	const T x = LuaUtils::retrieve<T>(L, -4);
	const T y = LuaUtils::retrieve<T>(L, -3);
	const T z = LuaUtils::retrieve<T>(L, -2);
	const T w = LuaUtils::retrieve<T>(L, -1);
	const Vector4f v(x, y, z, w);

	LuaVector4Utils<T>::push(L, v);

	return 1;
}

namespace {

	template <class T>
	bool retrieveVectorsOrScalar(lua_State *L, Vector4<T> &first, Vector4<T> &second, T &scalar)
	{
		bool scalarOp = true;

		if (lua_isnumber(L, -2) && lua_isnumber(L, -1))
			LuaDebug::traceError(L, "Expecting two vec4 tables or a vec4 table and a number");
		else if (lua_isnumber(L, -2) && lua_istable(L, -1))
		{
			scalar = LuaUtils::retrieve<T>(L, -2);
			first = LuaVector4Utils<T>::retrieveTable(L, -1);
		}
		else if (lua_istable(L, -2) && lua_isnumber(L, -1))
		{
			scalar = LuaUtils::retrieve<T>(L, -1);
			lua_pop(L, 1);
			first = LuaVector4Utils<T>::retrieveTable(L, -1);
		}
		else if (lua_istable(L, -2) && lua_istable(L, -1))
		{
			second = LuaVector4Utils<T>::retrieveTable(L, -1);
			lua_pop(L, 1);
			first = LuaVector4Utils<T>::retrieveTable(L, -1);
			lua_pop(L, 1);
			scalarOp = false;
		}
		else
			LuaDebug::traceError(L, "Expecting two vec4 tables or a vec4 table and a number");

		return scalarOp;
	}

}

template <class T>
int LuaVector4<T>::add(lua_State *L)
{
	T scalar = 0.0f;
	Vector4f first(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4f second(0.0f, 0.0f, 0.0f, 0.0f);

	const bool scalarOp = retrieveVectorsOrScalar(L, first, second, scalar);

	if (scalarOp)
		first += scalar;
	else
		first += second;

	LuaVector4Utils<T>::push(L, first);

	return 1;
}

template <class T>
int LuaVector4<T>::subtract(lua_State *L)
{
	T scalar = 0.0f;
	Vector4f first(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4f second(0.0f, 0.0f, 0.0f, 0.0f);

	const bool scalarOp = retrieveVectorsOrScalar(L, first, second, scalar);

	if (scalarOp)
		first -= scalar;
	else
		first -= second;

	LuaVector4Utils<T>::push(L, first);

	return 1;
}

template <class T>
int LuaVector4<T>::multiply(lua_State *L)
{
	T scalar = 1.0f;
	Vector4f first(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f second(1.0f, 1.0f, 1.0f, 1.0f);

	const bool scalarOp = retrieveVectorsOrScalar(L, first, second, scalar);

	if (scalarOp)
		first *= scalar;
	else
		first *= second;

	LuaVector4Utils<T>::push(L, first);

	return 1;
}

template <class T>
int LuaVector4<T>::divide(lua_State *L)
{
	T scalar = 1.0f;
	Vector4f first(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f second(1.0f, 1.0f, 1.0f, 1.0f);

	const bool scalarOp = retrieveVectorsOrScalar(L, first, second, scalar);

	if (scalarOp)
		first /= scalar;
	else
		first /= second;

	LuaVector4Utils<T>::push(L, first);

	return 1;
}

template <class T>
int LuaVector4<T>::length(lua_State *L)
{
	const Vector4f v = LuaVector4Utils<T>::retrieveTable(L, -1);

	LuaUtils::push(L, v.length());

	return 1;
}

template <class T>
int LuaVector4<T>::sqrLength(lua_State *L)
{
	const Vector4f v = LuaVector4Utils<T>::retrieveTable(L, -1);

	LuaUtils::push(L, v.sqrLength());

	return 1;
}

template <class T>
int LuaVector4<T>::normalized(lua_State *L)
{
	Vector4f v = LuaVector4Utils<T>::retrieveTable(L, -1);

	v.normalize();
	LuaVector4Utils<T>::push(L, v);

	return 1;
}

template <class T>
int LuaVector4<T>::dot(lua_State *L)
{
	const Vector4f second = LuaVector4Utils<T>::retrieveTable(L, -1);
	lua_pop(L, 1);
	const Vector4f first = LuaVector4Utils<T>::retrieveTable(L, -1);
	lua_pop(L, 1);

	const T product = ncine::dot(first, second);
	LuaUtils::push(L, product);

	return 1;
}

}

#endif
