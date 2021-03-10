#ifndef CLASS_NCINE_SCENENODE
#define CLASS_NCINE_SCENENODE

#include "Object.h"
#include <nctl/Array.h>
#include "Vector2.h"
#include "Matrix4x4.h"
#include "Color.h"
#include "Colorf.h"

namespace ncine {

class RenderQueue;

/// The base class for the transformation nodes hierarchy
class DLL_PUBLIC SceneNode : public Object
{
  public:
	/// The minimum amount of rotation to trigger a sine and cosine calculation
	static const float MinRotation;

	/// Relative X coordinate as a public property
	float x;
	/// Relative Y coordinate as a public property
	float y;

	/// Constructor for a node with a parent and a specified relative position
	SceneNode(SceneNode *parent, float xx, float yy);
	/// Constructor for a node with a parent and a specified relative position as a vector
	SceneNode(SceneNode *parent, const Vector2f &position);
	/// Constructor for a node with a parent and positioned in the relative origin
	explicit SceneNode(SceneNode *parent);
	/// Constructor for a node with no parent and positioned in the origin
	SceneNode();
	/// The destructor will delete every child node
	~SceneNode() override;

	/// Move constructor
	SceneNode(SceneNode &&other);
	/// Move assignment operator
	SceneNode &operator=(SceneNode &&other);

	/// Returns a copy of this object
	inline SceneNode clone() const { return SceneNode(*this); }

	inline static ObjectType sType() { return ObjectType::SCENENODE; }

	/// Returns the parent as a constant node, if there is any
	inline const SceneNode *parent() const { return parent_; }
	/// Returns the parent node, if there is any
	inline SceneNode *parent() { return parent_; }
	/// Sets the parent node
	void setParent(SceneNode *parentNode);
	/// Returns the array of child nodes
	inline const nctl::Array<SceneNode *> &children() { return children_; }
	/// Returns an array of constant child nodes
	const nctl::Array<const SceneNode *> &children() const;
	/// Adds a node as a child of this one
	void addChildNode(SceneNode *childNode);
	/// Removes a child of this node, without reparenting nephews
	bool removeChildNode(SceneNode *childNode);
	/// Removes the child at the specified index, without reparenting nephews
	bool removeChildNodeAt(unsigned int index);
	/// Removes all children, without reparenting nephews
	bool removeAllChildrenNodes();
	/// Removes a child of this node reparenting nephews as children
	bool unlinkChildNode(SceneNode *childNode);

	/// Called once every frame to update the node
	virtual void update(float interval);
	/// Draws the node and visits its children
	virtual void visit(RenderQueue &renderQueue);
	/// Renders the node
	virtual void draw(RenderQueue &renderQueue) {}

	/// Returns true if the node is updating
	inline bool isUpdateEnabled() const { return updateEnabled_; }
	/// Enables or disables node updating
	inline void setUpdateEnabled(bool updateEnabled) { updateEnabled_ = updateEnabled; }
	/// Returns true if the node is drawing
	inline bool isDrawEnabled() const { return drawEnabled_; }
	/// Enables or disables node drawing
	inline void setDrawEnabled(bool drawEnabled) { drawEnabled_ = drawEnabled; }
	/// Returns true if the node is both updating and drawing
	inline bool isEnabled() const { return (updateEnabled_ == true && drawEnabled_ == true); }
	/// Enables or disables both node updating and drawing
	void setEnabled(bool isEnabled);

	/// Returns node position relative to its parent
	inline Vector2f position() const { return Vector2f(x, y); }
	/// Returns absolute X coordinate node position
	inline float absX() const { return absX_; }
	/// Returns absolute Y coordinate node position
	inline float absY() const { return absY_; }
	/// Returns absolute node position
	inline Vector2f absPosition() const { return Vector2f(absX_, absY_); }
	/// Sets the node position through two coordinates
	void setPosition(float xx, float yy);
	/// Sets the node position through a vector
	void setPosition(const Vector2f &pos);
	/// Moves a node based on two offsets
	void move(float xx, float yy);
	/// Adds a move vector to the node current position
	void move(const Vector2f &pos);

	/// Gets the transformation anchor point in pixels
	inline Vector2f absAnchorPoint() const { return anchorPoint_; }
	/// Sets the transformation anchor point in pixels
	inline void setAbsAnchorPoint(float xx, float yy) { anchorPoint_.set(xx, yy); }
	/// Sets the transformation anchor point in pixels with a `Vector2f`
	inline void setAbsAnchorPoint(const Vector2f &point) { anchorPoint_ = point; }

	/// Gets the node scale factors
	inline const Vector2f &scale() const { return scaleFactor_; }
	/// Gets the node absolute scale factors
	inline const Vector2f &absScale() const { return absScaleFactor_; }
	/// Scales the node size both horizontally and vertically
	inline void setScale(float scaleFactor) { scaleFactor_.set(scaleFactor, scaleFactor); }
	/// Scales the node size both horizontally and vertically
	inline void setScale(float scaleFactorX, float scaleFactorY) { scaleFactor_.set(scaleFactorX, scaleFactorY); }
	/// Scales the node size both horizontally and vertically with a `Vector2f`
	inline void setScale(const Vector2f &scaleFactor) { scaleFactor_ = scaleFactor; }

