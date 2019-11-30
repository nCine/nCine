#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaBaseSprite.h"
#include "LuaClassWrapper.h"
#include "LuaDrawableNode.h"
#include "LuaRectUtils.h"
#include "LuaVector2Utils.h"
#include "LuaUtils.h"
#include "BaseSprite.h"

namespace ncine {

namespace LuaNames {
namespace BaseSprite {
	static const char *setWidth = "set_width";
	static const char *setHeight = "set_height";
	static const char *setSize = "set_size";

	static const char *texture = "get_texture";
	static const char *setTexture = "set_texture";

	static const char *texRect = "get_texrect";
	static const char *setTexRect = "set_texrect";

	static const char *anchorPoint = "get_anchor_point";
	static const char *setAnchorPoint = "set_anchor_point";

	static const char *isFlippedX = "is_flipped_x";
	static const char *setFlippedX = "set_flipped_x";
	static const char *isFlippedY = "is_flipped_y";
	static const char *setFlippedY = "set_flipped_y";
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

	LuaUtils::addFunction(L, LuaNames::BaseSprite::texRect, texRect);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setTexRect, setTexRect);

	LuaUtils::addFunction(L, LuaNames::BaseSprite::anchorPoint, anchorPoint);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setAnchorPoint, setAnchorPoint);

	LuaUtils::addFunction(L, LuaNames::BaseSprite::isFlippedX, isFlippedX);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setFlippedX, setFlippedX);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::isFlippedY, isFlippedY);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setFlippedY, setFlippedY);
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

	LuaClassWrapper<Texture>::pushUntrackedUserData(L, sprite->texture());

	return 1;
}

int LuaBaseSprite::setTexture(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -2);
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -1);

	sprite->setTexture(texture);

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
	const Recti texRect = LuaRectiUtils::retrieve(L, -1, rectIndex);
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, rectIndex - 1);

	sprite->setTexRect(texRect);

	return 0;
}

int LuaBaseSprite::anchorPoint(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -1);

	const Vector2f anchorPoint = sprite->anchorPoint();
	LuaVector2fUtils::push(L, anchorPoint);

	return 1;
}

int LuaBaseSprite::setAnchorPoint(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2f anchorPoint = LuaVector2fUtils::retrieve(L, -1, vectorIndex);
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, vectorIndex - 1);

	sprite->setAnchorPoint(anchorPoint);

	return 0;
}

int LuaBaseSprite::isFlippedX(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -1);

	const bool result = sprite->isFlippedX();
	LuaUtils::push(L, result);

	return 1;
}

int LuaBaseSprite::setFlippedX(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -1);
	const bool flippedX = LuaUtils::retrieve<bool>(L, -1);

	sprite->setFlippedX(flippedX);

	return 0;
}

int LuaBaseSprite::isFlippedY(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -1);

	const bool result = sprite->isFlippedY();
	LuaUtils::push(L, result);

	return 1;
}

int LuaBaseSprite::setFlippedY(lua_State *L)
{
	BaseSprite *sprite = LuaClassWrapper<BaseSprite>::unwrapUserData(L, -1);
	const bool flippedY = LuaUtils::retrieve<bool>(L, -1);

	sprite->setFlippedY(flippedY);

	return 0;
}

}
