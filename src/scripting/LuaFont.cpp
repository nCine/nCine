#include "LuaFont.h"
#include "LuaUntrackedUserData.h"
#include "LuaClassTracker.h"
#include "LuaVector2Utils.h"
#include "LuaUtils.h"
#include "Font.h"
#include "Texture.h"

namespace ncine {

namespace LuaNames {
namespace Font {
	static const char *Font = "font";

	static const char *texture = "get_texture";
	static const char *setTexture = "set_texture";

	static const char *lineHeight = "get_line_height";
	static const char *base = "get_base";
	static const char *textureSize = "get_texture_size";
	static const char *numGlyphs = "num_glyphs";
	static const char *numKernings = "num_kernings";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaFont::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_createtable(L, 0, 8);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<Font>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::Font::texture, texture);
	LuaUtils::addFunction(L, LuaNames::Font::setTexture, setTexture);

	LuaUtils::addFunction(L, LuaNames::Font::lineHeight, lineHeight);
	LuaUtils::addFunction(L, LuaNames::Font::base, base);
	LuaUtils::addFunction(L, LuaNames::Font::textureSize, textureSize);
	LuaUtils::addFunction(L, LuaNames::Font::numGlyphs, numGlyphs);
	LuaUtils::addFunction(L, LuaNames::Font::numKernings, numKernings);

	lua_setfield(L, -2, LuaNames::Font::Font);
}

void LuaFont::release(void *object)
{
	Font *font = reinterpret_cast<Font *>(object);
	delete font;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaFont::newObject(lua_State *L)
{
	const char *fntFilename = LuaUtils::retrieve<const char *>(L, -2);
	const char *texFilename = LuaUtils::retrieve<const char *>(L, -1);

	if (*texFilename == '\n')
		LuaClassTracker<Font>::newObject(L, fntFilename, texFilename);
	else
		LuaClassTracker<Font>::newObject(L, fntFilename);

	return 1;
}

int LuaFont::texture(lua_State *L)
{
	Font *font = LuaUntrackedUserData<Font>::retrieve(L, -1);

	if (font)
		LuaUntrackedUserData<Texture>::push(L, font->texture());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaFont::setTexture(lua_State *L)
{
	Font *font = LuaUntrackedUserData<Font>::retrieve(L, -2);
	Texture *texture = LuaUntrackedUserData<Texture>::retrieveOrNil(L, -1);

	if (font)
		font->setTexture(texture);

	return 0;
}

int LuaFont::lineHeight(lua_State *L)
{
	Font *font = LuaUntrackedUserData<Font>::retrieve(L, -1);

	if (font)
		LuaUtils::push(L, font->lineHeight());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaFont::base(lua_State *L)
{
	Font *font = LuaUntrackedUserData<Font>::retrieve(L, -1);

	if (font)
		LuaUtils::push(L, font->base());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaFont::textureSize(lua_State *L)
{
	Font *font = LuaUntrackedUserData<Font>::retrieve(L, -1);

	if (font)
		LuaVector2iUtils::push(L, font->textureSize());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaFont::numGlyphs(lua_State *L)
{
	Font *font = LuaUntrackedUserData<Font>::retrieve(L, -1);

	if (font)
		LuaUtils::push(L, font->numGlyphs());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaFont::numKernings(lua_State *L)
{
	Font *font = LuaUntrackedUserData<Font>::retrieve(L, -1);

	if (font)
		LuaUtils::push(L, font->numKernings());
	else
		LuaUtils::pushNil(L);

	return 1;
}

}
