#include "SceneNode.h"
#ifdef WITH_MULTITHREADING
	#include "ThreadPool.h"
	#include "ThreadCommands.h"
#endif

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
	  scaleFactor_(1.0f), rotation_(0.0f), absX_(0.0f), absY_(0.0f), absScaleFactor_(1.0f), absRotation_(0.0f),
	  worldMatrix_(Matrix4x4f::Identity), localMatrix_(Matrix4x4f::Identity)
{
	if (parent)
		parent->addChildNode(this);
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
	for (SceneNode *child : children_)
		delete(child);

	if (parent_)
	{
		parent_->removeChildNode(this);
		parent_ = nullptr;
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void SceneNode::addChildNode(SceneNode *childNode)
{
	ASSERT(childNode);

	if (childNode->parent_)
		childNode->parent_->removeChildNode(childNode);

	childNode->parent_ = this;
	children_.pushBack(childNode);
}

/*!	\return True if the node has been removed */
bool SceneNode::removeChildNode(SceneNode *childNode)
{
	ASSERT(childNode);
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
#ifndef WITH_MULTITHREADING
			child->update(interval);
#else
			theServiceLocator().threadPool().enqueueCommand(nctl::makeUnique<UpdateNodeCommand>(*i, interval));
#endif
			child->transform();
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
	localMatrix_ = Matrix4x4f::Identity;
	localMatrix_ *= Matrix4x4f::translation(x, y, 0.0f);
	localMatrix_ *= Matrix4x4f::rotationZ(rotation_);
	localMatrix_ *= Matrix4x4f::scale(scaleFactor_, scaleFactor_, 1.0f);

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
