#ifndef CLASS_NCSCENENODE
#define CLASS_NCSCENENODE

#include "ncObject.h"
#include "ncList.h"
#include "ncVector2f.h"
#include "ncColor.h"

class ncRenderQueue;

/// The base class for the transformation nodes hierarchy
class ncSceneNode : public ncObject
{
  public:
	/// The minimum amount of rotation to trigger a sine and cosine calculation
	static const float MinRotation;

	/// Relative X coordinate as a public property
	float x;
	/// Relative Y coordinate as a public property
	float y;

	bool shouldUpdate_;
	bool shouldDraw_;

	ncSceneNode(ncSceneNode* parent, float x, float y);
	ncSceneNode(ncSceneNode* parent);
	ncSceneNode();
	virtual ~ncSceneNode();

	inline static ObjectType sType() { return SCENENODE_TYPE; }

	const ncSceneNode* parentNode() const { return parent_; }
	const ncList<ncSceneNode *>& children() const { return children_; }
	// Adds a node as a child of this one
	bool addChildNode(ncSceneNode *childNode);
	// Removes a child of this node, without reparenting nephews
	bool removeChildNode(ncSceneNode *childNode);
	// Removes a child of this node while iterating on children, without reparenting nephews
	bool removeChildNode(ncList<ncSceneNode *>::Iterator it);
	// Removes a child of this node reparenting nephews as children
	bool unlinkChildNode(ncSceneNode *childNode);

	// Called once every frame to update the node
	virtual void update(float interval);
	// Draws the node and visits its children
	virtual void visit(ncRenderQueue& renderQueue);
	/// Renders the node
	virtual void draw(ncRenderQueue& renderQueue) { }

	/// Returns node position relative to its parent
	inline ncVector2f position() const { return ncVector2f(x, y); }
	/// Returns absolute node position
	inline ncVector2f absPosition() const { return ncVector2f(absX_, absY_); }
	/// Sets the sprite position through two coordinates
	inline void setPosition(float xx, float yy) { x = xx; y = yy; }
	/// Sets the sprite position through a vector
	inline void setPosition(const ncVector2f& pos) { x = pos.x; y = pos.y; }
	/// Moves a node based on two offsets
	inline void move(float xx, float yy) { x += xx; y += yy; }
	/// Add a move vector to the node current position
	inline void move(const ncVector2f& pos) { x += pos.x; y += pos.y; }

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
	// Sets the node rotation in degrees
	void setRotation(float rotation);

	/// Gets the node color
	inline ncColor color() const { return color_; }
	/// Gets the node absolute color
	inline ncColor absColor() const { return absColor_; }
	/// Sets the node color through a ncColor class
	inline void setColor(ncColor color) { color_ = color; }
	/// Sets the node color through unsigned char components
	inline void setColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) { color_.set(red, green, blue, alpha); }
	/// Sets the node color through float components
	inline void setColorF(float red, float green, float blue, float alpha) { color_.setF(red, green, blue, alpha); }
	/// Sets the node alpha through an unsigned char component
	inline void setAlpha(unsigned char alpha) { color_.setAlpha(alpha); }
	/// Sets the node alpha through a float component
	inline void setAlphaF(float alpha) { color_.setAlphaF(alpha); }

  protected:
	ncSceneNode* parent_;
	ncList<ncSceneNode *> children_;

	/// Scale factor for node size
	float scaleFactor_;
	/// Degrees for clock-wise node rotation
	float rotation_;

	/// Node color for transparency, translucency, etc...
	/** Even if the base scene node is not always drawable, it carries
		color information to easily pass that information to its children */
	ncColor color_;

	/// Absolute X coordinate as calculated by the Transform() function
	float absX_;
	/// Absolute Y coordinate as calculated by the Transform() function
	float absY_;
	/// Absolute scale factor as calculated by the Transform() function
	float absScaleFactor_;
	/// Absolute node rotation as calculated by the Transform() function
	float absRotation_;

	/// Absolute node color as calculated by the Transform() function
	ncColor absColor_;

	virtual void transform();
};

/// Sets the node rotation in degrees
inline void ncSceneNode::setRotation(float rotation)
{
	while (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	while (rotation < -360.0f)
	{
		rotation += 360.0f;
	}

	rotation_ = rotation;
}

#endif
