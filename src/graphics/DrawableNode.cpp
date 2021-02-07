#include <nctl/algorithms.h>
#include "DrawableNode.h"
#include "RenderQueue.h"
#include "RenderCommand.h"
#include "Application.h"
#include "RenderStatistics.h"

namespace ncine {

namespace {

	GLenum toGlBlendingFactor(DrawableNode::BlendingFactor blendingFactor)
	{
		switch (blendingFactor)
		{
			case DrawableNode::BlendingFactor::ZERO: return GL_ZERO;
			case DrawableNode::BlendingFactor::ONE: return GL_ONE;
			case DrawableNode::BlendingFactor::SRC_COLOR: return GL_SRC_COLOR;
			case DrawableNode::BlendingFactor::ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
			case DrawableNode::BlendingFactor::DST_COLOR: return GL_DST_COLOR;
			case DrawableNode::BlendingFactor::ONE_MINUS_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
			case DrawableNode::BlendingFactor::SRC_ALPHA: return GL_SRC_ALPHA;
			case DrawableNode::BlendingFactor::ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
			case DrawableNode::BlendingFactor::DST_ALPHA: return GL_DST_ALPHA;
			case DrawableNode::BlendingFactor::ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
			case DrawableNode::BlendingFactor::CONSTANT_COLOR: return GL_CONSTANT_COLOR;
			case DrawableNode::BlendingFactor::ONE_MINUS_CONSTANT_COLOR: return GL_ONE_MINUS_CONSTANT_COLOR;
			case DrawableNode::BlendingFactor::CONSTANT_ALPHA: return GL_CONSTANT_ALPHA;
			case DrawableNode::BlendingFactor::ONE_MINUS_CONSTANT_ALPHA: return GL_ONE_MINUS_CONSTANT_ALPHA;
			case DrawableNode::BlendingFactor::SRC_ALPHA_SATURATE: return GL_SRC_ALPHA_SATURATE;
		}
		return GL_ZERO;
	}

