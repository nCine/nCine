#include "LuaColorUtils.h"
#include "LuaUtils.h"
#include "Color.h"

namespace ncine {

namespace LuaNames {
namespace Color {
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
	LuaUtils::createTable(L, 0, 4);
	LuaUtils::pushField(L, LuaNames::Color::r, color.r());
	LuaUtils::pushField(L, LuaNames::Color::g, color.g());
	LuaUtils::pushField(L, LuaNames::Color::b, color.b());
	LuaUtils::pushField(L, LuaNames::Color::a, color.a());
}

void LuaColorUtils::pushField(lua_State *L, const char *name, const Color &color)
{
	push(L, color);
	LuaUtils::setField(L, -2, name);
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
	const unsigned char red = LuaUtils::retrieveField<uint32_t>(L, index, LuaNames::Color::r);
	const unsigned char green = LuaUtils::retrieveField<uint32_t>(L, index, LuaNames::Color::g);
	const unsigned char blue = LuaUtils::retrieveField<uint32_t>(L, index, LuaNames::Color::b);
	const unsigned char alpha = LuaUtils::retrieveField<uint32_t>(L, index, LuaNames::Color::a);
	return Color(red, green, blue, alpha);
}

Color LuaColorUtils::retrieveArray(lua_State *L, int index)
{
	LuaUtils::rawGeti(L, index, 1);
	const unsigned char red = LuaUtils::retrieve<uint32_t>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, index, 2);
	const unsigned char green = LuaUtils::retrieve<uint32_t>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, index, 3);
	const unsigned char blue = LuaUtils::retrieve<uint32_t>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, index, 4);
	const unsigned char alpha = LuaUtils::retrieve<uint32_t>(L, -1);
	LuaUtils::pop(L);

	return Color(red, green, blue, alpha);
}

Color LuaColorUtils::retrieveParams(lua_State *L, int index)
{
	const unsigned char red = LuaUtils::retrieve<uint32_t>(L, index);
	const unsigned char green = LuaUtils::retrieve<uint32_t>(L, index + 1);
	const unsigned char blue = LuaUtils::retrieve<uint32_t>(L, index + 2);
	const unsigned char alpha = LuaUtils::retrieve<uint32_t>(L, index + 3);
	return Color(red, green, blue, alpha);
}

Color LuaColorUtils::retrieveTableField(lua_State *L, int index, const char *name)
{
	LuaUtils::retrieveFieldTable(L, index, name);
	const unsigned char red = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::Color::r);
	const unsigned char green = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::Color::g);
	const unsigned char blue = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::Color::b);
	const unsigned char alpha = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::Color::a);
	LuaUtils::pop(L);

	return Color(red, green, blue, alpha);
}

Color LuaColorUtils::retrieveArrayField(lua_State *L, int index, const char *name)
{
	LuaUtils::retrieveFieldTable(L, index, name);

	LuaUtils::rawGeti(L, -1, 1);
	const unsigned char red = LuaUtils::retrieve<uint32_t>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, -1, 2);
	const unsigned char green = LuaUtils::retrieve<uint32_t>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, -1, 3);
	const unsigned char blue = LuaUtils::retrieve<uint32_t>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::rawGeti(L, -1, 4);
	const unsigned char alpha = LuaUtils::retrieve<uint32_t>(L, -1);
	LuaUtils::pop(L);

	LuaUtils::pop(L);

	return Color(red, green, blue, alpha);
}

}
