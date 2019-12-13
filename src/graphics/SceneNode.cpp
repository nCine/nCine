#include "SceneNode.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const float SceneNode::MinRotation = 0.5f;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! \param parent The parent can be `nullptr` */
SceneNode::SceneNode(SceneNode *parent, float xx, float yy)
    : Object(ObjectType::SCENENODE), x(xx), y(yy), updateEnabled_(true), drawEnabled_(true), parent_(nullptr),
      anchorPoint_(0.0f, 0.0f), scaleFactor_(1.0f, 1.0f), rotation_(0.0f),
      absX_(0.0f), absY_(0.0f), absScaleFactor_(1.0f, 1.0f), absRotation_(0.0f),
      worldMatrix_(Matrix4x4f::Identity), localMatrix_(Matrix4x4f::Identity),
      shouldDeleteChildrenOnDestruction_(true)
{
	setParent(parent);
}

/*! \param parent The parent can be `nullptr` */
SceneNode::SceneNode(SceneNode *parent, const Vector2f &position)
    : SceneNode(parent, position.x, position.y)
{
}

/*! \param parent The parent can be `nullptr` */
SceneNode::SceneNode(SceneNode *parent)
    : SceneNode(parent, 0.0f, 0.0f)
{
}

SceneNode::SceneNode()
    : SceneNode(nullptr, 0.0f, 0.0f)
{
}

SceneNode::~SceneNode()
{
	if (shouldDeleteChildrenOnDestruction_)
	{
		for (SceneNode *child : children_)
			delete child;
	}
	else
	{
		for (SceneNode *child : children_)
			child->parent_ = nullptr;
	}

	setParent(nullptr);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void SceneNode::setParent(SceneNode *parentNode)
{
	ASSERT(parentNode != this);

	if (parent_)
		parent_->children_.remove(this);
	if (parentNode)
		parentNode->children_.pushBack(this);
	parent_ = parentNode;
}

void SceneNode::addChildNode(SceneNode *childNode)
{
	ASSERT(childNode);
	ASSERT(childNode != this);

	if (childNode->parent_)
		childNode->parent_->removeChildNode(childNode);

	children_.pushBack(childNode);
	childNode->parent_ = this;
}

/*!	\return True if the node has been removed */
bool SceneNode::removeChildNode(SceneNode *childNode)
{
	ASSERT(childNode);
	ASSERT(childNode != this);
	bool hasBeenRemoved = false;

	if (!children_.isEmpty() && // avoid checking if this node has no children
	    childNode->parent_ == this) // avoid checking if the child doesn't belong to this node
	{
		childNode->parent_ = nullptr;
		children_.remove(childNode);
		hasBeenRemoved = true;
	}

	return hasBeenRemoved;
}

/*!
 * It is faster to remove through an iterator than with a linear search for a specific node.
 * \return True if the node has been removed
 */
bool SceneNode::removeChildNode(nctl::List<SceneNode *>::ConstIterator it)
{
	ASSERT(*it != this);
	bool hasBeenRemoved = false;

	if (*it && // cannot pass a `nullptr`
	    !children_.isEmpty() && // avoid checking if this node has no children
	    (*it)->parent_ == this) // avoid checking the child doesn't belong to this one
	{
		(*it)->parent_ = nullptr;
		children_.erase(it);
		hasBeenRemoved = true;
	}

	return hasBeenRemoved;
}

/*!	\return True if the node has been unlinked */
bool SceneNode::unlinkChildNode(SceneNode *childNode)
{
	ASSERT(childNode);
	ASSERT(childNode != this);
	bool hasBeenUnlinked = false;

	if (!children_.isEmpty() && // avoid checking if this node has no children
	    childNode->parent_ == this) // avoid checking if the child doesn't belong to this node
	{
		childNode->parent_ = nullptr;
		children_.remove(childNode);

		// Nephews reparenting
		for (SceneNode *child : childNode->children_)
			addChildNode(child);

		hasBeenUnlinked = true;
	}

	return hasBeenUnlinked;
}

void SceneNode::update(float interval)
{
	// Early return not needed, the first call to this method is on the root node

	for (SceneNode *child : children_)
	{
		if (child->updateEnabled_)
		{
			child->transform();
			child->update(interval);
		}
	}
}

void SceneNode::visit(RenderQueue &renderQueue)
{
	// Early return not needed, the first call to this method is on the root node

	for (SceneNode *child : children_)
	{
		if (child->drawEnabled_)
		{
			child->draw(renderQueue);
			child->visit(renderQueue);
		}
	}
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

void SceneNode::transform()
{
	// Calculating world and local matrices
	localMatrix_ = Matrix4x4f::translation(x, y, 0.0f);
	localMatrix_.rotateZ(rotation_);
	localMatrix_.scale(scaleFactor_.x, scaleFactor_.y, 1.0f);
	localMatrix_.translate(-anchorPoint_.x, -anchorPoint_.y, 0.0f);

	absScaleFactor_ = scaleFactor_;
	absRotation_ = rotation_;
	absColor_ = color_;

	if (parent_)
	{
		worldMatrix_ = parent_->worldMatrix_ * localMatrix_;

		absScaleFactor_ *= parent_->absScaleFactor_;
		absRotation_ += parent_->absRotation_;
		absColor_ *= parent_->absColor_;
	}
	else
		worldMatrix_ = localMatrix_;

	absX_ = worldMatrix_[3][0];
	absY_ = worldMatrix_[3][1];
}

}
