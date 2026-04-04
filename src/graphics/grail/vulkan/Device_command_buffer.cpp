#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include "grail/Device.h"
#include "grail/vulkan/Device_backend.h"
#include "grail/vulkan/vlk_utils.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

bool Device::createCommandBufferImpl(CommandBuffer &commandBuffer)
{
	BackendData &bck = *backendData_;
	BackendData::FrameContext &frameContext = bck.frameContexts_[bck.currentFrameIndex];

	commandBuffer.backendData_ = &bck.obtainCommandBufferData(frameContext);
	commandBuffer.backendData_->deviceFrontend = frontendData_.get();

	return true;
}

void Device::submitCommandBuffer(CommandBuffer &commandBuffer)
{
	BackendData &bck = *backendData_;
	BackendData::FrameContext &frameContext = bck.frameContexts_[bck.currentFrameIndex];

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { frameContext.acquireSemaphore };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.pWaitDstStageMask = waitStages;

	if (frameContext.swapchainHasAcquired == false)
		submitInfo.waitSemaphoreCount = 0;

	VkCommandBuffer commandBuffers[] = { commandBuffer.backendData_->commandBuffer };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers;

	Swapchain::BackendData &swpBck = *swapchain_.backendData_;
	VkSemaphore presentSemaphore = swpBck.presentSemaphores[swpBck.currentImageIndex_];

	VkSemaphore signalSemaphores[] = { presentSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (frameContext.swapchainHasAcquired == false)
		submitInfo.signalSemaphoreCount = 0;

	const VkResult result = vkQueueSubmit(bck.graphicsQueue_.queue, 1, &submitInfo, frameContext.frameFence);
	vlkFatalAssert(result);
}

} // namespace grail
} // namespace ncine
