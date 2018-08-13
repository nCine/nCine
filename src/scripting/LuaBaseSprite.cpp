#include "LuaBaseSprite.h"
#include "LuaClassWrapper.h"
#include "LuaDrawableNode.h"
#include "LuaRect.h"
#include "LuaUtils.h"
#include "BaseSprite.h"

namespace ncine {

namespace LuaNames {
namespace BaseSprite
{
	static const char *setWidth = "set_width";
	static const char *setHeight = "set_height";
	static const char *setSize = "set_size";

	static const char *texture = "get_texture";
	static const char *setTexture = "set_texture";
	static const char *setOpaqueTexture = "set_opaque_texture";

	static const char *texRect = "get_texrect";
	static const char *setTexRect = "set_texrect";

	static const char *flipX = "flip_x";
	static const char *flipY = "flip_y";
}}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void LuaBaseSprite::exposeFunctions(lua_State *L)
{
	LuaDrawableNode::exposeFunctions(L);

	LuaUtils::addFunction(L, LuaNames::BaseSprite::setWidth, setWidth);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setHeight, setHeight);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setSize, setSize);

	LuaUtils::addFunction(L, LuaNames::BaseSprite::texture, texture);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setTexture, setTexture);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setOpaqueTexture, setOpaqueTexture);

	LuaUtils::addFunction(L, LuaNames::BaseSprite::texRect, texRect);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setTexRect, setTexRect);

	LuaUtils::addFunction(L, LuaNames::BaseSprite::flipX, flipX);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::flipY, flipY);
}

int LuaBaseSprite::setWidth(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -2);
	const float width = LuaUtils::retrieve<float>(L, -1);

	sprite->setWidth(width);

	return 0;
}

int LuaBaseSprite::setHeight(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -2);
	const float height = LuaUtils::retrieve<float>(L, -1);

	sprite->setHeight(height);

	return 0;
}

int LuaBaseSprite::setSize(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -3);
	const float width = LuaUtils::retrieve<float>(L, -2);
	const float height = LuaUtils::retrieve<float>(L, -1);

	sprite->setSize(width, height);

	return 0;
}

int LuaBaseSprite::texture(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -1);

	const Texture *texture = sprite->texture();
	LuaUtils::push(L, texture);

	return 1;
}

int LuaBaseSprite::setTexture(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -2);
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -1);

	sprite->setTexture(texture);

	return 0;
}

int LuaBaseSprite::setOpaqueTexture(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -2);
	const bool opaqueTexture = LuaUtils::retrieve<bool>(L, -1);

	sprite->setOpaqueTexture(opaqueTexture);

	return 0;
}

int LuaBaseSprite::texRect(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -1);

	const Recti texRect = sprite->texRect();
	LuaRectiUtils::push(L, texRect);

	return 1;
}

int LuaBaseSprite::setTexRect(lua_State *L)
{
	int rectIndex = 0;
	const Recti texRect = LuaRectiUtils::retrieve(L, -1, &rectIndex);
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, rectIndex - 1);

	sprite->setTexRect(texRect);

	return 0;
}

int LuaBaseSprite::flipX(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -1);

	sprite->flipX();

	return 0;
}

int LuaBaseSprite::flipY(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -1);

	sprite->flipY();

	return 0;
}

}
