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
	enum PriorityBase
	{
		SCENE_PRIORITY = 0,
		HUD_PRIORITY = 128
	};

	ncDrawableNode(ncSceneNode* parent, int x, int y)
		: ncSceneNode(parent, x, y) { }
	ncDrawableNode(ncSceneNode* parent)
		: ncSceneNode(parent) { }
	ncDrawableNode()
		: ncSceneNode() { }
	virtual ~ncDrawableNode() { }

	/// Returns the draw command class for this node
	inline const ncRenderCommand* renderCommand() const { return &renderCommand_; }
	// Updates the draw command and adds it to the queue
	virtual void draw(ncRenderQueue& renderQueue);

	// Applies node transformations to vertices
	void applyTransformations();

	/// Returns the node rendering priority
	inline int priority() const { return renderCommand_.priority(); }
	/// Sets the node rendering priority
	inline void setPriority(int priority) { renderCommand_.setPriority(priority); }

  protected:
	/// The render command class associated with this node
	ncRenderCommand renderCommand_;

	/// Updates the render command
	virtual void updateRenderCommand() = 0;
};

/// Updates the draw command and adds it to the queue
inline void ncDrawableNode::draw(ncRenderQueue& renderQueue)
{
	updateRenderCommand();
//	ApplyTransformations();
	renderQueue.addCommand(&renderCommand_);
}

/// Applies node transformations to vertices
inline void ncDrawableNode::applyTransformations()
{
	ncRenderGeometry &geometry = renderCommand_.geometry();

	float sine = 0.0f;
	float cosine = 1.0f;
	if (abs(absRotation_) > MinRotation && abs(absRotation_) < 360.0f - MinRotation)
	{
		sine = sinf(-absRotation_ * M_PI / 180.0f);
		cosine = cosf(-absRotation_ * M_PI / 180.0f);
	}

	for (int i = geometry.firstVertex(); i < geometry.numVertices() * 2; i = i + 2)
	{
		float x = geometry.vertexPointer()[i] * absScaleFactor_;			float y = geometry.vertexPointer()[i + 1] * absScaleFactor_;
		geometry.vertexPointer()[i] = absX_ + x * cosine - y * sine;		geometry.vertexPointer()[i + 1] = absY_ + y * cosine + x * sine;
	}
}

#endif
