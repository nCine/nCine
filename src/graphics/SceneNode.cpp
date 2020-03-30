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
    : Object(ObjectType::SCENENODE), x(xx), y(yy),
      updateEnabled_(true), drawEnabled_(true), parent_(nullptr), children_(4),
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
	// Can't set yourself or your parent as parent
	if (parentNode == this || parentNode == parent_)
		return;

	if (parent_)
		parent_->removeChildNode(this);
	if (parentNode)
		parentNode->children_.pushBack(this);
	parent_ = parentNode;
}

void SceneNode::addChildNode(SceneNode *childNode)
{
	// Can't add yourself or one of your children as a child
	if (childNode == this || (childNode != nullptr && childNode->parent_ == this))
		return;

	if (childNode->parent_)
		childNode->parent_->removeChildNode(childNode);

	children_.pushBack(childNode);
	childNode->parent_ = this;
}

/*! \return True if the node has been removed */
bool SceneNode::removeChildNode(SceneNode *childNode)
{
	// Can't remove yourself or a `nullptr` from your children
	if (childNode == this || childNode == nullptr)
		return false;

	bool hasBeenRemoved = false;

	if (!children_.isEmpty() && // avoid checking if this node has no children
	    childNode->parent_ == this) // avoid checking if the child doesn't belong to this node
	{
		for (unsigned int i = 0; i < children_.size(); i++)
		{
			if (children_[i] == childNode)
			{
				hasBeenRemoved = removeChildNodeAt(i);
				break;
			}
		}
	}

	return hasBeenRemoved;
}

/*! \return True if the node has been removed */
bool SceneNode::removeChildNodeAt(unsigned int index)
{
	const unsigned int size = children_.size();
	// Can't remove at an index past the number of children
	if (children_.isEmpty() || index > size - 1)
		return false;

	children_[index]->parent_ = nullptr;
	// Fast removal without preserving the order
	children_[index] = nctl::move(children_[size - 1]);
	children_.setSize(size - 1);
	return true;
}

/*!	\return True if the node has been unlinked */
bool SceneNode::unlinkChildNode(SceneNode *childNode)
{
	// Can't unlink yourself or a `nullptr` from your children
	if (childNode == this || childNode == nullptr)
		return false;

	bool hasBeenUnlinked = false;

	if (!children_.isEmpty() && // avoid checking if this node has no children
	    childNode->parent_ == this) // avoid checking if the child doesn't belong to this node
	{
		removeChildNode(childNode);

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
