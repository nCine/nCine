#include <nctl/algorithms.h>
#include "DrawableNode.h"
#include "RenderQueue.h"
#include "RenderCommand.h"
#include "Application.h"
#include "RenderStatistics.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const Vector2f DrawableNode::AnchorCenter(0.5f, 0.5f);
const Vector2f DrawableNode::AnchorBottomLeft(0.0f, 0.0f);
const Vector2f DrawableNode::AnchorTopLeft(0.0f, 1.0f);
const Vector2f DrawableNode::AnchorBottomRight(1.0f, 0.0f);
const Vector2f DrawableNode::AnchorTopRight(1.0f, 1.0f);

unsigned short DrawableNode::imguiLayer_ = LayerBase::HIGHEST - 1024;
unsigned short DrawableNode::nuklearLayer_ = LayerBase::HIGHEST - 512;

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

DrawableNode::~DrawableNode() = default;

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

void DrawableNode::setAnchorPoint(float xx, float yy)
{
	const float clampedX = nctl::clamp(xx, 0.0f, 1.0f);
	const float clampedY = nctl::clamp(yy, 0.0f, 1.0f);
	anchorPoint_.set((clampedX - 0.5f) * width(), (clampedY - 0.5f) * height());
}

unsigned short DrawableNode::layer() const
{
	return renderCommand_->layer();
}

/*! \note The maximum admissible value for `layer` is 65535. */
void DrawableNode::setLayer(unsigned short layer)
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
