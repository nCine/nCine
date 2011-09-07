#ifndef CLASS_NCSCENENODE
#define CLASS_NCSCENENODE

#include "ncObject.h"
#include "ncList.h"
#include "ncPoint.h"
#include "ncRect.h"

class ncRenderCommand;

/// The base class for the object hierarchy of a game
class ncSceneNode : public ncObject
{
protected:
	ncSceneNode* m_pParent;
	ncList<ncSceneNode *> m_children;

	/// Absolute X coordinate as calculated by the RenderGraph class
	int m_absX;
	/// Absolute Y coordinate as calculated by the RenderGraph class
	int m_absY;

public:
	/// Relative X coordinate as a public property
	int x;
	/// Relative Y coordinate as a public property
	int y;
	bool bShouldUpdate;
	bool bShouldDraw;

	ncSceneNode(ncSceneNode* pParent, int iX, int iY)
		: m_pParent(NULL), x(iX), y(iY), bShouldUpdate(true), bShouldDraw(true)
	{
		m_eType = SCENENODE_TYPE;

		if (pParent)
			pParent->AddChildNode(this);
	}
	ncSceneNode(ncSceneNode* pParent)
		: m_pParent(NULL), x(0), y(0), bShouldUpdate(true), bShouldDraw(true)
	{
		m_eType = SCENENODE_TYPE;

		if (pParent)
			pParent->AddChildNode(this);
	}
	ncSceneNode()
		: ncObject(), m_pParent(NULL), x(0), y(0), bShouldUpdate(true), bShouldDraw(true) { m_eType = SCENENODE_TYPE; }
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

	inline ncPoint Position() { return ncPoint(x, y); }
	inline ncPoint AbsPosition() { return ncPoint(m_absX, m_absY); }
	inline void setPosition(int iX, int iY) { x = iX; y = iY; }
	inline void setPosition(ncPoint pos) { x = pos.x; y = pos.y; }

	friend class ncRenderGraph;
	friend class ncRenderQueue;
};

#endif
