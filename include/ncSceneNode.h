#ifndef CLASS_NCSCENENODE
#define CLASS_NCSCENENODE

#include "ncList.h"
#include "ncPoint.h"
#include "ncRect.h"

/// The base class for the object hierarchy of a game
class ncSceneNode
{
protected:
	enum eNodeType {
		BASE_TYPE = 0,
		SPRITE_TYPE
	};

	eNodeType m_eType;
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
		: m_eType(BASE_TYPE), m_pParent(NULL), x(iX), y(iY), bShouldUpdate(true), bShouldDraw(true)
	{
		if (pParent)
			pParent->AddChildNode(this);
	}
	ncSceneNode(ncSceneNode* pParent)
		: m_eType(BASE_TYPE), m_pParent(NULL), x(0), y(0), bShouldUpdate(true), bShouldDraw(true)
	{
		if (pParent)
			pParent->AddChildNode(this);
	}
	ncSceneNode() : m_eType(BASE_TYPE), m_pParent(NULL), x(0), y(0), bShouldUpdate(true), bShouldDraw(true) { }
	virtual ~ncSceneNode();

	inline static eNodeType sType() { return BASE_TYPE; }
	inline eNodeType Type() const { return m_eType; }

	ncSceneNode* ParentNode() const { return m_pParent; }
	void AddChildNode(ncSceneNode *pChildNode);
	void RemoveChildNode(ncSceneNode *pChildNode);

	virtual void Update(unsigned long int ulInterval);

	inline ncPoint Position() { return ncPoint(x, y); }
	inline ncPoint AbsPosition() { return ncPoint(m_absX, m_absY); }
	inline void setPosition(int iX, int iY) { x = iX; y = iY; }
	inline void setPosition(ncPoint pos) { x = pos.x; y = pos.y; }

	friend class ncRenderGraph;
};

#endif