	/// Gets the node rotation in degrees
	inline float rotation() const { return rotation_; }
	/// Gets the node absolute rotation in degrees
	inline float absRotation() const { return absRotation_; }
	/// Sets the node rotation in degrees
	void setRotation(float rotation);

	/// Gets the node color
	inline Color color() const { return color_; }
	/// Gets the node absolute color
	inline Color absColor() const { return absColor_; }
	/// Sets the node color through a `Color` object
	inline void setColor(Color color) { color_ = color; }
	/// Sets the node color through a `Colorf` object
	inline void setColor(Colorf color) { color_ = color; }
	/// Sets the node color through unsigned char components
	inline void setColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) { color_.set(red, green, blue, alpha); }
	/// Sets the node color through float components
	inline void setColorF(float red, float green, float blue, float alpha) { color_ = Colorf(red, green, blue, alpha); }
	/// Gets the node alpha
	inline float alpha() const { return color_.a(); }
	/// Gets the node absolute alpha
	inline float absAlpha() const { return absColor_.a(); }
	/// Sets the node alpha through an unsigned char component
	inline void setAlpha(unsigned char alpha) { color_.setAlpha(alpha); }
	/// Sets the node alpha through a float component
	inline void setAlphaF(float alpha) { color_.setAlpha(static_cast<unsigned char>(alpha * 255)); }

	/// Gets the node world matrix
	inline const Matrix4x4f &worldMatrix() const { return worldMatrix_; }
	/// Gets the node local matrix
	inline const Matrix4x4f &localMatrix() const { return localMatrix_; }

	/// Gets the delete children on destruction flag
	/*! If the flag is true the children are deleted upon node destruction. */
	inline bool deleteChildrenOnDestruction() const { return shouldDeleteChildrenOnDestruction_; }
	/// Sets the delete children on destruction flag
	inline void setDeleteChildrenOnDestruction(bool shouldDeleteChildrenOnDestruction) { shouldDeleteChildrenOnDestruction_ = shouldDeleteChildrenOnDestruction; }

  protected:
	bool updateEnabled_;
	bool drawEnabled_;

	/// A pointer to the parent node
	SceneNode *parent_;
	/// The array of child nodes
	nctl::Array<SceneNode *> children_;

	/// The anchor point for transformations, in pixels
	/// \note The default point is the center
	Vector2f anchorPoint_;
	/// Horizontal and vertical scale factors for node size
	Vector2f scaleFactor_;
	/// Degrees for clock-wise node rotation in degrees
	float rotation_;

	/// Node color for transparency and translucency
	/*! Even if the base scene node is not always drawable, it carries
	 *  color information to easily pass that information to its children. */
	Color color_;

	/// Absolute X coordinate as calculated by the `transform()` function
	float absX_;
	/// Absolute Y coordinate as calculated by the `transform()` function
	float absY_;
	/// Absolute horizontal and vertical scale factors as calculated by the `transform()` function
	Vector2f absScaleFactor_;
	/// Absolute node rotation as calculated by the `transform()` function
	float absRotation_;

	/// Absolute node color as calculated by the `transform()` function
	Color absColor_;

	/// World transformation matrix (calculated from local and parent's world)
	Matrix4x4f worldMatrix_;
	/// Local transformation matrix
	Matrix4x4f localMatrix_;

	/// A flag indicating whether the destructor should also delete all children
	bool shouldDeleteChildrenOnDestruction_;

	/// Deleted assignment operator
	SceneNode &operator=(const SceneNode &) = delete;

	/// Protected copy constructor used to clone objects
	SceneNode(const SceneNode &other);

	/// Swaps the child pointer of a parent when moving an object
	void swapChildPointer(SceneNode *first, SceneNode *second);

	virtual void transform();
};

inline const nctl::Array<const SceneNode *> &SceneNode::children() const
{
	return reinterpret_cast<const nctl::Array<const SceneNode *> &>(children_);
}

inline void SceneNode::setEnabled(bool enabled)
{
	updateEnabled_ = enabled;
	drawEnabled_ = enabled;
}

inline void SceneNode::setPosition(float xx, float yy)
{
	x = xx;
	y = yy;
}

inline void SceneNode::setPosition(const Vector2f &pos)
{
	x = pos.x;
	y = pos.y;
}

inline void SceneNode::move(float xx, float yy)
{
	x += xx;
	y += yy;
}

inline void SceneNode::move(const Vector2f &pos)
{
	x += pos.x;
	y += pos.y;
}

inline void SceneNode::setRotation(float rotation)
{
	rotation_ = fmodf(rotation, 360.0f);
}

}

#endif
