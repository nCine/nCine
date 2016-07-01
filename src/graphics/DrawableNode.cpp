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
	renderQueue.addCommand(renderCommand_);
}

/// Returns the node rendering layer
unsigned int DrawableNode::layer() const
{
	return renderCommand_->layer();
}

/// Sets the node rendering layer
/*! The maximum admissible value is 65535 */
void DrawableNode::setLayer(unsigned int layer)
{
	renderCommand_->setLayer(layer);
}

}
