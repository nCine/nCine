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
	static const float sMinRotation;

	/// Relative X coordinate as a public property
	float x;
	/// Relative Y coordinate as a public property
	float y;

	bool bShouldUpdate;
	bool bShouldDraw;

	ncSceneNode(ncSceneNode* pParent, float fX, float fY);
	ncSceneNode(ncSceneNode* pParent);
	ncSceneNode();
	virtual ~ncSceneNode();

	inline static eObjectType sType() { return SCENENODE_TYPE; }

	const ncSceneNode* ParentNode() const { return m_pParent; }
	const ncList<ncSceneNode *>& Children() const { return m_children; }
	// Adds a node as a child of this one
	bool AddChildNode(ncSceneNode *pChildNode);
	// Removes a child of this node, without reparenting nephews
	bool RemoveChildNode(ncSceneNode *pChildNode);
	// Removes a child of this node while iterating on children, without reparenting nephews
	bool RemoveChildNode(ncList<ncSceneNode *>::Iterator it);
	// Removes a child of this node reparenting nephews as children
	bool UnlinkChildNode(ncSceneNode *pChildNode);

	// Called once every frame to update the node
	virtual void Update(float fInterval);
	// Draws the node and visits its children
	virtual void Visit(ncRenderQueue& rRenderQueue);
	/// Renders the node
	virtual void Draw(ncRenderQueue& rRenderQueue) { }

	/// Returns node position relative to its parent
	inline ncVector2f Position() const { return ncVector2f(x, y); }
	/// Returns absolute node position
	inline ncVector2f AbsPosition() const { return ncVector2f(m_fAbsX, m_fAbsY); }
	/// Sets the sprite position through two coordinates
	inline void SetPosition(float fX, float fY) { x = fX; y = fY; }
	/// Sets the sprite position through a vector
	inline void SetPosition(const ncVector2f& pos) { x = pos.x; y = pos.y; }
	/// Moves a node based on two offsets
	inline void Move(float fX, float fY) { x += fX; y += fY; }
	/// Add a move vector to the node current position
	inline void Move(const ncVector2f& pos) { x += pos.x; y += pos.y; }

	/// Gets the node scale factor
	inline float Scale() const { return m_fScaleFactor; }
	/// Gets the node absolute scale factor
	inline float AbsScale() const { return m_fAbsScaleFactor; }
	/// Scales the node size
	inline void SetScale(float fScaleFactor) { m_fScaleFactor = fScaleFactor; }

	/// Gets the node rotation in degrees
	inline float Rotation() const { return m_fRotation; }
	/// Gets the node absolute rotation in degrees
	inline float AbsRotation() const { return m_fAbsRotation; }
	// Sets the node rotation in degrees
	void SetRotation(float fRotation);

	/// Gets the node color
	inline ncColor Color() const { return m_color; }
	/// Gets the node absolute color
	inline ncColor AbsColor() const { return m_absColor; }
	/// Sets the node color through a ncColor class
	inline void SetColor(ncColor color) { m_color = color; }
	/// Sets the node color through unsigned char components
	inline void SetColor(unsigned char ubR, unsigned char ubG, unsigned char ubB, unsigned char ubA) { m_color.Set(ubR, ubG, ubB, ubA); }
	/// Sets the node color through float components
	inline void SetColorF(float fR, float fG, float fB, float fA) { m_color.SetF(fR, fG, fB, fA); }
	/// Sets the node alpha through an unsigned char component
	inline void SetAlpha(unsigned char ubA) { m_color.SetAlpha(ubA); }
	/// Sets the node alpha through a float component
	inline void SetAlphaF(float fA) { m_color.SetAlphaF(fA); }

protected:
	ncSceneNode* m_pParent;
	ncList<ncSceneNode *> m_children;

	/// Scale factor for node size
	float m_fScaleFactor;
	/// Degrees for clock-wise node rotation
	float m_fRotation;

	/// Node color for transparency, translucency, etc...
	/** Even if the base scene node is not always drawable, it carries
		color information to easily pass that information to its children */
	ncColor m_color;

	/// Absolute X coordinate as calculated by the Transform() function
	float m_fAbsX;
	/// Absolute Y coordinate as calculated by the Transform() function
	float m_fAbsY;
	/// Absolute scale factor as calculated by the Transform() function
	float m_fAbsScaleFactor;
	/// Absolute node rotation as calculated by the Transform() function
	float m_fAbsRotation;

	/// Absolute node color as calculated by the Transform() function
	ncColor m_absColor;

	virtual void Transform();
};

/// Sets the node rotation in degrees
inline void ncSceneNode::SetRotation(float fRotation)
{
	while (fRotation > 360.0f)
		fRotation -= 360.0f;
	while (fRotation < -360.0f)
		fRotation += 360.0f;

	m_fRotation = fRotation;
}

#endif
