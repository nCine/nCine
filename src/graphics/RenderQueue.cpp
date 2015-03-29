#ifdef WITH_DEPTH_TEST
	#define NCINE_INCLUDE_OPENGL
	#include "common_headers.h"
#endif

#include "common_functions.h"
#include "RenderQueue.h"
#include "SceneNode.h"
#include "Array.h"
#include "Sprite.h"

//#include <assert.h> // for checking sorting correctness

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Adds a draw command to the queue
void RenderQueue::addCommand(const RenderCommand *command)
{
	typedNumVertices_[command->type()] += command->geometry().numVertices();
	typedNumCommands_[command->type()]++;

	numVertices_ += command->geometry().numVertices();

#ifdef WITH_DEPTH_TEST
	if (command->material().isTransparent() == false)
	{
		opaqueRenderCommands_.insertBack(command);
	}
	else
#endif
		transparentRenderCommands_.insertBack(command);
}

/// Sorts the queue then issues every render command in order
void RenderQueue::draw()
{
	sortQueues();

#ifdef WITH_DEPTH_TEST
	glDisable(GL_BLEND);
	// TODO: Investigate about the heavy performance drop with alpha testing
	glEnable(GL_ALPHA_TEST);
	// Rendering opaque nodes front to back
	for (int i = opaqueRenderCommands_.size() - 1; i > -1; i--)
	{
		opaqueRenderCommands_[i]->issue();
	}

	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
#endif
	// Rendering transparent nodes back to front
	for (unsigned int i = 0; i < transparentRenderCommands_.size(); i++)
	{
		transparentRenderCommands_[i]->issue();
	}
#ifdef WITH_DEPTH_TEST
	// Has to be enabled again before exiting this method
	// or glClear(GL_DEPTH_BUFFER_BIT) won't have any effect
	glDepthMask(GL_TRUE);
#endif

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
	nc::qSort(opaqueRenderCommands_, 0, opaqueRenderCommands_.size() - 1);
	nc::qSort(transparentRenderCommands_, 0, transparentRenderCommands_.size() - 1);

	// Check sorting correctness
//	for (unsigned int i = 1; i < opaqueRenderCommands_.size(); i++)
//		assert(opaqueRenderCommands_[i-1]->sortKey() <= opaqueRenderCommands_[i]->sortKey());
//	for (unsigned int i = 1; i < transparentRenderCommands_.size(); i++)
//		assert(transparentRenderCommands_[i-1]->sortKey() <= transparentRenderCommands_[i]->sortKey());
}

}
