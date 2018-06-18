#include "nctl/algorithms.h"
#include "RenderQueue.h"
#include "RenderResources.h"
#include "RenderStatistics.h"
#include "GLDebug.h"
#include "Application.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderQueue::RenderQueue()
	: debugGroupString_(64),
	  opaqueQueue_(16), opaqueBatchedQueue_(16), transparentQueue_(16), transparentBatchedQueue_(16)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void RenderQueue::addCommand(RenderCommand *command)
{
	// Calculating a sorting key before adding the command to the queue
	command->calculateSortKey();

	if (command->material().isTransparent() == false)
		opaqueQueue_.pushBack(command);
	else
		transparentQueue_.pushBack(command);
}

namespace {

bool descendingOrder(const RenderCommand *a, const RenderCommand *b) { return a->sortKey() > b->sortKey(); }
bool ascendingOrder(const RenderCommand *a, const RenderCommand *b) { return a->sortKey() < b->sortKey(); }

const char *commandTypeString(const RenderCommand &command)
{
	switch (command.type())
	{
		case RenderCommand::CommandTypes::GENERIC: return "generic";
		case RenderCommand::CommandTypes::PLOTTER: return "plotter";
		case RenderCommand::CommandTypes::SPRITE: return "sprite";
		case RenderCommand::CommandTypes::MESH_SPRITE: return "mesh sprite";
		case RenderCommand::CommandTypes::PARTICLE: return "particle";
		case RenderCommand::CommandTypes::TEXT: return "text";
		default: return "unknown";
	}
}

}

void RenderQueue::draw()
{
	const bool batchingEnabled = theApplication().renderingSettings().batchingEnabled;

	// Reset all rendering statistics
	ncine::RenderStatistics::reset();

	// Sorting the queues with the relevant orders
	nctl::quicksort(opaqueQueue_.begin(), opaqueQueue_.end(), descendingOrder);
	nctl::quicksort(transparentQueue_.begin(), transparentQueue_.end(), ascendingOrder);

	nctl::Array<RenderCommand *> *opaques = &opaqueQueue_;
	nctl::Array<RenderCommand *> *transparents = &transparentQueue_;

	if (batchingEnabled)
	{
		// Always create batches after sorting
		batcher_.createBatches(opaqueQueue_, opaqueBatchedQueue_);
		opaques = &opaqueBatchedQueue_;

		batcher_.createBatches(transparentQueue_, transparentBatchedQueue_);
		transparents = &transparentBatchedQueue_;
	}

	// Avoid GPU stalls by uploading to VBOs, IBOs and UBOs before drawing
	if (opaques->isEmpty() == false)
	{
		GLDebug::ScopedGroup scoped("Committing vertices, indices and uniform blocks in opaques");
		for (RenderCommand *opaqueRenderCommand : *opaques)
		{
			opaqueRenderCommand->commitVertices();
			opaqueRenderCommand->commitIndices();
			opaqueRenderCommand->commitUniformBlocks();
		}
	}

	if (transparents->isEmpty() == false)
	{
		GLDebug::ScopedGroup scoped("Committing vertices, indices and uniform blocks in transparents");
		for (RenderCommand *transparentRenderCommand : *transparents)
		{
			transparentRenderCommand->commitVertices();
			transparentRenderCommand->commitIndices();
			transparentRenderCommand->commitUniformBlocks();
		}
	}

	// Now that UBOs and VBOs have been updated, they can be flushed and unmapped
	RenderResources::buffersManager().flushUnmap();

	unsigned int commandIndex = 0;
	// Rendering opaque nodes front to back
	for (RenderCommand *opaqueRenderCommand : *opaques)
	{
		const int numInstances = opaqueRenderCommand->numInstances();
		const int batchSize = opaqueRenderCommand->batchSize();
		if (numInstances > 0)
			debugGroupString_.format("Opaque %u (%d %s on layer %u)",
				commandIndex, numInstances, commandTypeString(*opaqueRenderCommand), opaqueRenderCommand->layer());
		else if (batchSize > 0)
			debugGroupString_.format("Opaque %u (%d %s on layer %u)",
				commandIndex, batchSize, commandTypeString(*opaqueRenderCommand), opaqueRenderCommand->layer());
		else
			debugGroupString_.format("Opaque %u (%s on layer %u)",
				commandIndex, commandTypeString(*opaqueRenderCommand), opaqueRenderCommand->layer());

		GLDebug::ScopedGroup scoped(debugGroupString_.data());
		commandIndex++;

		RenderStatistics::gatherStatistics(*opaqueRenderCommand);
		opaqueRenderCommand->issue();
	}

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	// Rendering transparent nodes back to front
	for (RenderCommand *transparentRenderCommand : *transparents)
	{
		const int numInstances = transparentRenderCommand->numInstances();
		const int batchSize = transparentRenderCommand->batchSize();
		if (numInstances > 0)
			debugGroupString_.format("Transparent %u (%d %s on layer %u)",
				commandIndex, numInstances, commandTypeString(*transparentRenderCommand), transparentRenderCommand->layer());
		else if (batchSize > 0)
			debugGroupString_.format("Transparent %u (%d %s on layer %u)",
				commandIndex, batchSize, commandTypeString(*transparentRenderCommand), transparentRenderCommand->layer());
		else
			debugGroupString_.format("Transparent %u (%s on layer %u)",
				commandIndex, commandTypeString(*transparentRenderCommand), transparentRenderCommand->layer());

		GLDebug::ScopedGroup scoped(debugGroupString_.data());
		commandIndex++;

		RenderStatistics::gatherStatistics(*transparentRenderCommand);
		transparentRenderCommand->issue();
	}
	// Has to be enabled again before exiting this method
	// or glClear(GL_DEPTH_BUFFER_BIT) won't have any effect
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	opaqueQueue_.clear();
	opaqueBatchedQueue_.clear();
	transparentQueue_.clear();
	transparentBatchedQueue_.clear();

	RenderResources::buffersManager().remap();
	batcher_.reset();
	GLDebug::reset();
}

}
