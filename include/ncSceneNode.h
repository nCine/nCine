#ifndef CLASS_NCSCENENODE
#define CLASS_NCSCENENODE

#include "ncObject.h"
#include "ncList.h"
#include "ncVector2f.h"

class ncRenderQueue;

/// The base class for the transformation nodes hierarchy
class ncSceneNode : public ncObject
{
protected:
	ncSceneNode* m_pParent;
	ncList<ncSceneNode *> m_children;

	/// Scale factor for node size
	float m_fScaleFactor;
	/// Degrees for clock-wise node rotation
	float m_fRotation;

	/// Absolute X coordinate as calculated by the Transform() function
	float m_fAbsX;
	/// Absolute Y coordinate as calculated by the Transform() function
	float m_fAbsY;
	/// Absolute scale factor as calculated by the Transform() function
	float m_fAbsScaleFactor;
	/// Absolute node rotation as calculated by the Transform() function
	float m_fAbsRotation;

	virtual void Transform();

public:
	/// The minimum amount of rotation to trigger a sine and cosine calculation
	static const float sMinRotation;

	/// Relative X coordinate as a public property
	float x;
	/// Relative Y coordinate as a public property
	float y;

	bool bShouldUpdate;
	bool bShouldDraw;

	ncSceneNode(ncSceneNode* pParent, float fX, float fY)
		: m_pParent(NULL),
		  m_fScaleFactor(1.0f), m_fRotation(0.0f), m_fAbsX(0.0f), m_fAbsY(0.0f), m_fAbsScaleFactor(1.0f), m_fAbsRotation(0.0f),
		  x(fX), y(fY), bShouldUpdate(true), bShouldDraw(true)
	{
		m_eType = SCENENODE_TYPE;

		if (pParent)
			pParent->AddChildNode(this);
	}
	ncSceneNode(ncSceneNode* pParent)
		: m_pParent(NULL),
		  m_fScaleFactor(1.0f), m_fRotation(0.0f), m_fAbsX(0.0f), m_fAbsY(0.0f), m_fAbsScaleFactor(1.0f), m_fAbsRotation(0.0f),
		  x(0.0f), y(0.0f), bShouldUpdate(true), bShouldDraw(true)
	{
		m_eType = SCENENODE_TYPE;

		if (pParent)
			pParent->AddChildNode(this);
	}
	ncSceneNode()
		: ncObject(), m_pParent(NULL),
		  m_fScaleFactor(1.0f), m_fRotation(0.0f), m_fAbsX(0.0f), m_fAbsY(0.0f), m_fAbsScaleFactor(1.0f), m_fAbsRotation(0.0f),
		  x(0.0f), y(0.0f), bShouldUpdate(true), bShouldDraw(true)
	{
		m_eType = SCENENODE_TYPE;
	}
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
	virtual void Update(unsigned long int ulInterval);
	// Draws the node and visits its children
	virtual void Visit(ncRenderQueue& rRenderQueue);
	/// Renders the node
	virtual void Draw(ncRenderQueue& rRenderQueue) { }

	/// Returns node position relative to its parent
	inline ncVector2f Position() const { return ncVector2f(x, y); }
	/// Returns absolute node position
	inline ncVector2f AbsPosition() const { return ncVector2f(m_fAbsX, m_fAbsY); }
	/// Moves a node based on two offsets
	inline void Move(float fX, float fY) { x += fX; y += fY; }
	/// Add a move vector to the node current position
	inline void Move(const ncVector2f& pos) { x += pos.x; y += pos.y; }
	/// Sets the sprite position through two coordinates
	inline void SetPosition(float fX, float fY) { x = fX; y = fY; }
	/// Sets the sprite position through a vector
	inline void SetPosition(const ncVector2f& pos) { x = pos.x; y = pos.y; }

	/// Gets the node scale factor
	inline float Scale() const { return m_fScaleFactor; }
	/// Scales the node size
	inline void SetScale(float fScaleFactor) { m_fScaleFactor = fScaleFactor; }
	/// Gets the node absolute scale factor
	inline float AbsScale() const { return m_fAbsScaleFactor; }

	/// Gets the node rotation in degrees
	inline float Rotation() const { return m_fRotation; }
	/// Sets the node rotation in degrees
	inline void SetRotation(float fRotation)
	{
		while (fRotation > 360.0f)
			fRotation -= 360.0f;
		while (fRotation < -360.0f)
			fRotation += 360.0f;

		m_fRotation = fRotation;
	}
	/// Gets the node absolute rotation in degrees
	inline float AbsRotation() const { return m_fAbsRotation; }
};

#endif
