#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaBaseSprite.h"
#include "LuaUntrackedUserData.h"
#include "LuaDrawableNode.h"
#include "LuaRectUtils.h"
#include "LuaVector2Utils.h"
#include "LuaUtils.h"
#include "BaseSprite.h"

namespace ncine {

namespace LuaNames {
namespace BaseSprite {
	static const char *setSize = "set_size";

	static const char *texture = "get_texture";
	static const char *setTexture = "set_texture";
	static const char *resetTexture = "reset_texture";

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

	LuaUtils::addFunction(L, LuaNames::BaseSprite::setSize, setSize);

	LuaUtils::addFunction(L, LuaNames::BaseSprite::texture, texture);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setTexture, setTexture);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::resetTexture, resetTexture);

	LuaUtils::addFunction(L, LuaNames::BaseSprite::texRect, texRect);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setTexRect, setTexRect);

	LuaUtils::addFunction(L, LuaNames::BaseSprite::anchorPoint, anchorPoint);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setAnchorPoint, setAnchorPoint);

	LuaUtils::addFunction(L, LuaNames::BaseSprite::isFlippedX, isFlippedX);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setFlippedX, setFlippedX);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::isFlippedY, isFlippedY);
	LuaUtils::addFunction(L, LuaNames::BaseSprite::setFlippedY, setFlippedY);
}

int LuaBaseSprite::setSize(lua_State *L)
{
	BaseSprite *sprite = LuaUntrackedUserData<BaseSprite>::retrieve(L, -3);
	const float width = LuaUtils::retrieve<float>(L, -2);
	const float height = LuaUtils::retrieve<float>(L, -1);

	if (sprite)
		sprite->setSize(width, height);

	return 0;
}

int LuaBaseSprite::texture(lua_State *L)
{
	BaseSprite *sprite = LuaUntrackedUserData<BaseSprite>::retrieve(L, -1);

	if (sprite)
		LuaUntrackedUserData<Texture>::push(L, sprite->texture());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaBaseSprite::setTexture(lua_State *L)
{
	BaseSprite *sprite = LuaUntrackedUserData<BaseSprite>::retrieve(L, -2);
	Texture *texture = LuaUntrackedUserData<Texture>::retrieveOrNil(L, -1);

	if (sprite)
		sprite->setTexture(texture);

	return 0;
}

int LuaBaseSprite::resetTexture(lua_State *L)
{
	BaseSprite *sprite = LuaUntrackedUserData<BaseSprite>::retrieve(L, -1);

	if (sprite)
		sprite->resetTexture();

	return 0;
}

int LuaBaseSprite::texRect(lua_State *L)
{
	BaseSprite *sprite = LuaUntrackedUserData<BaseSprite>::retrieve(L, -1);

	if (sprite)
		LuaRectiUtils::push(L, sprite->texRect());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaBaseSprite::setTexRect(lua_State *L)
{
	int rectIndex = 0;
	const Recti texRect = LuaRectiUtils::retrieve(L, -1, rectIndex);
	BaseSprite *sprite = LuaUntrackedUserData<BaseSprite>::retrieve(L, rectIndex - 1);

	if (sprite)
		sprite->setTexRect(texRect);

	return 0;
}

int LuaBaseSprite::anchorPoint(lua_State *L)
{
	BaseSprite *sprite = LuaUntrackedUserData<BaseSprite>::retrieve(L, -1);

	if (sprite)
		LuaVector2fUtils::push(L, sprite->anchorPoint());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaBaseSprite::setAnchorPoint(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2f anchorPoint = LuaVector2fUtils::retrieve(L, -1, vectorIndex);
	BaseSprite *sprite = LuaUntrackedUserData<BaseSprite>::retrieve(L, vectorIndex - 1);

	if (sprite)
		sprite->setAnchorPoint(anchorPoint);

	return 0;
}

int LuaBaseSprite::isFlippedX(lua_State *L)
{
	BaseSprite *sprite = LuaUntrackedUserData<BaseSprite>::retrieve(L, -1);

	if (sprite)
		LuaUtils::push(L, sprite->isFlippedX());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaBaseSprite::setFlippedX(lua_State *L)
{
	BaseSprite *sprite = LuaUntrackedUserData<BaseSprite>::retrieve(L, -2);
	const bool flippedX = LuaUtils::retrieve<bool>(L, -1);

	if (sprite)
		sprite->setFlippedX(flippedX);

	return 0;
}

int LuaBaseSprite::isFlippedY(lua_State *L)
{
	BaseSprite *sprite = LuaUntrackedUserData<BaseSprite>::retrieve(L, -1);

	if (sprite)
		LuaUtils::push(L, sprite->isFlippedY());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaBaseSprite::setFlippedY(lua_State *L)
{
	BaseSprite *sprite = LuaUntrackedUserData<BaseSprite>::retrieve(L, -2);
	const bool flippedY = LuaUtils::retrieve<bool>(L, -1);

	if (sprite)
		sprite->setFlippedY(flippedY);

	return 0;
}

}
