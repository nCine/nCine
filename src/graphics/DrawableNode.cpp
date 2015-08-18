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

/// Increases or decreases the node rendering priority by adding a certain amount
int DrawableNode::addPriority(int amount)
{
	int priority = renderCommand_->priority() + amount;
	renderCommand_->setPriority(priority);

	return priority;
}

}
