#include "ScreenViewport.h"
#include "Camera.h"
#include "RenderQueue.h"
#include "RenderCommandPool.h"
#include "RenderResources.h"
#include "RenderStatistics.h"
#include "Application.h"
#include "DisplayMode.h"
#include "GLClearColor.h"
#include "GLViewport.h"
#include "GLDebug.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ScreenViewport::ScreenViewport()
    : Viewport()
{
	width_ = theApplication().widthInt();
	height_ = theApplication().heightInt();
	viewportRect_.set(0, 0, width_, height_);

	const DisplayMode displayMode = theApplication().gfxDevice().displayMode();
	if (displayMode.depthBits() == 16)
		depthStencilFormat_ = DepthStencilFormat::DEPTH16;
	else if (displayMode.depthBits() == 24)
		depthStencilFormat_ = (displayMode.stencilBits() == 8) ? DepthStencilFormat::DEPTH24_STENCIL8 : DepthStencilFormat::DEPTH24;

	rootNode_ = &theApplication().rootNode();
	type_ = Type::SCREEN;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ScreenViewport::resize(int width, int height)
{
	if (width == width_ && height == height_)
		return;

	viewportRect_.set(0, 0, width, height);

	if (camera_ != nullptr)
		camera_->setOrthoProjection(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
	RenderResources::defaultCamera_->setOrthoProjection(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

	width_ = width;
	height_ = height;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ScreenViewport::update()
{
	for (int i = chain_.size() - 1; i >= 0; i--)
	{
		if (chain_[i] && !chain_[i]->stateBits_.test(StateBitPositions::UpdatedBit))
			chain_[i]->update();
	}
	Viewport::update();
}

void ScreenViewport::visit()
{
	for (int i = chain_.size() - 1; i >= 0; i--)
	{
		if (chain_[i] && !chain_[i]->stateBits_.test(StateBitPositions::VisitedBit))
			chain_[i]->visit();
	}
	Viewport::visit();
}

void ScreenViewport::sortAndCommitQueue()
{
	// Reset all rendering statistics
	RenderStatistics::reset();

	for (int i = chain_.size() - 1; i >= 0; i--)
	{
		if (chain_[i] && !chain_[i]->stateBits_.test(StateBitPositions::CommittedBit))
			chain_[i]->sortAndCommitQueue();
	}
	Viewport::sortAndCommitQueue();

	// Now that UBOs and VBOs have been updated, they can be flushed and unmapped
	RenderResources::buffersManager().flushUnmap();
}

void ScreenViewport::draw()
{
	// Recursive calls into the chain
	Viewport::draw(0);

	for (unsigned int i = 0; i < chain_.size(); i++)
	{
		if (chain_[i])
		{
			chain_[i]->renderQueue_->clear();
			chain_[i]->stateBits_.reset();
			// Change the mode only after the whole chain has been drawn,
			// as the same viewport can appear multiple times.
			if (chain_[i]->clearMode_ == ClearMode::NEXT_FRAME_ONLY)
				chain_[i]->clearMode_ = ClearMode::THIS_FRAME_ONLY;
		}
	}

	renderQueue_->clear();
	stateBits_.reset();
	if (clearMode_ == ClearMode::NEXT_FRAME_ONLY)
		clearMode_ = ClearMode::THIS_FRAME_ONLY;

	RenderResources::buffersManager().remap();
	RenderResources::renderCommandPool().reset();
	GLDebug::reset();
}

}
