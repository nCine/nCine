#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaDrawableNode.h"
#include "LuaSceneNode.h"
#include "LuaClassWrapper.h"
#include "LuaUtils.h"
#include "LuaVector2Utils.h"
#include "DrawableNode.h"

namespace ncine {

namespace LuaNames {
namespace DrawableNode
{
	static const char *LOWEST = "LOWEST";
	static const char *SCENE = "SCENE";
	static const char *HUD = "HUD";
	static const char *HIGHEST = "HIGHEST";
	static const char *LayerBase = "layer_base";

	static const char *width = "get_width";
	static const char *height = "get_height";
	static const char *size = "get_size";
	static const char *layer = "get_layer";
	static const char *setLayer = "set_layer";
}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaDrawableNode::exposeConstants(lua_State *L)
{
	lua_createtable(L, 4, 0);

	LuaUtils::pushField(L, LuaNames::DrawableNode::LOWEST, static_cast<int64_t>(DrawableNode::LayerBase::LOWEST));
	LuaUtils::pushField(L, LuaNames::DrawableNode::SCENE, static_cast<int64_t>(DrawableNode::LayerBase::SCENE));
	LuaUtils::pushField(L, LuaNames::DrawableNode::HUD, static_cast<int64_t>(DrawableNode::LayerBase::HUD));
	LuaUtils::pushField(L, LuaNames::DrawableNode::HIGHEST, static_cast<int64_t>(DrawableNode::LayerBase::HIGHEST));

	lua_setfield(L, -2, LuaNames::DrawableNode::LayerBase);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void LuaDrawableNode::exposeFunctions(lua_State *L)
{
	LuaSceneNode::exposeFunctions(L);

	LuaUtils::addFunction(L, LuaNames::DrawableNode::width, width);
	LuaUtils::addFunction(L, LuaNames::DrawableNode::height, height);
	LuaUtils::addFunction(L, LuaNames::DrawableNode::size, size);
	LuaUtils::addFunction(L, LuaNames::DrawableNode::layer, layer);
	LuaUtils::addFunction(L, LuaNames::DrawableNode::setLayer, setLayer);
}

int LuaDrawableNode::width(lua_State *L)
{
	DrawableNode *node = LuaClassWrapper<DrawableNode>::unwrapUserData(L, -1);

	const float width = node->width();
	LuaUtils::push(L, width);

	return 1;
}

int LuaDrawableNode::height(lua_State *L)
{
	DrawableNode *node = LuaClassWrapper<DrawableNode>::unwrapUserData(L, -1);

	const float height = node->height();
	LuaUtils::push(L, height);

	return 1;
}

int LuaDrawableNode::size(lua_State *L)
{
	DrawableNode *node = LuaClassWrapper<DrawableNode>::unwrapUserData(L, -1);

	const Vector2f size = node->size();
	LuaVector2fUtils::push(L, size);

	return 1;
}

int LuaDrawableNode::layer(lua_State *L)
{
	DrawableNode *node = LuaClassWrapper<DrawableNode>::unwrapUserData(L, -1);

	const unsigned int layer = node->layer();
	LuaUtils::push(L, layer);

	return 1;
}

int LuaDrawableNode::setLayer(lua_State *L)
{
	DrawableNode *node = LuaClassWrapper<DrawableNode>::unwrapUserData(L, -2);
	const int32_t layer = LuaUtils::retrieve<int32_t>(L, -1);

	node->setLayer(layer);

	return 0;
}

}
