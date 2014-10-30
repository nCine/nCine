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

SceneNode::SceneNode(SceneNode* parent, float x, float y)
	: x(x), y(y), shouldUpdate_(true), shouldDraw_(true), parent_(NULL),
	  scaleFactor_(1.0f), rotation_(0.0f), absX_(0.0f), absY_(0.0f), absScaleFactor_(1.0f), absRotation_(0.0f)
{
	type_ = SCENENODE_TYPE;

	if (parent)
	{
		parent->addChildNode(this);
	}
}

SceneNode::SceneNode(SceneNode* parent)
	: x(0.0f), y(0.0f), shouldUpdate_(true), shouldDraw_(true), parent_(NULL),
	  scaleFactor_(1.0f), rotation_(0.0f), absX_(0.0f), absY_(0.0f), absScaleFactor_(1.0f), absRotation_(0.0f)
{
	type_ = SCENENODE_TYPE;

	if (parent)
	{
		parent->addChildNode(this);
	}
}

SceneNode::SceneNode()
	: Object(), x(0.0f), y(0.0f), shouldUpdate_(true), shouldDraw_(true), parent_(NULL),
	  scaleFactor_(1.0f), rotation_(0.0f), absX_(0.0f), absY_(0.0f), absScaleFactor_(1.0f), absRotation_(0.0f)
{
	type_ = SCENENODE_TYPE;
}

SceneNode::~SceneNode()
{
	List<SceneNode *>::Const_Iterator i = children_.begin();
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

/// Adds a node as a child of this one
/*!
	\return True if the node has been added
*/
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
		children_.insertBack(childNode);
		hasBeenAdded = true;
	}

	return hasBeenAdded;
}

/// Removes a child of this node, without reparenting nephews
/*!
	\return True if the node has been removed
*/
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

/// Removes a child of this node while iterating on children, without reparenting nephews
/*!
	It is faster to remove through an iterator than with a linear search for a specific node
	\return True if the node has been removed
*/
bool SceneNode::removeChildNode(List<SceneNode *>::Iterator it)
{
	bool hasBeenRemoved = false;

	if (*it && // cannot pass a NULL pointer
		!children_.isEmpty() && // avoid checking if this node has no children
		(*it)->parent_ == this) // avoid checking the child doesn't belong to this one
	{
		(*it)->parent_ = NULL;
		children_.remove(it);
		hasBeenRemoved = true;
	}

	return hasBeenRemoved;
}

/// Removes a child of this node reparenting nephews as children
/*!
	\return True if the node has been unlinked
*/
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
		List<SceneNode *>::Const_Iterator i = childNode->children_.begin();
		while (i != childNode->children_.end())
		{
			addChildNode(*i);
			++i;
		}

		hasBeenUnlinked = true;
	}

	return hasBeenUnlinked;
}

/// Called once every frame to update the node
void SceneNode::update(float interval)
{
	for (List<SceneNode *>::Const_Iterator i = children_.begin(); i != children_.end(); ++i)
	{
		if ((*i)->shouldUpdate_)
		{
#ifndef WITH_MULTITHREADING
			(*i)->update(interval);
#else
			ServiceLocator::threadPool().enqueueCommand(new UpdateNodeCommand(*i, interval));
#endif
		}
	}
}

/// Draws the node and visits its children
void SceneNode::visit(RenderQueue &renderQueue)
{
	// early return if a node is invisible
	if (!shouldDraw_)
	{
		return;
	}

	transform();
	draw(renderQueue);

	for (List<SceneNode *>::Const_Iterator i = children_.begin(); i != children_.end(); ++i)
	{
		(*i)->visit(renderQueue);
	}
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

void SceneNode::transform()
{
	// Calculating absolute transformations
	if (parent_)
	{
		absScaleFactor_ = parent_->absScaleFactor_ * scaleFactor_;
		absRotation_ = parent_->absRotation_ + rotation_;
		// New scaled position accounting parent scale factor (allow zooming)
		float scaledX = parent_->absScaleFactor_ * x;
		float scaledY = parent_->absScaleFactor_ * y;

		float sine = 0.0f;
		float cosine = 1.0f;
		float parentRot = parent_->absRotation_;
		if (fabs(parentRot) > MinRotation && fabs(parentRot) < 360.0f - MinRotation)
		{
			sine = sinf(-parentRot * M_PI / 180.0f);
			cosine = cosf(-parentRot * M_PI / 180.0f);
		}

		absX_ = parent_->absX_ + scaledX * cosine - scaledY * sine;
		absY_ = parent_->absY_ + scaledY * cosine + scaledX * sine;

		absColor_ = parent_->absColor_ * color_;
	}
	else
	{
		absX_ = x;
		absY_ = y;
		absScaleFactor_ = scaleFactor_;
		absRotation_ = rotation_;
		absColor_ = color_;
	}
}

}