	DrawableNode::BlendingFactor fromGlBlendingFactor(GLenum blendingFactor)
	{
		switch (blendingFactor)
		{
			case GL_ZERO: return DrawableNode::BlendingFactor::ZERO;
			case GL_ONE: return DrawableNode::BlendingFactor::ONE;
			case GL_SRC_COLOR: return DrawableNode::BlendingFactor::SRC_COLOR;
			case GL_ONE_MINUS_SRC_COLOR: return DrawableNode::BlendingFactor::ONE_MINUS_SRC_COLOR;
			case GL_DST_COLOR: return DrawableNode::BlendingFactor::DST_COLOR;
			case GL_ONE_MINUS_DST_COLOR: return DrawableNode::BlendingFactor::ONE_MINUS_DST_COLOR;
			case GL_SRC_ALPHA: return DrawableNode::BlendingFactor::SRC_ALPHA;
			case GL_ONE_MINUS_SRC_ALPHA: return DrawableNode::BlendingFactor::ONE_MINUS_SRC_ALPHA;
			case GL_DST_ALPHA: return DrawableNode::BlendingFactor::DST_ALPHA;
			case GL_ONE_MINUS_DST_ALPHA: return DrawableNode::BlendingFactor::ONE_MINUS_DST_ALPHA;
			case GL_CONSTANT_COLOR: return DrawableNode::BlendingFactor::CONSTANT_COLOR;
			case GL_ONE_MINUS_CONSTANT_COLOR: return DrawableNode::BlendingFactor::ONE_MINUS_CONSTANT_COLOR;
			case GL_CONSTANT_ALPHA: return DrawableNode::BlendingFactor::CONSTANT_ALPHA;
			case GL_ONE_MINUS_CONSTANT_ALPHA: return DrawableNode::BlendingFactor::ONE_MINUS_CONSTANT_ALPHA;
			case GL_SRC_ALPHA_SATURATE: return DrawableNode::BlendingFactor::SRC_ALPHA_SATURATE;
		}
		return DrawableNode::BlendingFactor::ZERO;
	}

}

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
	renderCommand_->setIdSortKey(id());
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

DrawableNode::DrawableNode(DrawableNode &&) = default;

DrawableNode &DrawableNode::operator=(DrawableNode &&) = default;

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

bool DrawableNode::isBlendingEnabled() const
{
	return renderCommand_->material().isBlendingEnabled();
}

void DrawableNode::setBlendingEnabled(bool blendingEnabled)
{
	renderCommand_->material().setBlendingEnabled(blendingEnabled);
}

DrawableNode::BlendingFactor DrawableNode::srcBlendingFactor() const
{
	return fromGlBlendingFactor(renderCommand_->material().srcBlendingFactor());
}

DrawableNode::BlendingFactor DrawableNode::destBlendingFactor() const
{
	return fromGlBlendingFactor(renderCommand_->material().destBlendingFactor());
}

void DrawableNode::setBlendingPreset(BlendingPreset blendingPreset)
{
	switch (blendingPreset)
	{
		case BlendingPreset::DISABLED:
			renderCommand_->material().setBlendingFactors(toGlBlendingFactor(BlendingFactor::ONE), toGlBlendingFactor(BlendingFactor::ZERO));
			break;
		case BlendingPreset::ALPHA:
			renderCommand_->material().setBlendingFactors(toGlBlendingFactor(BlendingFactor::SRC_ALPHA), toGlBlendingFactor(BlendingFactor::ONE_MINUS_SRC_ALPHA));
			break;
		case BlendingPreset::PREMULTIPLIED_ALPHA:
			renderCommand_->material().setBlendingFactors(toGlBlendingFactor(BlendingFactor::ONE), toGlBlendingFactor(BlendingFactor::ONE_MINUS_SRC_ALPHA));
			break;
		case BlendingPreset::ADDITIVE:
			renderCommand_->material().setBlendingFactors(toGlBlendingFactor(BlendingFactor::SRC_ALPHA), toGlBlendingFactor(BlendingFactor::ONE));
			break;
		case BlendingPreset::MULTIPLY:
			renderCommand_->material().setBlendingFactors(toGlBlendingFactor(BlendingFactor::DST_COLOR), toGlBlendingFactor(BlendingFactor::ZERO));
			break;
	}
}

void DrawableNode::setBlendingFactors(BlendingFactor srcBlendingFactor, BlendingFactor destBlendingFactor)
{
	renderCommand_->material().setBlendingFactors(toGlBlendingFactor(srcBlendingFactor), toGlBlendingFactor(destBlendingFactor));
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
	const float width = (absScale().x > 0.0f) ? absWidth() : -absWidth();
	const float height = (absScale().y > 0.0f) ? absHeight() : -absHeight();
	float rotatedWidth = width;
	float rotatedHeight = height;

	if (absRotation_ > MinRotation || absRotation_ < -MinRotation)
	{
		const float sinRot = sinf(absRotation_ * fDegToRad);
		const float cosRot = cosf(absRotation_ * fDegToRad);
		rotatedWidth = fabsf(width * cosRot) + fabsf(height * sinRot);
		rotatedHeight = fabsf(width * sinRot) + fabsf(height * cosRot);
	}

	aabb_ = Rectf::fromCenterAndSize(absX_, absY_, rotatedWidth, rotatedHeight);
}

DrawableNode::DrawableNode(const DrawableNode &other)
    : SceneNode(other),
      width_(other.width_), height_(other.height_),
      renderCommand_(nctl::makeUnique<RenderCommand>())
{
	renderCommand_->setIdSortKey(id());
	setBlendingEnabled(other.isBlendingEnabled());
	setBlendingFactors(other.srcBlendingFactor(), other.destBlendingFactor());
	setLayer(other.layer());
}

}
