#ifndef CLASS_NCINE_SCENENODE
#define CLASS_NCINE_SCENENODE

#include "Object.h"
#include <nctl/List.h>
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

	inline static ObjectType sType() { return ObjectType::SCENENODE; }

	/// Returns the parent as a constant node, if there is any
	inline const SceneNode *parent() const { return parent_; }
	/// Returns the parent node, if there is any
	inline SceneNode *parent() { return parent_; }
	/// Sets the parent node
	void setParent(SceneNode *parentNode);
	/// Returns a list of child nodes
	inline const nctl::List<SceneNode *> &children() const { return children_; }
	/// Adds a node as a child of this one
	void addChildNode(SceneNode *childNode);
	/// Removes a child of this node, without reparenting nephews
	bool removeChildNode(SceneNode *childNode);
	/// Removes a child of this node while iterating on children, without reparenting nephews
	bool removeChildNode(nctl::List<SceneNode *>::ConstIterator it);
	/// Removes a child of this node reparenting nephews as children
	bool unlinkChildNode(SceneNode *childNode);

	/// Called once every frame to update the node
	virtual void update(float interval);
	/// Draws the node and visits its children
	virtual void visit(RenderQueue &renderQueue);
	/// Renders the node
	virtual void draw(RenderQueue &renderQueue) {}

	/// Returns true if the node is updating
	inline bool updateEnabled() const { return updateEnabled_; }
	/// Enables or disables node updating
	inline void setUpdateEnabled(bool updateEnabled) { updateEnabled_ = updateEnabled; }
	/// Returns true if the node is drawing
	inline bool drawEnabled() const { return drawEnabled_; }
	/// Enables or disables node drawing
	inline void setDrawEnabled(bool drawEnabled) { drawEnabled_ = drawEnabled; }
	/// Returns true if the node is both updating and drawing
	inline bool enabled() const { return (updateEnabled_ == true && drawEnabled_ == true); }
	/// Enables or disables both node updating and drawing
	void setEnabled(bool enabled);

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

	/// Gets the node scale factor
	inline float scale() const { return scaleFactor_; }
	/// Gets the node absolute scale factor
	inline float absScale() const { return absScaleFactor_; }
	/// Scales the node size
	inline void setScale(float scaleFactor) { scaleFactor_ = scaleFactor; }

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
	/// Sets the node alpha through an unsigned char component
	inline void setAlpha(unsigned char alpha) { color_.setAlpha(alpha); }
	/// Sets the node alpha through a float component
	inline void setAlphaF(float alpha) { color_.setAlpha(static_cast<unsigned char>(alpha * 255)); }

	/// Gets the node world matrix
	inline const Matrix4x4f &worldMatrix() const { return worldMatrix_; }
	/// Gets the node local matrix
	inline const Matrix4x4f &localMatrix() const { return localMatrix_; }

  protected:
	bool updateEnabled_;
	bool drawEnabled_;

	/// A pointer to the parent node
	SceneNode *parent_;
	/// The list of child nodes
	nctl::List<SceneNode *> children_;

	/// Scale factor for node size
	float scaleFactor_;
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
	/// Absolute scale factor as calculated by the `transform()` function
	float absScaleFactor_;
	/// Absolute node rotation as calculated by the `transform()` function
	float absRotation_;

	/// Absolute node color as calculated by the `transform()` function
	Color absColor_;

	/// World transformation matrix (calculated from local and parent's world)
	Matrix4x4f worldMatrix_;
	/// Local transformation matrix
	Matrix4x4f localMatrix_;

	/// Protected copy constructor
	SceneNode(const SceneNode &);
	/// Protected assignment operator
	SceneNode &operator=(const SceneNode &);

	virtual void transform();
};

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
