#ifndef CLASS_NCDRAWABLENODE
#define CLASS_NCDRAWABLENODE
#include "ncSceneNode.h"
#include "ncRenderCommand.h"

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
};

inline ncDrawableNode::~ncDrawableNode() { }

#endif
