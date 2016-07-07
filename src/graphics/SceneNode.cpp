#include <cstdlib> // for exit()
#include "SceneNode.h"
#include "ServiceLocator.h"
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

/*! \param parent The parent can be `NULL` */
SceneNode::SceneNode(SceneNode* parent, float xx, float yy)
	: Object(SCENENODE_TYPE), x(xx), y(yy), shouldUpdate_(true), shouldDraw_(true), parent_(NULL),
	  scaleFactor_(1.0f), rotation_(0.0f), absX_(0.0f), absY_(0.0f), absScaleFactor_(1.0f), absRotation_(0.0f),
	  worldMatrix_(Matrix4x4f::Identity), localMatrix_(Matrix4x4f::Identity)
{
	if (parent)
	{
		parent->addChildNode(this);
	}
}

/*! \param parent The parent can be `NULL` */
SceneNode::SceneNode(SceneNode* parent)
	: Object(SCENENODE_TYPE), x(0.0f), y(0.0f), shouldUpdate_(true), shouldDraw_(true), parent_(NULL),
	  scaleFactor_(1.0f), rotation_(0.0f), absX_(0.0f), absY_(0.0f), absScaleFactor_(1.0f), absRotation_(0.0f),
	  worldMatrix_(Matrix4x4f::Identity), localMatrix_(Matrix4x4f::Identity)
{
	if (parent)
	{
		parent->addChildNode(this);
	}
}

SceneNode::SceneNode()
	: Object(SCENENODE_TYPE), x(0.0f), y(0.0f), shouldUpdate_(true), shouldDraw_(true), parent_(NULL),
	  scaleFactor_(1.0f), rotation_(0.0f), absX_(0.0f), absY_(0.0f), absScaleFactor_(1.0f), absRotation_(0.0f),
	  worldMatrix_(Matrix4x4f::Identity), localMatrix_(Matrix4x4f::Identity)
{

}

SceneNode::~SceneNode()
{
	List<SceneNode *>::ConstIterator i = children_.begin();
	while (i != children_.end())
	{
		delete(*i++);
	}

	if (parent_)
	{
		parent_->removeChildNode(this);
		parent_ = NULL;
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/*!	\return True if the node has been added */
bool SceneNode::addChildNode(SceneNode *childNode)
{
	bool hasBeenAdded = false;

	if (childNode)
	{
		if (childNode->parent_)
		{
			childNode->parent_->removeChildNode(childNode);
		}

		childNode->parent_ = this;
		children_.pushBack(childNode);
		hasBeenAdded = true;
	}

	return hasBeenAdded;
}

/*!	\return True if the node has been removed */
bool SceneNode::removeChildNode(SceneNode *childNode)
{
	bool hasBeenRemoved = false;

	if (childNode && // cannot pass a NULL pointer
		!children_.isEmpty() && // avoid checking if this node has no children
		childNode->parent_ == this) // avoid checking the child doesn't belong to this one
	{
		childNode->parent_ = NULL;
		children_.remove(childNode);
		hasBeenRemoved = true;
	}

	return hasBeenRemoved;
}

/*!
	It is faster to remove through an iterator than with a linear search for a specific node.
	\return True if the node has been removed
*/
bool SceneNode::removeChildNode(List<SceneNode *>::ConstIterator it)
{
	bool hasBeenRemoved = false;

	if (*it && // cannot pass a NULL pointer
		!children_.isEmpty() && // avoid checking if this node has no children
		(*it)->parent_ == this) // avoid checking the child doesn't belong to this one
	{
		(*it)->parent_ = NULL;
		children_.erase(it);
		hasBeenRemoved = true;
	}

	return hasBeenRemoved;
}

/*!	\return True if the node has been unlinked */
bool SceneNode::unlinkChildNode(SceneNode *childNode)
{
	bool hasBeenUnlinked = false;

	if (childNode && // cannot pass a NULL pointer
		!children_.isEmpty() && // avoid checking if this node has no children
		childNode->parent_ == this) // avoid checking the child doesn't belong to this one
	{
		childNode->parent_ = NULL;
		children_.remove(childNode);

		// Nephews reparenting
		List<SceneNode *>::ConstIterator i = childNode->children_.begin();
		while (i != childNode->children_.end())
		{
			addChildNode(*i);
			++i;
		}

		hasBeenUnlinked = true;
	}

	return hasBeenUnlinked;
}

void SceneNode::update(float interval)
{
	// Early return not needed, the first call to this method is on the root node

	for (List<SceneNode *>::ConstIterator i = children_.begin(); i != children_.end(); ++i)
	{
		if ((*i)->shouldUpdate_)
		{
#ifndef WITH_MULTITHREADING
			(*i)->update(interval);
#else
			theServiceLocator().threadPool().enqueueCommand(new UpdateNodeCommand(*i, interval));
#endif
			(*i)->transform();
		}
	}
}

void SceneNode::visit(RenderQueue &renderQueue)
{
	// Early return not needed, the first call to this method is on the root node

	for (List<SceneNode *>::ConstIterator i = children_.begin(); i != children_.end(); ++i)
	{
		if ((*i)->shouldDraw_)
		{
			(*i)->draw(renderQueue);
			(*i)->visit(renderQueue);
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
		absRotation_ += parent_->absRotation_ ;
		absColor_ *= parent_->absColor_;
	}
	else
	{
		worldMatrix_ = localMatrix_;
	}

	absX_ = worldMatrix_[3][0];
	absY_ = worldMatrix_[3][1];
}

}
