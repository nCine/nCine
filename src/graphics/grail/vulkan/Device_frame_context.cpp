#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include "grail/Device.h"
#include "grail/vulkan/vlk_utils.h"
#include "grail/vulkan/Device_backend.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// Device::BackendData
///////////////////////////////////////////////////////////

VkCommandPool Device::BackendData::createCommandPool(uint32_t queueFamilyIndex)
{
	VkCommandPool commandPool = VK_NULL_HANDLE;

	VkCommandPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	createInfo.queueFamilyIndex = queueFamilyIndex;

	const VkResult result = vkCreateCommandPool(device_, &createInfo, allocator_, &commandPool);
	vlkFatalAssert(result);

	return commandPool;
}

bool Device::BackendData::resetCommandPool(VkCommandPool commandPool)
{
	const VkResult result = vkResetCommandPool(device_, commandPool, 0);
	vlkAssert(result);

	return (result == VK_SUCCESS);
}

void Device::BackendData::destroyCommandPool(VkCommandPool commandPool)
{
	ASSERT(commandPool != VK_NULL_HANDLE);
	vkDestroyCommandPool(device_, commandPool, allocator_);
}

VkCommandBuffer Device::BackendData::allocateCommandBuffers(VkCommandPool commandPool, VkCommandBuffer *commandBuffers, uint32_t count)
{
	ASSERT(commandPool != VK_NULL_HANDLE);
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = count;

	const VkResult result = vkAllocateCommandBuffers(device_, &allocInfo, commandBuffers);
	vlkFatalAssert(result);

	return commandBuffer;
}

///////////////////////////////////////////////////////////
// Device::BackendData
///////////////////////////////////////////////////////////

bool Device::BackendData::createFrameContext(FrameContext &frameContext)
{
	ASSERT(frameContext.frameFence == VK_NULL_HANDLE);
	frameContext.frameFence = createFence(true); // Note: It should start as signalled

	ASSERT(frameContext.acquireSemaphore == VK_NULL_HANDLE);
	frameContext.acquireSemaphore = createSemaphore();

	ASSERT(frameContext.graphicsCmdPool == VK_NULL_HANDLE);
	frameContext.graphicsCmdPool = createCommandPool(graphicsQueue_.queueIndex);

	ASSERT(frameContext.freeCommandBuffers.isEmpty());
	ASSERT(frameContext.usedCommandBuffers.isEmpty());

	allocateFreeCommandBuffers(frameContext);

	ASSERT(dynamicUploadBuffer_.isValid() == true);
	ASSERT(dynamicUploadBufferMapBase_ != nullptr);

	return true;
}

void Device::BackendData::destroyFrameContext(FrameContext &frameContext)
{
	frameContext.usedCommandBuffers.clear();
	frameContext.freeCommandBuffers.clear();

	destroyCommandPool(frameContext.graphicsCmdPool);
	frameContext.graphicsCmdPool = VK_NULL_HANDLE;

	destroySemaphore(frameContext.acquireSemaphore);
	frameContext.acquireSemaphore = VK_NULL_HANDLE;

	destroyFence(frameContext.frameFence);
	frameContext.frameFence = VK_NULL_HANDLE;
}

void Device::BackendData::beginFrameContext()
{
	FrameContext &currentFC = frameContexts_[currentFrameIndex];

	waitFence(currentFC.frameFence);
	resetFence(currentFC.frameFence);

	// Recycling the command buffer handles
	for (uint32_t i = 0; i < currentFC.usedCommandBuffers.size(); i++)
		currentFC.freeCommandBuffers.pushBack(currentFC.usedCommandBuffers[i]);
	currentFC.usedCommandBuffers.clear();
	resetCommandPool(currentFC.graphicsCmdPool);

	// Reset the allocation offset for this frame dynamic uniform buffer
	currentFC.dynamicUploadBufferOffset.store(0);
}

void Device::BackendData::endFrameContext()
{
	currentFrameIndex = (currentFrameIndex + 1) % NumFramesInFlight;
}

void Device::BackendData::allocateFreeCommandBuffers(FrameContext &frameContext)
{
	nctl::StaticArray<VkCommandBuffer, FrameContext::CommandBufferBatchSize> commandBufferHandles;
	for (uint32_t i = 0; i < commandBufferHandles.capacity(); i++)
		commandBufferHandles.pushBack(VK_NULL_HANDLE);

	allocateCommandBuffers(frameContext.graphicsCmdPool, commandBufferHandles.data(), commandBufferHandles.size());
	for (uint32_t i = 0; i < commandBufferHandles.size(); i++)
	{
		frameContext.freeCommandBuffers.emplaceBack();
		frameContext.freeCommandBuffers.back().commandBuffer = commandBufferHandles[i];
		frameContext.freeCommandBuffers.back().deviceBackend = this;
	}
}

CommandBuffer::BackendData &Device::BackendData::obtainCommandBufferData(FrameContext &frameContext)
{
	if (frameContext.freeCommandBuffers.isEmpty())
		allocateFreeCommandBuffers(frameContext);

	frameContext.usedCommandBuffers.pushBack(frameContext.freeCommandBuffers.back());
	frameContext.freeCommandBuffers.popBack();

	return frameContext.usedCommandBuffers.back();
}

} // namespace grail
} // namespace ncine
