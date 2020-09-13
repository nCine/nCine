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
	static const char *add = "add";
	static const char *subtract = "sub";
	static const char *multiply = "mul";

	static const char *BLACK = "BLACK";
	static const char *WHITE = "WHITE";
	static const char *RED = "RED";
	static const char *GREEN = "GREEN";
	static const char *BLUE = "BLUE";
	static const char *YELLOW = "YELLOW";
	static const char *MAGENTA = "MAGENTA";
	static const char *CYAN = "CYAN";

	static const char *Colors = "colors";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaColor::expose(lua_State *L)
{
	lua_newtable(L);

	LuaUtils::addFunction(L, LuaNames::Color::create, create);
	LuaUtils::addFunction(L, LuaNames::Color::add, add);
	LuaUtils::addFunction(L, LuaNames::Color::subtract, subtract);
	LuaUtils::addFunction(L, LuaNames::Color::multiply, multiply);

	lua_setfield(L, -2, LuaNames::Color::Color);
}

void LuaColor::exposeConstants(lua_State *L)
{
	lua_createtable(L, 8, 0);

	LuaColorUtils::pushField(L, LuaNames::Color::BLACK, Colorf::Black);
	LuaColorUtils::pushField(L, LuaNames::Color::WHITE, Colorf::White);
	LuaColorUtils::pushField(L, LuaNames::Color::RED, Colorf::Red);
	LuaColorUtils::pushField(L, LuaNames::Color::GREEN, Colorf::Green);
	LuaColorUtils::pushField(L, LuaNames::Color::BLUE, Colorf::Blue);
	LuaColorUtils::pushField(L, LuaNames::Color::YELLOW, Colorf::Yellow);
	LuaColorUtils::pushField(L, LuaNames::Color::MAGENTA, Colorf::Magenta);
	LuaColorUtils::pushField(L, LuaNames::Color::CYAN, Colorf::Cyan);

	lua_setfield(L, -2, LuaNames::Color::Colors);
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

int LuaColor::add(lua_State *L)
{
	Colorf first(1.0f, 1.0f, 1.0f, 1.0f);
	Colorf second(1.0f, 1.0f, 1.0f, 1.0f);

	if (lua_istable(L, -2) && lua_istable(L, -1))
	{
		second = LuaColorUtils::retrieveTable(L, -1);
		lua_pop(L, 1);
		first = LuaColorUtils::retrieveTable(L, -1);
		lua_pop(L, 1);
	}
	else
		LuaDebug::traceError(L, "Expecting two color tables");

	first += second;

	LuaColorUtils::push(L, first);

	return 1;
}

int LuaColor::subtract(lua_State *L)
{
	Colorf first(1.0f, 1.0f, 1.0f, 1.0f);
	Colorf second(1.0f, 1.0f, 1.0f, 1.0f);

	if (lua_istable(L, -2) && lua_istable(L, -1))
	{
		second = LuaColorUtils::retrieveTable(L, -1);
		lua_pop(L, 1);
		first = LuaColorUtils::retrieveTable(L, -1);
		lua_pop(L, 1);
	}
	else
		LuaDebug::traceError(L, "Expecting two color tables");

	first -= second;

	LuaColorUtils::push(L, first);

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
