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

VkFence Device::BackendData::createFence(bool signaled)
{
	VkFence fence = VK_NULL_HANDLE;

	VkFenceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	if (signaled)
		createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	const VkResult result = vkCreateFence(device_, &createInfo, allocator_, &fence);
	vlkFatalAssert(result);

	return fence;
}

void Device::BackendData::destroyFence(VkFence fence)
{
	ASSERT(fence != VK_NULL_HANDLE);
	vkDestroyFence(device_, fence, allocator_);
}

void Device::BackendData::waitFence(VkFence fence)
{
	ASSERT(fence != VK_NULL_HANDLE);
	vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX);
}

void Device::BackendData::resetFence(VkFence fence)
{
	ASSERT(fence != VK_NULL_HANDLE);
	vkResetFences(device_, 1, &fence);
}

VkSemaphore Device::BackendData::createSemaphore()
{
	VkSemaphore semaphore = VK_NULL_HANDLE;

	VkSemaphoreCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	const VkResult result = vkCreateSemaphore(device_, &createInfo, allocator_, &semaphore);
	vlkFatalAssert(result);

	return semaphore;
}

void Device::BackendData::destroySemaphore(VkSemaphore semaphore)
{
	ASSERT(semaphore != VK_NULL_HANDLE);
	vkDestroySemaphore(device_, semaphore, allocator_);
}

} // namespace grail
} // namespace ncine
