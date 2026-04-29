#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include "grail/Device.h"
#include "grail/private/Device_frontend.h"
#include "grail/vulkan/Device_backend.h"
#include "grail/vulkan/vlk_utils.h"

namespace ncine {
namespace grail {

namespace {

VkBufferUsageFlags grlBufferUsageToVk(Buffer::Usage usage)
{
	VkBufferUsageFlags flags = 0;

	if (hasFlag(usage, Buffer::Usage::VERTEX))
		flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	if (hasFlag(usage, Buffer::Usage::INDEX))
		flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

	if (hasFlag(usage, Buffer::Usage::UNIFORM))
		flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	if (hasFlag(usage, Buffer::Usage::STORAGE))
		flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

	if (hasFlag(usage, Buffer::Usage::TRANSFER_SRC))
		flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	if (hasFlag(usage, Buffer::Usage::TRANSFER_DST))
		flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	return flags;
}

VmaMemoryUsage grlMemoryTypeToVma(Buffer::MemoryType type)
{
	switch (type)
	{
		case Buffer::MemoryType::DEFAULT: return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		case Buffer::MemoryType::UPLOAD: return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		case Buffer::MemoryType::READBACK: return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
	}

	return VMA_MEMORY_USAGE_AUTO;
}

}

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

bool Device::createBufferImpl(uint32_t index, const Buffer::Desc &desc)
{
	BackendData &bck = *backendData_;
	BackendData::BufferData &data = bck.bufferData_[index];
	ASSERT(data.buffer == VK_NULL_HANDLE && data.allocation == VK_NULL_HANDLE);

	VkBufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = desc.size;
	createInfo.usage = grlBufferUsageToVk(desc.usage);
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	const bool hostVisibleMemory = (desc.memoryType == Buffer::MemoryType::UPLOAD ||
	                                desc.memoryType == Buffer::MemoryType::READBACK);

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = grlMemoryTypeToVma(desc.memoryType);
	if (hostVisibleMemory)
		allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

	const VkResult result = vmaCreateBuffer(bck.vmaAllocator_, &createInfo, &allocInfo, &data.buffer, &data.allocation, &data.allocationInfo);
	vlkFatalAssert(result);

	if (bck.caps_.debugUtils && desc.debugName)
		setObjectName(bck.device_, data.buffer, desc.debugName);

	return (result == VK_SUCCESS);
}

void Device::destroyBufferImpl(uint32_t index)
{
	BackendData &bck = *backendData_;
	BackendData::BufferData &data = bck.bufferData_[index];

	ASSERT(data.buffer != VK_NULL_HANDLE && data.allocation != VK_NULL_HANDLE);
	vmaDestroyBuffer(bck.vmaAllocator_, data.buffer, data.allocation);
	data.buffer = VK_NULL_HANDLE;
	data.allocation = VK_NULL_HANDLE;
}

void *Device::mapBuffer(Buffer::Handle handle)
{
	const FrontendData &frn = *frontendData_;
	ASSERT(frn.isAlive(handle) == true);

	BackendData &bck = *backendData_;
	BackendData::BufferData &data = bck.bufferData_[handle.index()];

	VkMemoryPropertyFlags props;
	vmaGetAllocationMemoryProperties(bck.vmaAllocator_, data.allocation, &props);

	const bool hostVisible = ((props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0);
	ASSERT(hostVisible == true);
	if (hostVisible == false)
		return nullptr;

	// Persistent mapping
	if (data.allocationInfo.pMappedData)
		return data.allocationInfo.pMappedData;

	void *ptr = nullptr;
	const VkResult result = vmaMapMemory(bck.vmaAllocator_, data.allocation, &ptr);
	vlkFatalAssert(result);

	// Refresh allocation info so pMappedData becomes valid
	vmaGetAllocationInfo(bck.vmaAllocator_, data.allocation, &data.allocationInfo);

	return ptr;
}

void Device::unmapBuffer(Buffer::Handle handle)
{
	const FrontendData &frn = *frontendData_;
	ASSERT(frn.isAlive(handle) == true);

	BackendData &bck = *backendData_;
	BackendData::BufferData &data = bck.bufferData_[handle.index()];

	// Persistent mapping
	if (data.allocationInfo.pMappedData)
		return;

	vmaUnmapMemory(bck.vmaAllocator_, data.allocation);

	// Refresh allocation info so pMappedData becomes valid
	vmaGetAllocationInfo(bck.vmaAllocator_, data.allocation, &data.allocationInfo);
}

Buffer::DynamicAlloc Device::allocateDynamicBuffer(uint32_t size)
{
	ASSERT(size > 0);
	if (size == 0)
		return Buffer::DynamicAlloc{};

	BackendData &bck = *backendData_;
	BackendData::FrameContext &frameContext = bck.frameContexts_[bck.currentFrameIndex];

	// There is one buffer only, but it's partitioned among frames
	const uint32_t frameStride = ExtraLimits::DynamicUploadBufferSize;
	const uint32_t base = frameStride * bck.currentFrameIndex;

	const uint32_t alignment = bck.physicalDeviceProperties_.limits.minUniformBufferOffsetAlignment;
	uint32_t currentOffset = frameContext.dynamicUploadBufferOffset.load(nctl::MemoryModel::RELAXED);

	while (true)
	{
		const uint32_t alignedOffset = (currentOffset + alignment - 1) & ~(alignment - 1);
		const uint32_t absAlignedOffset = base + alignedOffset;

		if (alignedOffset >= frameStride || size > frameStride - alignedOffset)
			return Buffer::DynamicAlloc{};

		const uint32_t nextOffset = alignedOffset + size;
		if (frameContext.dynamicUploadBufferOffset.cmpExchange(currentOffset, nextOffset, nctl::MemoryModel::ACQ_REL))
		{
			Buffer::DynamicAlloc alloc;
			alloc.handle = bck.dynamicUploadBuffer_;
			alloc.offset = absAlignedOffset;
			alloc.mapPtr = bck.dynamicUploadBufferMapBase_ + absAlignedOffset;
			return alloc;
		}
	}
}

} // namespace grail
} // namespace ncine
