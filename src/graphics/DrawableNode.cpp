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

void DrawableNode::draw(RenderQueue& renderQueue)
{
	updateRenderCommand();
	renderQueue.addCommand(renderCommand_);
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

}
