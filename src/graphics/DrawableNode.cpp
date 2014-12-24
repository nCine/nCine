#include "DrawableNode.h"
#include "RenderQueue.h"
#include "RenderCommand.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

DrawableNode::DrawableNode(SceneNode* parent, float x, float y)
	: SceneNode(parent, x, y), renderCommand_(new RenderCommand)
{

}

DrawableNode::DrawableNode(SceneNode* parent)
	: SceneNode(parent), renderCommand_(new RenderCommand)
{

}

DrawableNode::DrawableNode()
	: SceneNode(), renderCommand_(new RenderCommand)
{

}

DrawableNode::~DrawableNode()
{
	delete renderCommand_;
	renderCommand_ = NULL;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Updates the draw command and adds it to the queue
void DrawableNode::draw(RenderQueue& renderQueue)
{
	updateRenderCommand();
//	ApplyTransformations();
	renderQueue.addCommand(renderCommand_);
}

/// Applies node transformations to vertices
void DrawableNode::applyTransformations()
{
	RenderGeometry &geometry = renderCommand_->geometry();

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

/// Returns the node rendering priority
int DrawableNode::priority() const
{
	return renderCommand_->priority();
}


/// Sets the node rendering priority
void DrawableNode::setPriority(int priority)
{
	renderCommand_->setPriority(priority);
}

}
