#include "LuaSceneNode.h"
#include "LuaClassWrapper.h"
#include "LuaClassTracker.h"
#include "LuaColorUtils.h"
#include "LuaVector2Utils.h"
#include "SceneNode.h"

namespace ncine {

namespace LuaNames {
namespace SceneNode {
	static const char *SceneNode = "scenenode";

	static const char *parent = "get_parent";
	static const char *setParent = "set_parent";
	static const char *addChildNode = "add_child";
	static const char *removeChildNode = "remove_child";
	static const char *removeAllChildrenNodes= "remove_all_children";
	static const char *unlinkChildNode = "unlink_child";

	static const char *isEnabled = "is_enabled";
	static const char *setEnabled = "set_enabled";

	static const char *position = "get_position";
	static const char *setPosition = "set_position";
	static const char *absAnchorPoint = "get_abs_anchor_point";
	static const char *setAbsAnchorPoint = "set_abs_anchor_point";
	static const char *scale = "get_scale";
	static const char *setScaleX = "set_scale_x";
	static const char *setScaleY = "set_scale_y";
	static const char *setScale = "set_scale";
	static const char *rotation = "get_rotation";
	static const char *setRotation = "set_rotation";

	static const char *color = "get_color";
	static const char *setColor = "set_color";
	static const char *alpha = "get_alpha";
	static const char *setAlpha = "set_alpha";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaSceneNode::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_createtable(L, 0, 25);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<SceneNode>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
		LuaUtils::addFunction(L, LuaNames::cloneNode, cloneNode);
	}

	exposeFunctions(L);

	lua_setfield(L, -2, LuaNames::SceneNode::SceneNode);
}

void LuaSceneNode::release(void *object)
{
	SceneNode *node = reinterpret_cast<SceneNode *>(object);
	delete node;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void LuaSceneNode::exposeFunctions(lua_State *L)
{
	LuaUtils::addFunction(L, LuaNames::SceneNode::parent, parent);
	LuaUtils::addFunction(L, LuaNames::SceneNode::setParent, setParent);
	LuaUtils::addFunction(L, LuaNames::SceneNode::addChildNode, addChildNode);
	LuaUtils::addFunction(L, LuaNames::SceneNode::removeChildNode, removeChildNode);
	LuaUtils::addFunction(L, LuaNames::SceneNode::removeAllChildrenNodes, removeAllChildrenNodes);
	LuaUtils::addFunction(L, LuaNames::SceneNode::unlinkChildNode, unlinkChildNode);

	LuaUtils::addFunction(L, LuaNames::SceneNode::isEnabled, isEnabled);
	LuaUtils::addFunction(L, LuaNames::SceneNode::setEnabled, setEnabled);

	LuaUtils::addFunction(L, LuaNames::SceneNode::position, position);
	LuaUtils::addFunction(L, LuaNames::SceneNode::setPosition, setPosition);
	LuaUtils::addFunction(L, LuaNames::SceneNode::absAnchorPoint, absAnchorPoint);
	LuaUtils::addFunction(L, LuaNames::SceneNode::setAbsAnchorPoint, setAbsAnchorPoint);
	LuaUtils::addFunction(L, LuaNames::SceneNode::scale, scale);
	LuaUtils::addFunction(L, LuaNames::SceneNode::setScaleX, setScaleX);
	LuaUtils::addFunction(L, LuaNames::SceneNode::setScaleY, setScaleY);
	LuaUtils::addFunction(L, LuaNames::SceneNode::setScale, setScale);
	LuaUtils::addFunction(L, LuaNames::SceneNode::rotation, rotation);
	LuaUtils::addFunction(L, LuaNames::SceneNode::setRotation, setRotation);

	LuaUtils::addFunction(L, LuaNames::SceneNode::color, color);
	LuaUtils::addFunction(L, LuaNames::SceneNode::setColor, setColor);
	LuaUtils::addFunction(L, LuaNames::SceneNode::alpha, alpha);
	LuaUtils::addFunction(L, LuaNames::SceneNode::setAlpha, setAlpha);
}

int LuaSceneNode::newObject(lua_State *L)
{
	SceneNode *parent = LuaClassWrapper<SceneNode>::unwrapUserDataOrNil(L, -3);
	const float x = LuaUtils::retrieve<float>(L, -2);
	const float y = LuaUtils::retrieve<float>(L, -1);

	LuaClassTracker<SceneNode>::newObject(L, parent, x, y);

	return 1;
}

int LuaSceneNode::cloneNode(lua_State *L)
{
	const SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	LuaClassTracker<SceneNode>::cloneNode(L, *node);

	return 1;
}

int LuaSceneNode::parent(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	const SceneNode *parent = node->parent();
	LuaUtils::push(L, parent);

	return 1;
}

int LuaSceneNode::setParent(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -2);
	SceneNode *parent = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	node->setParent(parent);

	return 0;
}

int LuaSceneNode::addChildNode(lua_State *L)
{
	SceneNode *parent = LuaClassWrapper<SceneNode>::unwrapUserData(L, -2);
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	parent->addChildNode(node);

	return 0;
}

int LuaSceneNode::removeChildNode(lua_State *L)
{
	SceneNode *parent = LuaClassWrapper<SceneNode>::unwrapUserData(L, -2);
	SceneNode *child = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	const bool result = parent->removeChildNode(child);
	LuaUtils::push(L, result);

	return 1;
}

int LuaSceneNode::removeAllChildrenNodes(lua_State *L)
{
	SceneNode *parent = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	const bool result = parent->removeAllChildrenNodes();
	LuaUtils::push(L, result);

	return 1;
}

int LuaSceneNode::unlinkChildNode(lua_State *L)
{
	SceneNode *parent = LuaClassWrapper<SceneNode>::unwrapUserData(L, -2);
	SceneNode *child = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	const bool result = parent->unlinkChildNode(child);
	LuaUtils::push(L, result);

	return 1;
}

int LuaSceneNode::isEnabled(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	const bool result = node->isEnabled();
	LuaUtils::push(L, result);

	return 1;
}

int LuaSceneNode::setEnabled(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -2);
	const bool enabled = LuaUtils::retrieve<bool>(L, -1);

