#include <nctl/algorithms.h>
#include <nctl/StaticString.h>
#include "RenderQueue.h"
#include "RenderBatcher.h"
#include "RenderResources.h"
#include "RenderStatistics.h"
#include "GLDebug.h"
#include "Application.h"
#include "GLScissorTest.h"
#include "GLDepthTest.h"
#include "GLBlending.h"
#include "tracy_opengl.h"

namespace ncine {

namespace {
	/// The string used to output OpenGL debug group information
	static nctl::StaticString<64> debugString;
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderQueue::RenderQueue()
    : opaqueQueue_(16), opaqueBatchedQueue_(16),
      transparentQueue_(16), transparentBatchedQueue_(16)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool RenderQueue::isEmpty() const
{
	return (opaqueQueue_.isEmpty() && transparentQueue_.isEmpty());
}

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

void RenderQueue::sortAndCommit()
{
	const bool batchingEnabled = theApplication().renderingSettings().batchingEnabled;

	// Sorting the queues with the relevant orders
	nctl::quicksort(opaqueQueue_.begin(), opaqueQueue_.end(), descendingOrder);
	nctl::quicksort(transparentQueue_.begin(), transparentQueue_.end(), ascendingOrder);

	nctl::Array<RenderCommand *> *opaques = batchingEnabled ? &opaqueBatchedQueue_ : &opaqueQueue_;
	nctl::Array<RenderCommand *> *transparents = batchingEnabled ? &transparentBatchedQueue_ : &transparentQueue_;

	if (batchingEnabled)
	{
		ZoneScopedN("Batching");
		// Always create batches after sorting
		RenderResources::renderBatcher().createBatches(opaqueQueue_, opaqueBatchedQueue_);
		RenderResources::renderBatcher().createBatches(transparentQueue_, transparentBatchedQueue_);
	}

	// Avoid GPU stalls by uploading to VBOs, IBOs and UBOs before drawing
	if (opaques->isEmpty() == false)
	{
		ZoneScopedN("Commit opaques");
		debugString.format("Commit %u opaque command(s) for viewport 0x%lx", opaques->size(), uintptr_t(RenderResources::currentViewport()));
		GLDebug::ScopedGroup scoped(debugString.data());
		for (RenderCommand *opaqueRenderCommand : *opaques)
			opaqueRenderCommand->commitAll();
	}

	if (transparents->isEmpty() == false)
	{
		ZoneScopedN("Commit transparents");
		debugString.format("Commit %u transparent command(s) for viewport 0x%lx", transparents->size(), uintptr_t(RenderResources::currentViewport()));
		GLDebug::ScopedGroup scoped(debugString.data());
		for (RenderCommand *transparentRenderCommand : *transparents)
			transparentRenderCommand->commitAll();
	}
}

void RenderQueue::draw()
{
	const bool batchingEnabled = theApplication().renderingSettings().batchingEnabled;
	nctl::Array<RenderCommand *> *opaques = batchingEnabled ? &opaqueBatchedQueue_ : &opaqueQueue_;
	nctl::Array<RenderCommand *> *transparents = batchingEnabled ? &transparentBatchedQueue_ : &transparentQueue_;

	unsigned int commandIndex = 0;
	// Rendering opaque nodes front to back
	for (RenderCommand *opaqueRenderCommand : *opaques)
	{
		TracyGpuZone("Opaque");
		const int numInstances = opaqueRenderCommand->numInstances();
		const int batchSize = opaqueRenderCommand->batchSize();
		const uint16_t layer = opaqueRenderCommand->layer();
		const uint16_t visitOrder = opaqueRenderCommand->visitOrder();

		if (numInstances > 0)
			debugString.format("Opaque %u (%d %s on layer %u, visit order %u)",
			                   commandIndex, numInstances, commandTypeString(*opaqueRenderCommand), layer, visitOrder);
		else if (batchSize > 0)
			debugString.format("Opaque %u (%d %s on layer %u, visit order %u)",
			                   commandIndex, batchSize, commandTypeString(*opaqueRenderCommand), layer, visitOrder);
		else
			debugString.format("Opaque %u (%s on layer %u, visit order %u)",
			                   commandIndex, commandTypeString(*opaqueRenderCommand), layer, visitOrder);

		GLDebug::ScopedGroup scoped(debugString.data());
		commandIndex++;

		RenderStatistics::gatherStatistics(*opaqueRenderCommand);
		opaqueRenderCommand->commitCameraTransformation();
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
		const uint16_t layer = transparentRenderCommand->layer();
		const uint16_t visitOrder = transparentRenderCommand->visitOrder();

		if (numInstances > 0)
			debugString.format("Transparent %u (%d %s on layer %u, visit order %u)",
			                   commandIndex, numInstances, commandTypeString(*transparentRenderCommand), layer, visitOrder);
		else if (batchSize > 0)
			debugString.format("Transparent %u (%d %s on layer %u, visit order %u)",
			                   commandIndex, batchSize, commandTypeString(*transparentRenderCommand), layer, visitOrder);
		else
			debugString.format("Transparent %u (%s on layer %u, visit order %u)",
			                   commandIndex, commandTypeString(*transparentRenderCommand), layer, visitOrder);

		GLDebug::ScopedGroup scoped(debugString.data());
		commandIndex++;

		RenderStatistics::gatherStatistics(*transparentRenderCommand);
		GLBlending::setBlendFunc(transparentRenderCommand->material().srcBlendingFactor(), transparentRenderCommand->material().destBlendingFactor());
		transparentRenderCommand->commitCameraTransformation();
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

	RenderResources::renderBatcher().reset();
}

}
