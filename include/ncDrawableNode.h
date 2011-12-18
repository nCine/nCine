#ifndef CLASS_NCDRAWABLENODE
#define CLASS_NCDRAWABLENODE
#include "ncSceneNode.h"
#include "ncRenderCommand.h"
#include "ncRenderQueue.h"

/// A class for objects that can be drawn through the render queue
class ncDrawableNode : public ncSceneNode
{
protected:
	/// The render command class associated with this node
	ncRenderCommand m_renderCmd;
	/// Updates the render command
	virtual void UpdateRenderCommand() = 0;

public:

	/// Drawing priorities from back to front
	enum ePriorityBase {
		SCENE_PRIORITY = 0,
		HUD_PRIORITY = 128
	};

	ncDrawableNode(ncSceneNode* pParent, int iX, int iY)
		: ncSceneNode(pParent, iX, iY) { }
	ncDrawableNode(ncSceneNode* pParent)
		: ncSceneNode(pParent) { }
	ncDrawableNode()
		: ncSceneNode() { }
	virtual ~ncDrawableNode();

	/// Returns the draw command class for this node
	inline const ncRenderCommand* Command() const { return &m_renderCmd; }
	/// Updates the draw command and add it to the queue
	virtual void Draw(ncRenderQueue& rRenderQueue)
	{
		UpdateRenderCommand();
		rRenderQueue.AddCommand(&m_renderCmd);
	}

	/// Returns the node rendering priority
	inline int Priority() const { return m_renderCmd.Priority(); }
	/// Sets the node rendering priority
	inline void SetPriority(int iPriority) { m_renderCmd.SetPriority(iPriority); }
};

inline ncDrawableNode::~ncDrawableNode() { }

#endif
