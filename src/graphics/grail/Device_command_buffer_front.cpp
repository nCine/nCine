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
	createCommandBufferImpl(commandBuffer);

	return commandBuffer;
}

} // namespace grail
} // namespace ncine
