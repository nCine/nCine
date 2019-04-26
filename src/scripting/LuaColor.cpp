#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaColor.h"
#include "LuaColorUtils.h"
#include "LuaUtils.h"
#include "LuaDebug.h"
#include "Colorf.h"

namespace ncine {

namespace LuaNames {
namespace Color {
	static const char *Color = "color";

	static const char *create = "create";
	static const char *multiply = "mul";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaColor::expose(lua_State *L)
{
	lua_newtable(L);

	LuaUtils::addFunction(L, LuaNames::Color::create, create);
	LuaUtils::addFunction(L, LuaNames::Color::multiply, multiply);

	lua_setfield(L, -2, LuaNames::Color::Color);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaColor::create(lua_State *L)
{
	const float r = LuaUtils::retrieve<float>(L, -4);
	const float g = LuaUtils::retrieve<float>(L, -3);
	const float b = LuaUtils::retrieve<float>(L, -2);
	const float a = LuaUtils::retrieve<float>(L, -1);
	const Colorf color(r, g, b, a);

	LuaColorUtils::push(L, color);

	return 1;
}

int LuaColor::multiply(lua_State *L)
{
	float scalar = 1.0f;
	Colorf first(1.0f, 1.0f, 1.0f, 1.0f);
	Colorf second(1.0f, 1.0f, 1.0f, 1.0f);
	bool scalarOp = true;

	if (lua_isnumber(L, -2) && lua_isnumber(L, -1))
		LuaDebug::traceError(L, "Expecting two color tables or a color table and a number");
	else if (lua_isnumber(L, -2) && lua_istable(L, -1))
	{
		scalar = LuaUtils::retrieve<float>(L, -2);
		first = LuaColorUtils::retrieveTable(L, -1);
	}
	else if (lua_istable(L, -2) && lua_isnumber(L, -1))
	{
		scalar = LuaUtils::retrieve<float>(L, -1);
		lua_pop(L, 1);
		first = LuaColorUtils::retrieveTable(L, -1);
	}
	else if (lua_istable(L, -2) && lua_istable(L, -1))
	{
		second = LuaColorUtils::retrieveTable(L, -1);
		lua_pop(L, 1);
		first = LuaColorUtils::retrieveTable(L, -1);
		lua_pop(L, 1);
		scalarOp = false;
	}
	else
		LuaDebug::traceError(L, "Expecting two color tables or a color table and a number");

	if (scalarOp)
		first *= scalar;
	else
		first *= second;

	LuaColorUtils::push(L, first);

	return 1;
}

}