	node->setEnabled(enabled);

	return 0;
}

int LuaSceneNode::position(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	const Vector2f &pos = node->position();
	LuaVector2fUtils::push(L, pos);

	return 1;
}

int LuaSceneNode::setPosition(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2f &pos = LuaVector2fUtils::retrieve(L, -1, vectorIndex);
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, vectorIndex - 1);

	node->setPosition(pos);

	return 0;
}

int LuaSceneNode::absAnchorPoint(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	const Vector2f &absAnchorPoint = node->absAnchorPoint();
	LuaVector2fUtils::push(L, absAnchorPoint);

	return 1;
}

int LuaSceneNode::setAbsAnchorPoint(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2f &absAnchorPoint = LuaVector2fUtils::retrieve(L, -1, vectorIndex);
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, vectorIndex - 1);

	node->setAbsAnchorPoint(absAnchorPoint);

	return 0;
}

int LuaSceneNode::scale(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	const Vector2f &scale = node->scale();
	LuaVector2fUtils::push(L, scale);

	return 1;
}

int LuaSceneNode::setScaleX(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -2);
	const float scaleX = LuaUtils::retrieve<float>(L, -1);

	node->setScale(scaleX, node->scale().y);

	return 0;
}

int LuaSceneNode::setScaleY(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -2);
	const float scaleY = LuaUtils::retrieve<float>(L, -1);

	node->setScale(node->scale().x, scaleY);

	return 0;
}

int LuaSceneNode::setScale(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -2);
	const float scale = LuaUtils::retrieve<float>(L, -1);

	node->setScale(scale);

	return 0;
}

int LuaSceneNode::rotation(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	const float rotation = node->rotation();
	LuaUtils::push(L, rotation);

	return 1;
}

int LuaSceneNode::setRotation(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, 1);
	const float rot = LuaUtils::retrieve<float>(L, 2);

	node->setRotation(rot);

	return 0;
}

int LuaSceneNode::color(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);
	const Colorf nodeColor(node->color());

	LuaColorUtils::push(L, nodeColor);

	return 1;
}

int LuaSceneNode::setColor(lua_State *L)
{
	int colorIndex = 0;
	const Colorf nodeColor = LuaColorUtils::retrieve(L, -1, colorIndex);
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, colorIndex - 1);

	node->setColor(nodeColor);

	return 0;
}

int LuaSceneNode::alpha(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	const float nodeAlpha = node->alpha();
	LuaUtils::push(L, nodeAlpha);

	return 1;
}

int LuaSceneNode::setAlpha(lua_State *L)
{
	SceneNode *node = LuaClassWrapper<SceneNode>::unwrapUserData(L, 1);
	const float nodeAlpha = LuaUtils::retrieve<float>(L, 2);

	node->setAlphaF(nodeAlpha);

	return 0;
}

}
