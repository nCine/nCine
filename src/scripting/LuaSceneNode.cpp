#include "LuaSceneNode.h"
#include "LuaUntrackedUserData.h"
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
	static const char *numChildren = "num_children";
	static const char *child = "get_child";
	static const char *children = "get_children";
	static const char *addChildNode = "add_child";
	static const char *removeChildNode = "remove_child";
	static const char *removeChildNodeAt = "remove_child_at";
	static const char *removeAllChildrenNodes = "remove_all_children";
	static const char *unlinkChildNode = "unlink_child";

	static const char *childOrderIndex = "get_child_order_index";
	static const char *swapChildrenNodes = "swap_children";
	static const char *swapNodeForward = "swap_node_forward";
	static const char *swapNodeBack = "swap_node_back";

	static const char *visitOrderState = "get_visit_order_state";
	static const char *setVisitOrderState = "set_visit_order_state";
	static const char *visitOrderIndex = "get_visit_order_index";

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

	static const char *layer = "get_layer";
	static const char *setLayer = "set_layer";

	static const char *lastFrameUpdated = "get_last_frame_updated";

	static const char *ENABLED = "ENABLED";
	static const char *DISABLED = "DISABLED";
	static const char *SAME_AS_PARENT = "SAME_AS_PARENT";
	static const char *VisitOrderState = "visit_order_state";
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

void LuaSceneNode::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 3);

	LuaUtils::pushField(L, LuaNames::SceneNode::ENABLED, static_cast<int64_t>(SceneNode::VisitOrderState::ENABLED));
	LuaUtils::pushField(L, LuaNames::SceneNode::DISABLED, static_cast<int64_t>(SceneNode::VisitOrderState::DISABLED));
	LuaUtils::pushField(L, LuaNames::SceneNode::SAME_AS_PARENT, static_cast<int64_t>(SceneNode::VisitOrderState::SAME_AS_PARENT));

	lua_setfield(L, -2, LuaNames::SceneNode::VisitOrderState);
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
	LuaUtils::addFunction(L, LuaNames::SceneNode::numChildren, numChildren);
	LuaUtils::addFunction(L, LuaNames::SceneNode::child, child);
	LuaUtils::addFunction(L, LuaNames::SceneNode::children, children);
	LuaUtils::addFunction(L, LuaNames::SceneNode::addChildNode, addChildNode);
	LuaUtils::addFunction(L, LuaNames::SceneNode::removeChildNode, removeChildNode);
	LuaUtils::addFunction(L, LuaNames::SceneNode::removeChildNodeAt, removeChildNodeAt);
	LuaUtils::addFunction(L, LuaNames::SceneNode::removeAllChildrenNodes, removeAllChildrenNodes);
	LuaUtils::addFunction(L, LuaNames::SceneNode::unlinkChildNode, unlinkChildNode);

	LuaUtils::addFunction(L, LuaNames::SceneNode::childOrderIndex, childOrderIndex);
	LuaUtils::addFunction(L, LuaNames::SceneNode::swapChildrenNodes, swapChildrenNodes);
	LuaUtils::addFunction(L, LuaNames::SceneNode::swapNodeForward, swapNodeForward);
	LuaUtils::addFunction(L, LuaNames::SceneNode::swapNodeBack, swapNodeBack);

	LuaUtils::addFunction(L, LuaNames::SceneNode::visitOrderState, visitOrderState);
	LuaUtils::addFunction(L, LuaNames::SceneNode::setVisitOrderState, setVisitOrderState);
	LuaUtils::addFunction(L, LuaNames::SceneNode::visitOrderIndex, visitOrderIndex);

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

	LuaUtils::addFunction(L, LuaNames::SceneNode::layer, layer);
	LuaUtils::addFunction(L, LuaNames::SceneNode::setLayer, setLayer);

	LuaUtils::addFunction(L, LuaNames::SceneNode::lastFrameUpdated, lastFrameUpdated);
}

int LuaSceneNode::newObject(lua_State *L)
{
	SceneNode *parent = LuaUntrackedUserData<SceneNode>::retrieveOrNil(L, -3);
	const float x = LuaUtils::retrieve<float>(L, -2);
	const float y = LuaUtils::retrieve<float>(L, -1);

	LuaClassTracker<SceneNode>::newObject(L, parent, x, y);

	return 1;
}

