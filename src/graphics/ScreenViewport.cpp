#include "ScreenViewport.h"
#include "RenderQueue.h"
#include "RenderCommandPool.h"
#include "RenderResources.h"
#include "RenderStatistics.h"
#include "SceneNode.h"
#include "Application.h"
#include "DisplayMode.h"
#include "GLClearColor.h"
#include "GLViewport.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ScreenViewport::ScreenViewport()
    : Viewport(Type::SCREEN)
{
	width_ = theApplication().widthInt();
	height_ = theApplication().heightInt();
	viewportRect_.set(0, 0, width_, height_);

	const DisplayMode displayMode = theApplication().gfxDevice().displayMode();
	if (displayMode.alphaBits() == 8)
		colorFormat_ = ColorFormat::RGBA8;

	if (displayMode.depthBits() == 16)
		depthStencilFormat_ = DepthStencilFormat::DEPTH16;
	else if (displayMode.depthBits() == 24)
		depthStencilFormat_ = (displayMode.stencilBits() == 8) ? DepthStencilFormat::DEPTH24_STENCIL8 : DepthStencilFormat::DEPTH24;

	rootNode_ = &theApplication().rootNode();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ScreenViewport::update()
{
	if (nextViewport_)
		nextViewport_->update();

	if (rootNode_)
		rootNode_->update(theApplication().interval());
}

void ScreenViewport::visit()
{
	if (nextViewport_)
		nextViewport_->visit();

	if (rootNode_)
		rootNode_->visit(*renderQueue_);
}

void ScreenViewport::sortAndCommitQueue()
{
	// Reset all rendering statistics
	RenderStatistics::reset();

	if (nextViewport_)
		nextViewport_->sortAndCommitQueue();

	renderQueue_->sortAndCommit();

	// Now that UBOs and VBOs have been updated, they can be flushed and unmapped
	RenderResources::buffersManager().flushUnmap();
}

void ScreenViewport::draw()
{
	if (nextViewport_)
		nextViewport_->draw();

	GLClearColor::State clearColorState = GLClearColor::state();
	GLClearColor::setColor(clearColor_);
	if (clearMode_ == ClearMode::EVERY_FRAME || clearMode_ == ClearMode::THIS_FRAME_ONLY)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	GLClearColor::setState(clearColorState);

	GLViewport::State viewportState = GLViewport::state();
	GLViewport::setRect(viewportRect_);
	renderQueue_->draw();
	GLViewport::setState(viewportState);

	if (clearMode_ == ClearMode::THIS_FRAME_ONLY)
		clearMode_ = ClearMode::NEVER;
	else if (clearMode_ == ClearMode::NEXT_FRAME_ONLY)
		clearMode_ = ClearMode::THIS_FRAME_ONLY;

	RenderResources::buffersManager().remap();
	RenderResources::renderCommandPool().reset();
}

}
