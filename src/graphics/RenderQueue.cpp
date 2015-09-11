#include "algorithms.h"
#include "RenderQueue.h"
#include "SceneNode.h"
#include "Array.h"
#include "Sprite.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderQueue::RenderQueue()
	: numVertices_(0), lastNumVertices_(0), lastNumCommands_(0),
	  opaqueRenderCommands_(16), transparentRenderCommands_(16)
{
	for (int i = 0; i < RenderCommand::TYPE_COUNT; i++)
	{
		typedLastNumVertices_[i] = typedNumVertices_[i];
		typedLastNumCommands_[i] = typedNumCommands_[i];
		typedNumVertices_[i] = 0;
		typedNumCommands_[i] = 0;
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Adds a draw command to the queue
void RenderQueue::addCommand(RenderCommand *command)
{
	// Calculating a sorting key before adding the command to the queue
	command->calculateSortKey();

	typedNumVertices_[command->type()] += command->geometry().numVertices();
	typedNumCommands_[command->type()]++;

	numVertices_ += command->geometry().numVertices();

	if (command->material().isTransparent() == false)
	{
		opaqueRenderCommands_.pushBack(command);
	}
	else
	{
		transparentRenderCommands_.pushBack(command);
	}
}

/// Sorts the queue then issues every render command in order
void RenderQueue::draw()
{
	sortQueues();

	// Rendering opaque nodes front to back
	for (int i = opaqueRenderCommands_.size() - 1; i > -1; i--)
	{
		opaqueRenderCommands_[i]->issue();
	}

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	// Rendering transparent nodes back to front
	for (unsigned int i = 0; i < transparentRenderCommands_.size(); i++)
	{
		transparentRenderCommands_[i]->issue();
	}
	// Has to be enabled again before exiting this method
	// or glClear(GL_DEPTH_BUFFER_BIT) won't have any effect
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	lastNumVertices_ = numVertices_;
	lastNumCommands_ = opaqueRenderCommands_.size() + transparentRenderCommands_.size();
	numVertices_ = 0;

	for (int i = 0; i < RenderCommand::TYPE_COUNT; i++)
	{
		typedLastNumVertices_[i] = typedNumVertices_[i];
		typedLastNumCommands_[i] = typedNumCommands_[i];
		typedNumVertices_[i] = 0;
		typedNumCommands_[i] = 0;
	}

	opaqueRenderCommands_.clear();
	transparentRenderCommands_.clear();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Sorts render nodes in both queues to minimize state changes
void RenderQueue::sortQueues()
{
	nc::quicksortDesc(opaqueRenderCommands_.begin(), opaqueRenderCommands_.end());
	nc::quicksortDesc(transparentRenderCommands_.begin(), transparentRenderCommands_.end());

	// Check sorting correctness
/*
	if (nc::isSorted(opaqueRenderCommands_.begin(), opaqueRenderCommands_.end(), nc::IsGreater<RenderCommand *>) == false)
	{
		printf("Opaque not sorted1\n");
	}

	if (nc::isSorted(transparentRenderCommands_.begin(), transparentRenderCommands_.end(), nc::IsGreater<RenderCommand *>) == false)
	{
		printf("Transparent not sorted1\n");
	}
*/
}

}
