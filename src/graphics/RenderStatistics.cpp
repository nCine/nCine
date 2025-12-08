#include "RenderStatistics.h"
#ifdef WITH_SCENEGRAPH
	#include "tracy.h"
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

nctl::String RenderStatistics::debugString_(256);
#ifdef WITH_SCENEGRAPH
RenderStatistics::Commands RenderStatistics::allCommands_;
RenderStatistics::Commands RenderStatistics::typedCommands_[RenderCommand::CommandTypes::COUNT];
RenderStatistics::CommandPool RenderStatistics::commandPool_;
RenderStatistics::Textures RenderStatistics::textures_;
unsigned int RenderStatistics::index_ = 0;
unsigned int RenderStatistics::culledNodes_[2] = { 0, 0 };
#endif
RenderStatistics::Buffers RenderStatistics::typedBuffers_[RenderBuffersManager::BufferTypes::COUNT];
RenderStatistics::CustomBuffers RenderStatistics::customVbos_;
RenderStatistics::CustomBuffers RenderStatistics::customIbos_;
RenderStatistics::VaoPool RenderStatistics::vaoPool_;

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void RenderStatistics::reset()
{
#ifdef WITH_SCENEGRAPH
	// Ping pong index for last and current frame
	index_ = (index_ + 1) % 2;
	culledNodes_[index_] = 0;

	TracyPlot("Vertices", static_cast<int64_t>(allCommands_.vertices));
	TracyPlot("Render Commands", static_cast<int64_t>(allCommands_.commands));

	for (unsigned int i = 0; i < RenderCommand::CommandTypes::COUNT; i++)
		typedCommands_[i].reset();
	allCommands_.reset();

	commandPool_.reset();
#endif

	for (unsigned int i = 0; i < RenderBuffersManager::BufferTypes::COUNT; i++)
		typedBuffers_[i].reset();

	vaoPool_.reset();
}

#ifdef WITH_SCENEGRAPH
void RenderStatistics::gatherStatistics(const RenderCommand &command)
{
	const GLsizei numVertices = command.geometry().numVertices();
	const unsigned int numIndices = command.geometry().numIndices();

	if (numVertices == 0 && numIndices == 0)
		return;

	unsigned int verticesToCount = 0;
	if (numIndices > 0)
		verticesToCount = (command.numInstances() > 0) ? numIndices * command.numInstances() : numIndices;
	else
		verticesToCount = (command.numInstances() > 0) ? numVertices * command.numInstances() : numVertices;

	const unsigned int typeIndex = command.type();
	typedCommands_[typeIndex].vertices += verticesToCount;
	typedCommands_[typeIndex].commands++;
	typedCommands_[typeIndex].transparents += (command.material().isBlendingEnabled()) ? 1 : 0;
	typedCommands_[typeIndex].instances += command.numInstances();
	typedCommands_[typeIndex].batchSize += command.batchSize();

	allCommands_.vertices += verticesToCount;
	allCommands_.commands++;
	allCommands_.transparents += (command.material().isBlendingEnabled()) ? 1 : 0;
	allCommands_.instances += command.numInstances();
	allCommands_.batchSize += command.batchSize();
}
#endif

void RenderStatistics::gatherStatistics(const RenderBuffersManager::ManagedBuffer &buffer)
{
	const unsigned int typeIndex = buffer.type;
	typedBuffers_[typeIndex].count++;
	typedBuffers_[typeIndex].size += buffer.size;
	typedBuffers_[typeIndex].usedSpace += buffer.size - buffer.freeSpace;
}

}
