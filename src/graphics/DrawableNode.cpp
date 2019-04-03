#include "DrawableNode.h"
#include "RenderQueue.h"
#include "RenderCommand.h"
#include "Application.h"
#include "RenderStatistics.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

DrawableNode::DrawableNode(SceneNode *parent, float xx, float yy)
	: SceneNode(parent, xx, yy), width_(0.0f), height_(0.0f),
	  renderCommand_(nctl::makeUnique<RenderCommand>())
{

}

DrawableNode::DrawableNode(SceneNode *parent, const Vector2f &position)
	: DrawableNode(parent, position.x, position.y)
{

}


DrawableNode::DrawableNode(SceneNode *parent)
	: DrawableNode(parent, 0.0f, 0.0f)
{

}

DrawableNode::DrawableNode()
	: DrawableNode(nullptr, 0.0f, 0.0f)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void DrawableNode::draw(RenderQueue &renderQueue)
{
	const bool cullingEnabled = theApplication().renderingSettings().cullingEnabled;

	if (cullingEnabled)
	{
		updateAabb();

		if (aabb_.overlaps(theApplication().gfxDevice().screenRect()))
		{
			updateRenderCommand();
			renderQueue.addCommand(renderCommand_.get());
		}
		else
			RenderStatistics::addCulledNode();
	}
	else
	{
		updateRenderCommand();
		renderQueue.addCommand(renderCommand_.get());
	}
}

unsigned int DrawableNode::layer() const
{
	return renderCommand_->layer();
}

/*! \note The maximum admissible value for `layer` is 65535. */
void DrawableNode::setLayer(unsigned int layer)
{
	renderCommand_->setLayer(layer);
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

void DrawableNode::updateAabb()
{
	float rotatedWidth = absWidth();
	float rotatedHeight = absHeight();

	if (absRotation_ > MinRotation || absRotation_ < -MinRotation)
	{
		float sinRot = sinf(absRotation_ * fDegToRad);
		float cosRot = cosf(absRotation_ * fDegToRad);
		rotatedWidth = fabsf(absHeight() * sinRot) + fabsf(absWidth() * cosRot);
		rotatedHeight = fabsf(absWidth() * sinRot) + fabsf(absHeight() * cosRot);
	}

	aabb_ = Rectf::fromCenterAndSize(absX_, absY_, rotatedWidth, rotatedHeight);
}

}
