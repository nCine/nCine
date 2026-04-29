#include "grail/Device.h"
#include "grail/private/Device_frontend.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

CommandBuffer Device::createCommandBuffer()
{
	CommandBuffer commandBuffer;
	createCommandBufferImpl(commandBuffer, nullptr);

	return commandBuffer;
}

CommandBuffer Device::createCommandBuffer(const char *debugName)
{
	CommandBuffer commandBuffer;
	createCommandBufferImpl(commandBuffer, debugName);

	return commandBuffer;
}

} // namespace grail
} // namespace ncine
