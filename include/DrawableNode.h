#ifndef CLASS_NCINE_DRAWABLENODE
#define CLASS_NCINE_DRAWABLENODE
#include "SceneNode.h"
#include "RenderCommand.h"
#include "RenderQueue.h"

namespace ncine {

/// A class for objects that can be drawn through the render queue
class DrawableNode : public SceneNode
{
  public:
	/// Drawing priorities from back to front
	enum PriorityBase
	{
		SCENE_PRIORITY = 0,
		HUD_PRIORITY = 128
	};

	DrawableNode(SceneNode* parent, float x, float y)
		: SceneNode(parent, x, y) { }
	DrawableNode(SceneNode* parent)
		: SceneNode(parent) { }
	DrawableNode()
		: SceneNode() { }
	virtual ~DrawableNode() { }

	/// Returns the draw command class for this node
	inline const RenderCommand* renderCommand() const { return &renderCommand_; }
	// Updates the draw command and adds it to the queue
	virtual void draw(RenderQueue& renderQueue);

	// Applies node transformations to vertices
	void applyTransformations();

	/// Returns the node rendering priority
	inline int priority() const { return renderCommand_.priority(); }
	/// Sets the node rendering priority
	inline void setPriority(int priority) { renderCommand_.setPriority(priority); }

  protected:
	/// The render command class associated with this node
	RenderCommand renderCommand_;

	/// Updates the render command
	virtual void updateRenderCommand() = 0;
};

/// Updates the draw command and adds it to the queue
inline void DrawableNode::draw(RenderQueue& renderQueue)
{
	updateRenderCommand();
//	ApplyTransformations();
	renderQueue.addCommand(&renderCommand_);
}

/// Applies node transformations to vertices
inline void DrawableNode::applyTransformations()
{
	RenderGeometry &geometry = renderCommand_.geometry();

	float sine = 0.0f;
	float cosine = 1.0f;
	if (fabs(absRotation_) > MinRotation && fabs(absRotation_) < 360.0f - MinRotation)
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

}

#endif
