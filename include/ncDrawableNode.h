#ifndef CLASS_NCDRAWABLENODE
#define CLASS_NCDRAWABLENODE
#include "ncSceneNode.h"
#include "ncRenderCommand.h"
#include "ncRenderQueue.h"

/// A class of objects that can be drawn through the render queue
class ncDrawableNode : public ncSceneNode
{
protected:
	ncRenderCommand m_renderCmd;

public:
	ncDrawableNode(ncSceneNode* pParent, int iX, int iY)
		: ncSceneNode(pParent, iX, iY) { }
	ncDrawableNode(ncSceneNode* pParent)
		: ncSceneNode(pParent) { }
	ncDrawableNode()
		: ncSceneNode() { }
	virtual ~ncDrawableNode();

	inline const ncRenderCommand* Command() const { return &m_renderCmd; }
	virtual void UpdateRenderCommand() = 0;
	virtual void Draw(ncRenderQueue& rRenderQueue)
	{
		UpdateRenderCommand();
		rRenderQueue.AddCommand(&m_renderCmd);
	}

	inline int Priority() const { return m_renderCmd.Priority(); }
	inline void SetPriority(int iPriority) { m_renderCmd.SetPriority(iPriority); }
};

inline ncDrawableNode::~ncDrawableNode() { }

#endif
