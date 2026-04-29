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

bool Device::createCommandBufferImpl(CommandBuffer &commandBuffer, const char *debugName)
{
	BackendData &bck = *backendData_;
	BackendData::FrameContext &frameContext = bck.frameContexts_[bck.currentFrameIndex];

	commandBuffer.backendData_ = &bck.obtainCommandBufferData(frameContext);
	CommandBuffer::BackendData &cmdBck = *commandBuffer.backendData_;
	cmdBck.deviceFrontend = frontendData_.get();

	if (bck.caps_.debugUtils)
		setObjectName(bck.device_, cmdBck.commandBuffer, debugName);

	return true;
}

/*! \return A submission ID that can be waited on or checked for completion */
uint64_t Device::submitCommandBuffer(CommandBuffer &commandBuffer, const SubmitDesc &desc)
{
	BackendData &bck = *backendData_;
	BackendData::FrameContext &frameContext = bck.frameContexts_[bck.currentFrameIndex];

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	// --- Wait semaphores ---
	VkSemaphore waitSemaphores[2];
	VkPipelineStageFlags waitStages[2];
	uint32_t waitCount = 0;
	uint64_t waitValues[2];

	// Acquire (binary)
	if (desc.waitAcquire && frameContext.swapchainHasAcquired)
	{
		waitSemaphores[waitCount] = frameContext.acquireSemaphore;
		waitStages[waitCount] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		waitValues[waitCount] = 0;
		waitCount++;
	}

	// Timeline wait
	if (desc.waitValue != 0)
	{
		waitSemaphores[waitCount] = bck.graphicsQueue_.timelineSemaphore;
		waitStages[waitCount] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		waitValues[waitCount] = desc.waitValue;
		waitCount++;
	}

	submitInfo.waitSemaphoreCount = waitCount;
	submitInfo.pWaitSemaphores = (waitCount > 0) ? waitSemaphores : nullptr;
	submitInfo.pWaitDstStageMask = (waitCount > 0) ? waitStages : nullptr;

	// --- Command buffer ---
	VkCommandBuffer cmd = commandBuffer.backendData_->commandBuffer;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmd;

	// --- Signal semaphores ---
	VkSemaphore signalSemaphores[2];
	uint64_t signalValues[2];
	uint32_t signalCount = 0;

	const uint64_t signalValue = ++bck.graphicsQueue_.nextValue;
	// Always signal timeline semaphore
	signalSemaphores[signalCount] = bck.graphicsQueue_.timelineSemaphore;
	signalValues[signalCount] = signalValue;
	signalCount++;

	// Signal present semaphore if requested
	if (desc.signalPresent && frameContext.swapchainHasAcquired)
	{
		Swapchain::BackendData &swpBck = *swapchain_.backendData_;
		VkSemaphore presentSemaphore = swpBck.presentSemaphores[swpBck.currentImageIndex_];

		signalSemaphores[signalCount] = presentSemaphore;
		signalValues[signalCount] = 0; // ignored but needed
		signalCount++;
	}

	submitInfo.signalSemaphoreCount = signalCount;
	submitInfo.pSignalSemaphores = signalSemaphores;

	// --- Timeline semaphore info ---
	VkTimelineSemaphoreSubmitInfo timelineInfo{};
	timelineInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;

	// Signal
	timelineInfo.signalSemaphoreValueCount = signalCount;
	timelineInfo.pSignalSemaphoreValues = signalValues;

	// Wait
	timelineInfo.waitSemaphoreValueCount = waitCount;
	timelineInfo.pWaitSemaphoreValues = (waitCount > 0) ? waitValues : nullptr;

	submitInfo.pNext = &timelineInfo;

	// --- Fence fallback ---
	VkFence fence = VK_NULL_HANDLE;
	if (bck.caps_.timelineSemaphores == false)
		fence = frameContext.frameFence;

	const VkResult result = vkQueueSubmit(bck.graphicsQueue_.queue, 1, &submitInfo, fence);
	vlkFatalAssert(result);

	frameContext.lastSubmissionId = signalValue;
	return signalValue;
}

/*! \return A submission ID that can be waited on or checked for completion */
uint64_t Device::submitCommandBuffer(CommandBuffer &commandBuffer, uint64_t waitValue)
{
	return submitCommandBuffer(commandBuffer, SubmitDesc{ false, false, waitValue });
}

/*! \return A submission ID that can be waited on or checked for completion */
uint64_t Device::submitCommandBuffer(CommandBuffer &commandBuffer)
{
	return submitCommandBuffer(commandBuffer, SubmitDesc{});
}

bool Device::isSubmissionComplete(uint64_t id)
{
	BackendData &bck = *backendData_;
	const uint64_t value = bck.getTimelineSemaphoreValue(bck.graphicsQueue_.timelineSemaphore);
	return (value >= id);
}

void Device::waitSubmission(uint64_t id)
{
	BackendData &bck = *backendData_;
	bck.waitTimelineSemaphore(bck.graphicsQueue_.timelineSemaphore, id);
}

} // namespace grail
} // namespace ncine
