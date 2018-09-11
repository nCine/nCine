#include "LuaColorUtils.h"
#include "LuaUtils.h"
#include "Colorf.h"

namespace ncine {

namespace LuaNames {
namespace Color
{
	static const char *r = "r";
	static const char *g = "g";
	static const char *b = "b";
	static const char *a = "a";
}}


///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaColorUtils::push(lua_State *L, const Colorf &color)
{
	LuaUtils::createTable(L, 4, 0);
	LuaUtils::pushField(L, LuaNames::Color::r, color.r());
	LuaUtils::pushField(L, LuaNames::Color::g, color.g());
	LuaUtils::pushField(L, LuaNames::Color::b, color.b());
	LuaUtils::pushField(L, LuaNames::Color::a, color.a());
}

Colorf LuaColorUtils::retrieve(lua_State *L, int index, int &newIndex)
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

Colorf LuaColorUtils::retrieveTable(lua_State *L, int index)
{
	const float red = LuaUtils::retrieveField<float>(L, index, LuaNames::Color::r);
	const float green = LuaUtils::retrieveField<float>(L, index, LuaNames::Color::g);
	const float blue = LuaUtils::retrieveField<float>(L, index, LuaNames::Color::b);
	const float alpha = LuaUtils::retrieveField<float>(L, index, LuaNames::Color::a);
	return Colorf(red, green, blue, alpha);
}

Colorf LuaColorUtils::retrieveArray(lua_State *L, int index)
{
	LuaUtils::rawGeti(L, index, 1);
	const float red = LuaUtils::retrieve<float>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, index, 2);
	const float green = LuaUtils::retrieve<float>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, index, 3);
	const float blue = LuaUtils::retrieve<float>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, index, 4);
	const float alpha = LuaUtils::retrieve<float>(L, -1);
	LuaUtils::pop(L);

	return Colorf(red, green, blue, alpha);
}

Colorf LuaColorUtils::retrieveParams(lua_State *L, int index)
{
	const float red = LuaUtils::retrieve<float>(L, index);
	const float green = LuaUtils::retrieve<float>(L, index + 1);
	const float blue = LuaUtils::retrieve<float>(L, index + 2);
	const float alpha = LuaUtils::retrieve<float>(L, index + 3);
	return Colorf(red, green, blue, alpha);
}

Colorf LuaColorUtils::retrieveTableField(lua_State *L, int index, const char *name)
{
	LuaUtils::retrieveFieldTable(L, index, name);
	const float red = LuaUtils::retrieveField<float>(L, -1, LuaNames::Color::r);
	const float green = LuaUtils::retrieveField<float>(L, -1, LuaNames::Color::g);
	const float blue = LuaUtils::retrieveField<float>(L, -1, LuaNames::Color::b);
	const float alpha = LuaUtils::retrieveField<float>(L, -1, LuaNames::Color::a);
	LuaUtils::pop(L);

	return Colorf(red, green, blue, alpha);
}

Colorf LuaColorUtils::retrieveArrayField(lua_State *L, int index, const char *name)
{
	LuaUtils::retrieveFieldTable(L, index, name);

	LuaUtils::rawGeti(L, -1, 1);
	const float red = LuaUtils::retrieve<float>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, -1, 2);
	const float green = LuaUtils::retrieve<float>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, -1, 3);
	const float blue = LuaUtils::retrieve<float>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, -1, 4);
	const float alpha = LuaUtils::retrieve<float>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::pop(L);

	return Colorf(red, green, blue, alpha);
}

}
