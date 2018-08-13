#include "LuaColorUtils.h"
#include "LuaUtils.h"
#include "Color.h"

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

void LuaColorUtils::push(lua_State *L, const Color &color)
{
	LuaUtils::createTable(L, 4, 0);
	LuaUtils::pushField(L, LuaNames::Color::r, color.fR());
	LuaUtils::pushField(L, LuaNames::Color::g, color.fG());
	LuaUtils::pushField(L, LuaNames::Color::b, color.fB());
	LuaUtils::pushField(L, LuaNames::Color::a, color.fA());
}

Color LuaColorUtils::retrieve(lua_State *L, int index, int &newIndex)
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

Color LuaColorUtils::retrieveTable(lua_State *L, int index)
{
	const float red = LuaUtils::retrieveField<float>(L, index, LuaNames::Color::r);
	const float green = LuaUtils::retrieveField<float>(L, index, LuaNames::Color::g);
	const float blue = LuaUtils::retrieveField<float>(L, index, LuaNames::Color::b);
	const float alpha = LuaUtils::retrieveField<float>(L, index, LuaNames::Color::a);
	return Color(red, green, blue, alpha);
}

Color LuaColorUtils::retrieveArray(lua_State *L, int index)
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

	return Color(red, green, blue, alpha);
}

Color LuaColorUtils::retrieveParams(lua_State *L, int index)
{
	const float red = LuaUtils::retrieve<float>(L, index);
	const float green = LuaUtils::retrieve<float>(L, index + 1);
	const float blue = LuaUtils::retrieve<float>(L, index + 2);
	const float alpha = LuaUtils::retrieve<float>(L, index + 3);
	return Color(red, green, blue, alpha);
}

Color LuaColorUtils::retrieveTableField(lua_State *L, int index, const char *name)
{
	LuaUtils::retrieveFieldTable(L, index, name);
	const float red = LuaUtils::retrieveField<float>(L, -1, LuaNames::Color::r);
	const float green = LuaUtils::retrieveField<float>(L, -1, LuaNames::Color::g);
	const float blue = LuaUtils::retrieveField<float>(L, -1, LuaNames::Color::b);
	const float alpha = LuaUtils::retrieveField<float>(L, -1, LuaNames::Color::a);
	LuaUtils::pop(L);

	return Color(red, green, blue, alpha);
}

Color LuaColorUtils::retrieveArrayField(lua_State *L, int index, const char *name)
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

	return Color(red, green, blue, alpha);
}

}