int LuaSceneNode::cloneNode(lua_State *L)
{
	const SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
		LuaClassTracker<SceneNode>::cloneNode(L, *node);
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::parent(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
		LuaUtils::push(L, node->parent());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::setParent(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -2);
	SceneNode *parent = LuaUntrackedUserData<SceneNode>::retrieveOrNil(L, -1);

	if (node)
	{
		const bool result = node->setParent(parent);
		LuaUtils::push(L, result);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::numChildren(lua_State *L)
{
	const SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
		LuaUtils::push(L, node->children().size());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::child(lua_State *L)
{
	const SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -2);
	const unsigned int index = LuaUtils::retrieve<uint64_t>(L, -1);

	if (node && index < node->children().size())
		LuaUtils::push(L, node->children()[index]);
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::children(lua_State *L)
{
	const SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
	{
		const unsigned int numChildren = node->children().size();
		lua_createtable(L, numChildren, 0);

		for (unsigned int i = 0; i < numChildren; i++)
		{
			LuaUtils::push(L, node->children()[i]);
			lua_rawseti(L, -2, i + 1); // Lua arrays start from index 1
		}
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::addChildNode(lua_State *L)
{
	SceneNode *parent = LuaUntrackedUserData<SceneNode>::retrieve(L, -2);
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (parent)
	{
		const bool result = parent->addChildNode(node);
		LuaUtils::push(L, result);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::removeChildNode(lua_State *L)
{
	SceneNode *parent = LuaUntrackedUserData<SceneNode>::retrieve(L, -2);
	SceneNode *child = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (parent)
	{
		const bool result = parent->removeChildNode(child);
		LuaUtils::push(L, result);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::removeChildNodeAt(lua_State *L)
{
	SceneNode *parent = LuaUntrackedUserData<SceneNode>::retrieve(L, -2);
	unsigned int index = LuaUtils::retrieve<uint64_t>(L, -1);

	if (parent)
	{
		const bool result = parent->removeChildNodeAt(index);
		LuaUtils::push(L, result);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::removeAllChildrenNodes(lua_State *L)
{
	SceneNode *parent = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (parent)
	{
		const bool result = parent->removeAllChildrenNodes();
		LuaUtils::push(L, result);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::unlinkChildNode(lua_State *L)
{
	SceneNode *parent = LuaUntrackedUserData<SceneNode>::retrieve(L, -2);
	SceneNode *child = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (parent)
	{
		const bool result = parent->unlinkChildNode(child);
		LuaUtils::push(L, result);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::childOrderIndex(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
		LuaUtils::push(L, node->childOrderIndex());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::swapChildrenNodes(lua_State *L)
{
	SceneNode *parent = LuaUntrackedUserData<SceneNode>::retrieve(L, -3);
	unsigned int firstIndex = LuaUtils::retrieve<uint64_t>(L, -1);
	unsigned int secondIndex = LuaUtils::retrieve<uint64_t>(L, -1);

	if (parent)
	{
		const bool result = parent->swapChildrenNodes(firstIndex, secondIndex);
		LuaUtils::push(L, result);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::swapNodeForward(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
	{
		const bool result = node->swapNodeForward();
		LuaUtils::push(L, result);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::swapNodeBack(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
	{
		const bool result = node->swapNodeBack();
		LuaUtils::push(L, result);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::visitOrderState(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
		LuaUtils::push(L, static_cast<int64_t>(node->visitOrderState()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::setVisitOrderState(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -2);
	const SceneNode::VisitOrderState visitOrderState = static_cast<SceneNode::VisitOrderState>(LuaUtils::retrieve<int64_t>(L, -1));

	if (node)
		node->setVisitOrderState(visitOrderState);

	return 0;
}

int LuaSceneNode::visitOrderIndex(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
		LuaUtils::push(L, static_cast<uint32_t>(node->visitOrderIndex()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::isEnabled(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
		LuaUtils::push(L, node->isEnabled());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::setEnabled(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -2);
	const bool enabled = LuaUtils::retrieve<bool>(L, -1);

	if (node)
		node->setEnabled(enabled);

	return 0;
}

int LuaSceneNode::position(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
		LuaVector2fUtils::push(L, node->position());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::setPosition(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2f &pos = LuaVector2fUtils::retrieve(L, -1, vectorIndex);
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, vectorIndex - 1);

	if (node)
		node->setPosition(pos);

	return 0;
}

int LuaSceneNode::absAnchorPoint(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
		LuaVector2fUtils::push(L, node->absAnchorPoint());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::setAbsAnchorPoint(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2f &absAnchorPoint = LuaVector2fUtils::retrieve(L, -1, vectorIndex);
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, vectorIndex - 1);

	if (node)
		node->setAbsAnchorPoint(absAnchorPoint);

	return 0;
}

int LuaSceneNode::scale(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
		LuaVector2fUtils::push(L, node->scale());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::setScaleX(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -2);
	const float scaleX = LuaUtils::retrieve<float>(L, -1);

	if (node)
		node->setScale(scaleX, node->scale().y);

	return 0;
}

int LuaSceneNode::setScaleY(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -2);
	const float scaleY = LuaUtils::retrieve<float>(L, -1);

	if (node)
		node->setScale(node->scale().x, scaleY);

	return 0;
}

int LuaSceneNode::setScale(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -2);
	const float scale = LuaUtils::retrieve<float>(L, -1);

	if (node)
		node->setScale(scale);

	return 0;
}

int LuaSceneNode::rotation(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
		LuaUtils::push(L, node->rotation());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::setRotation(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, 1);
	const float rot = LuaUtils::retrieve<float>(L, 2);

	if (node)
		node->setRotation(rot);

	return 0;
}

int LuaSceneNode::color(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);
	const Colorf nodeColor(node->color());

	if (node)
		LuaColorUtils::push(L, nodeColor);
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::setColor(lua_State *L)
{
	int colorIndex = 0;
	const Colorf nodeColor = LuaColorUtils::retrieve(L, -1, colorIndex);
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, colorIndex - 1);

	if (node)
		node->setColorF(nodeColor);

	return 0;
}

int LuaSceneNode::alpha(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
	{
		const float alpha = node->alpha() / 255.0f;
		LuaUtils::push(L, alpha);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::setAlpha(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, 1);
	const float nodeAlpha = LuaUtils::retrieve<float>(L, 2);

	if (node)
		node->setAlphaF(nodeAlpha);

	return 0;
}

int LuaSceneNode::layer(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
		LuaUtils::push(L, static_cast<uint32_t>(node->layer()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaSceneNode::setLayer(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -2);
	const uint32_t layer = LuaUtils::retrieve<uint32_t>(L, -1);

	if (node)
		node->setLayer(static_cast<uint16_t>(layer));

	return 0;
}

int LuaSceneNode::lastFrameUpdated(lua_State *L)
{
	SceneNode *node = LuaUntrackedUserData<SceneNode>::retrieve(L, -1);

	if (node)
		LuaUtils::push(L, static_cast<uint64_t>(node->lastFrameUpdated()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

}
