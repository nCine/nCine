#ifndef CLASS_NCDRAWABLENODE
#define CLASS_NCDRAWABLENODE
#include "ncSceneNode.h"
#include "ncRenderCommand.h"
#include "ncRenderQueue.h"

/// A class for objects that can be drawn through the render queue
class ncDrawableNode : public ncSceneNode
{
 public:
	/// Drawing priorities from back to front
	enum ePriorityBase
	{
		SCENE_PRIORITY = 0,
		HUD_PRIORITY = 128
	};

	ncDrawableNode(ncSceneNode* pParent, int iX, int iY)
		: ncSceneNode(pParent, iX, iY) { }
	ncDrawableNode(ncSceneNode* pParent)
		: ncSceneNode(pParent) { }
	ncDrawableNode()
		: ncSceneNode() { }
	virtual ~ncDrawableNode() { }

	/// Returns the draw command class for this node
	inline const ncRenderCommand* Command() const { return &m_renderCmd; }
	/// Updates the draw command and adds it to the queue
	virtual void Draw(ncRenderQueue& rRenderQueue)
	{
		UpdateRenderCommand();
//		ApplyTransformations();
		rRenderQueue.AddCommand(&m_renderCmd);
	}

	// Applies node transformations to vertices
	void ApplyTransformations();

	/// Returns the node rendering priority
	inline int Priority() const { return m_renderCmd.Priority(); }
	/// Sets the node rendering priority
	inline void SetPriority(int iPriority) { m_renderCmd.SetPriority(iPriority); }

 protected:
	/// Updates the render command
	virtual void UpdateRenderCommand() = 0;

	/// The render command class associated with this node
	ncRenderCommand m_renderCmd;
};

/// Applies node transformations to vertices
inline void ncDrawableNode::ApplyTransformations()
{
	ncRenderGeometry &rGeom = m_renderCmd.Geometry();

	float sine = 0.0f;
	float cosine = 1.0f;
	if (abs(m_fAbsRotation) > sMinRotation && abs(m_fAbsRotation) < 360.0f - sMinRotation)
	{
		sine = sinf(-m_fAbsRotation * M_PI / 180.0f);
		cosine = cosf(-m_fAbsRotation * M_PI / 180.0f);
	}

	for (int i = rGeom.FirstVertex(); i < rGeom.NumVertices() * 2; i = i + 2)
	{
		float fX = rGeom.VertexPointer()[i] * m_fAbsScaleFactor;			float fY = rGeom.VertexPointer()[i + 1] * m_fAbsScaleFactor;
		rGeom.VertexPointer()[i] = m_fAbsX + fX * cosine - fY * sine;		rGeom.VertexPointer()[i + 1] = m_fAbsY + fY * cosine + fX * sine;
	}
}

#endif
