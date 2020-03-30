#include <nctl/algorithms.h>
#include "RenderQueue.h"
#include "RenderResources.h"
#include "RenderStatistics.h"
#include "GLDebug.h"
#include "Application.h"
#include "GLScissorTest.h"
#include "GLDepthTest.h"
#include "GLBlending.h"
#include "tracy_opengl.h"

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
	// Calculating the material sorting key before adding the command to the queue
	command->calculateMaterialSortKey();

	if (command->material().isBlendingEnabled() == false)
		opaqueQueue_.pushBack(command);
	else
		transparentQueue_.pushBack(command);
}

namespace {

	bool descendingOrder(const RenderCommand *a, const RenderCommand *b)
	{
		return (a->materialSortKey() != b->materialSortKey())
		           ? a->materialSortKey() > b->materialSortKey()
		           : a->idSortKey() > b->idSortKey();
	}

	bool ascendingOrder(const RenderCommand *a, const RenderCommand *b)
	{
		return (a->materialSortKey() != b->materialSortKey())
		           ? a->materialSortKey() < b->materialSortKey()
		           : a->idSortKey() < b->idSortKey();
	}

	const char *commandTypeString(const RenderCommand &command)
	{
		switch (command.type())
		{
			case RenderCommand::CommandTypes::UNSPECIFIED: return "unspecified";
			case RenderCommand::CommandTypes::SPRITE: return "sprite";
			case RenderCommand::CommandTypes::MESH_SPRITE: return "mesh sprite";
			case RenderCommand::CommandTypes::PARTICLE: return "particle";
			case RenderCommand::CommandTypes::TEXT: return "text";
#ifdef WITH_IMGUI
			case RenderCommand::CommandTypes::IMGUI: return "imgui";
#endif
#ifdef WITH_NUKLEAR
			case RenderCommand::CommandTypes::NUKLEAR: return "nuklear";
#endif
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
		ZoneScopedN("Batching");
		// Always create batches after sorting
		batcher_.createBatches(opaqueQueue_, opaqueBatchedQueue_);
		opaques = &opaqueBatchedQueue_;

		batcher_.createBatches(transparentQueue_, transparentBatchedQueue_);
		transparents = &transparentBatchedQueue_;
	}

	// Avoid GPU stalls by uploading to VBOs, IBOs and UBOs before drawing
	if (opaques->isEmpty() == false)
	{
		ZoneScopedN("Commit opaques");
		GLDebug::ScopedGroup scoped("Committing vertices, indices and uniform blocks in opaques");
		for (RenderCommand *opaqueRenderCommand : *opaques)
		{
			opaqueRenderCommand->commitVertices();
			opaqueRenderCommand->commitIndices();
			opaqueRenderCommand->commitTransformation();
			opaqueRenderCommand->commitUniformBlocks();
		}
	}

	if (transparents->isEmpty() == false)
	{
		ZoneScopedN("Commit transparents");
		GLDebug::ScopedGroup scoped("Committing vertices, indices and uniform blocks in transparents");
		for (RenderCommand *transparentRenderCommand : *transparents)
		{
			transparentRenderCommand->commitVertices();
			transparentRenderCommand->commitIndices();
			transparentRenderCommand->commitTransformation();
			transparentRenderCommand->commitUniformBlocks();
		}
	}

	// Now that UBOs and VBOs have been updated, they can be flushed and unmapped
	RenderResources::buffersManager().flushUnmap();

	unsigned int commandIndex = 0;
	// Rendering opaque nodes front to back
	for (RenderCommand *opaqueRenderCommand : *opaques)
	{
		TracyGpuZone("Opaque");
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

	GLBlending::enable();
	GLDepthTest::disableDepthMask();
	// Rendering transparent nodes back to front
	for (RenderCommand *transparentRenderCommand : *transparents)
	{
		TracyGpuZone("Transparent");
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
		GLBlending::blendFunc(transparentRenderCommand->material().srcBlendingFactor(), transparentRenderCommand->material().destBlendingFactor());
		transparentRenderCommand->issue();
	}
	// Depth mask has to be enabled again before exiting this method
	// or glClear(GL_DEPTH_BUFFER_BIT) won't have any effect
	GLDepthTest::enableDepthMask();
	GLBlending::disable();

	GLScissorTest::disable();

	opaqueQueue_.clear();
	opaqueBatchedQueue_.clear();
	transparentQueue_.clear();
	transparentBatchedQueue_.clear();

	RenderResources::clearDirtyProjectionFlag(batchingEnabled);
	RenderResources::buffersManager().remap();
	batcher_.reset();
	GLDebug::reset();
}

}
