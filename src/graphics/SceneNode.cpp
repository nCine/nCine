#include "SceneNode.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const float SceneNode::MinRotation = 0.5f;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! \param parent The parent can be `nullptr` */
SceneNode::SceneNode(SceneNode *parent, float x, float y)
    : Object(ObjectType::SCENENODE),
      updateEnabled_(true), drawEnabled_(true), parent_(nullptr), children_(4),
      position_(x, y), anchorPoint_(0.0f, 0.0f), scaleFactor_(1.0f, 1.0f), rotation_(0.0f),
      color_(Color::White), absPosition_(0.0f, 0.0f), absScaleFactor_(1.0f, 1.0f),
      absRotation_(0.0f), absColor_(Color::White),
      worldMatrix_(Matrix4x4f::Identity), localMatrix_(Matrix4x4f::Identity),
      shouldDeleteChildrenOnDestruction_(true), dirtyBits_(0xFF)
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

SceneNode::SceneNode(SceneNode &&other)
    : Object(nctl::move(other)),
      updateEnabled_(other.updateEnabled_), drawEnabled_(other.drawEnabled_),
      parent_(other.parent_), children_(nctl::move(other.children_)),
      position_(other.position_), anchorPoint_(other.anchorPoint_),
      scaleFactor_(other.scaleFactor_), rotation_(other.rotation_), color_(other.color_),
      shouldDeleteChildrenOnDestruction_(other.shouldDeleteChildrenOnDestruction_),
      dirtyBits_(other.dirtyBits_)
{
	swapChildPointer(this, &other);
	for (SceneNode *child : children_)
		child->parent_ = this;
}

SceneNode &SceneNode::operator=(SceneNode &&other)
{
	Object::operator=(nctl::move(other));

	updateEnabled_ = other.updateEnabled_;
	drawEnabled_ = other.drawEnabled_;
	parent_ = other.parent_;
	children_ = nctl::move(other.children_);
	position_ = other.position_;
	anchorPoint_ = other.anchorPoint_;
	scaleFactor_ = other.scaleFactor_;
	rotation_ = other.rotation_;
	color_ = other.color_;
	shouldDeleteChildrenOnDestruction_ = other.shouldDeleteChildrenOnDestruction_;
	dirtyBits_ = other.dirtyBits_;

	swapChildPointer(this, &other);
	for (SceneNode *child : children_)
		child->parent_ = this;

	return *this;
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

	dirtyBits_.set(DirtyBitPositions::TransformationBit);
	dirtyBits_.set(DirtyBitPositions::AabbBit);
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
	// Can't remove at an index past the number of children
	if (children_.isEmpty() || index > children_.size() - 1)
		return false;

	children_[index]->parent_ = nullptr;
	dirtyBits_.set(DirtyBitPositions::TransformationBit);
	dirtyBits_.set(DirtyBitPositions::AabbBit);
	// Fast removal without preserving the order
	children_.unorderedRemoveAt(index);
	return true;
}

/*! \return True if there were at least one node to remove */
bool SceneNode::removeAllChildrenNodes()
{
	if (children_.isEmpty())
		return false;

	for (unsigned int i = 0; i < children_.size(); i++)
	{
		children_[i]->parent_ = nullptr;
		dirtyBits_.set(DirtyBitPositions::TransformationBit);
		dirtyBits_.set(DirtyBitPositions::AabbBit);
	}
	children_.clear();

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

	if (updateEnabled_)
	{
		transform();
		for (SceneNode *child : children_)
			child->update(interval);

		// A non drawable scenenode does not have the `updateRenderCommand()` method to reset the flags
		if (type_ == ObjectType::SCENENODE)
		{
			dirtyBits_.reset(DirtyBitPositions::TransformationBit);
			dirtyBits_.reset(DirtyBitPositions::ColorBit);
		}
	}
}

void SceneNode::visit(RenderQueue &renderQueue)
{
	// Early return not needed, the first call to this method is on the root node

	if (drawEnabled_)
	{
		draw(renderQueue);
		for (SceneNode *child : children_)
			child->visit(renderQueue);
	}
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

SceneNode::SceneNode(const SceneNode &other)
    : Object(other), updateEnabled_(other.updateEnabled_),
      drawEnabled_(other.drawEnabled_), parent_(nullptr), children_(4),
      position_(other.position_), anchorPoint_(other.anchorPoint_),
      scaleFactor_(other.scaleFactor_), rotation_(other.rotation_),
      color_(other.color_), absPosition_(0.0f, 0.0f), absScaleFactor_(1.0f, 1.0f), absRotation_(0.0f),
      absColor_(Color::White), worldMatrix_(Matrix4x4f::Identity), localMatrix_(Matrix4x4f::Identity),
      shouldDeleteChildrenOnDestruction_(other.shouldDeleteChildrenOnDestruction_), dirtyBits_(0xFF)
{
	setParent(other.parent_);
}

/*! \note It is faster than calling `setParent()` on the first child and `removeChildNode()` on the second one */
void SceneNode::swapChildPointer(SceneNode *first, SceneNode *second)
{
	ASSERT(first->parent_ == second->parent_);

	SceneNode *parent = first->parent_;
	if (parent)
	{
		for (unsigned int i = 0; i < parent->children_.size(); i++)
		{
			if (parent->children_[i] == second)
			{
				parent->children_[i] = this;
				second->parent_ = nullptr;
				break;
			}
		}
	}
}

void SceneNode::transform()
{
	ZoneScoped;

	const bool parentHasDirtyColor = parent_ && parent_->dirtyBits_.test(DirtyBitPositions::ColorBit);
	if (parentHasDirtyColor)
		dirtyBits_.set(DirtyBitPositions::ColorBit);

	if (dirtyBits_.test(DirtyBitPositions::ColorBit))
		absColor_ = parent_ ? color_ * parent_->absColor_ : color_;

	const bool parentHasDirtyTransformation = parent_ && parent_->dirtyBits_.test(DirtyBitPositions::TransformationBit);
	if (parentHasDirtyTransformation)
	{
		dirtyBits_.set(DirtyBitPositions::TransformationBit);
		dirtyBits_.set(DirtyBitPositions::AabbBit);
	}

	if (dirtyBits_.test(DirtyBitPositions::TransformationBit) == false)
		return;

	// Calculating world and local matrices
	localMatrix_ = Matrix4x4f::translation(position_.x, position_.y, 0.0f);
	localMatrix_.rotateZ(rotation_);
	localMatrix_.scale(scaleFactor_.x, scaleFactor_.y, 1.0f);
	localMatrix_.translate(-anchorPoint_.x, -anchorPoint_.y, 0.0f);

	absScaleFactor_ = scaleFactor_;
	absRotation_ = rotation_;

	if (parent_)
	{
		worldMatrix_ = parent_->worldMatrix_ * localMatrix_;

		absScaleFactor_ *= parent_->absScaleFactor_;
		absRotation_ += parent_->absRotation_;
	}
	else
		worldMatrix_ = localMatrix_;

	absPosition_.x = worldMatrix_[3][0];
	absPosition_.y = worldMatrix_[3][1];
}

}
