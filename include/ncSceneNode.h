#ifndef CLASS_NCSCENENODE
#define CLASS_NCSCENENODE

#include "ncObject.h"
#include "ncList.h"
#include "ncVector2f.h"

class ncRenderQueue;

/// The base class for the object hierarchy of a game
class ncSceneNode : public ncObject
{
protected:
	ncSceneNode* m_pParent;
	ncList<ncSceneNode *> m_children;

	/// Absolute X coordinate as calculated by the Transform() function
	float m_absX;
	/// Absolute Y coordinate as calculated by the Transform() function
	float m_absY;

	virtual void Transform();

public:
	/// Relative X coordinate as a public property
	float x;
	/// Relative Y coordinate as a public property
	float y;
	bool bShouldUpdate;
	bool bShouldDraw;

	ncSceneNode(ncSceneNode* pParent, float fX, float fY)
		: m_pParent(NULL), x(fX), y(fY), bShouldUpdate(true), bShouldDraw(true)
	{
		m_eType = SCENENODE_TYPE;

		if (pParent)
			pParent->AddChildNode(this);
	}
	ncSceneNode(ncSceneNode* pParent)
		: m_pParent(NULL), x(0.0f), y(0.0f), bShouldUpdate(true), bShouldDraw(true)
	{
		m_eType = SCENENODE_TYPE;

		if (pParent)
			pParent->AddChildNode(this);
	}
	ncSceneNode()
		: ncObject(), m_pParent(NULL), x(0.0f), y(0.0f), bShouldUpdate(true), bShouldDraw(true) { m_eType = SCENENODE_TYPE; }
	virtual ~ncSceneNode();

	inline static eObjectType sType() { return SCENENODE_TYPE; }
	static ncSceneNode* FromId(unsigned int uId);

	const ncSceneNode* ParentNode() const { return m_pParent; }
	const ncList<ncSceneNode *>& Children() const { return m_children; }
	// Add a node as a children of this one
	bool AddChildNode(ncSceneNode *pChildNode);
	// Remove a children of this node, without reparenting nephews
	bool RemoveChildNode(ncSceneNode *pChildNode);
	// Remove a children of this node reparenting nephews as children.
	bool UnlinkChildNode(ncSceneNode *pChildNode);

	virtual void Update(unsigned long int ulInterval);
	virtual void Visit(ncRenderQueue& rRenderQueue);
	virtual void Draw(ncRenderQueue& rRenderQueue) { }

	inline ncVector2f Position() { return ncVector2f(x, y); }
	inline ncVector2f AbsPosition() { return ncVector2f(m_absX, m_absY); }
	inline void SetPosition(float fX, float fY) { x = fX; y = fY; }
	inline void SetPosition(ncVector2f pos) { x = pos.x; y = pos.y; }
};

#endif
