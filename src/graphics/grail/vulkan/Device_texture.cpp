#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include "grail/Device.h"
#include "grail/private/Device_frontend.h"
#include "grail/vulkan/Device_backend.h"
#include "grail/vulkan/vlk_utils.h"

namespace ncine {
namespace grail {

namespace {

VkImageType grlTextureTypeToVk(Texture::Type type)
{
	switch (type)
	{
		case Texture::Type::TEXTURE_1D: return VK_IMAGE_TYPE_1D;
		case Texture::Type::TEXTURE_2D: return VK_IMAGE_TYPE_2D;
		case Texture::Type::TEXTURE_3D: return VK_IMAGE_TYPE_3D;

		default: return VK_IMAGE_TYPE_2D;
	}
}

VkImageUsageFlags grlTextureUsageToVk(Texture::Usage usage)
{
	VkImageUsageFlags flags = 0;

	if (hasFlag(usage, Texture::Usage::SAMPLED))
		flags |= VK_IMAGE_USAGE_SAMPLED_BIT;

	if (hasFlag(usage, Texture::Usage::STORAGE))
		flags |= VK_IMAGE_USAGE_STORAGE_BIT;

	if (hasFlag(usage, Texture::Usage::COLOR_TARGET))
		flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (hasFlag(usage, Texture::Usage::DEPTH_STENCIL))
		flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	if (hasFlag(usage, Texture::Usage::TRANSFER_SRC))
		flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	if (hasFlag(usage, Texture::Usage::TRANSFER_DST))
		flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	return flags;
}

VkSampleCountFlagBits grlSampleCountToVk(Texture::SampleCount samples)
{
	switch (samples)
	{
		case Texture::SampleCount::SAMPLE_1: return VK_SAMPLE_COUNT_1_BIT;
		case Texture::SampleCount::SAMPLE_2: return VK_SAMPLE_COUNT_2_BIT;
		case Texture::SampleCount::SAMPLE_4: return VK_SAMPLE_COUNT_4_BIT;
		case Texture::SampleCount::SAMPLE_8: return VK_SAMPLE_COUNT_8_BIT;

		default: return VK_SAMPLE_COUNT_1_BIT;
	}
}

}

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

bool Device::createTextureImpl(uint32_t index, const Texture::Desc &desc)
{
	BackendData &bck = *backendData_;
	BackendData::TextureData &data = bck.textureData_[index];
	ASSERT(data.image == VK_NULL_HANDLE && data.allocation == VK_NULL_HANDLE);

	// Asserting on description instead of patching creation information.
	// Description is immutable and used multiple times, it must reflect creation information.
	ASSERT(desc.depth > 0);
	ASSERT(desc.depth == 1 || desc.type == Texture::Type::TEXTURE_3D);
	ASSERT(desc.mipLevels > 0);
	ASSERT(desc.arrayLayers > 0);

	VkImageCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.imageType = grlTextureTypeToVk(desc.type);
	createInfo.extent.width = desc.width;
	createInfo.extent.height = desc.height;
	createInfo.extent.depth = desc.depth;
	createInfo.mipLevels = desc.mipLevels;
	createInfo.arrayLayers = desc.arrayLayers;
	createInfo.format = grlFormatToVk(desc.format);
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	createInfo.usage = grlTextureUsageToVk(desc.usage);
	createInfo.samples = grlSampleCountToVk(desc.sampleCount);
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	allocInfo.flags = 0;

	const VkResult result = vmaCreateImage(bck.vmaAllocator_, &createInfo, &allocInfo, &data.image, &data.allocation, &data.allocationInfo);
	vlkFatalAssert(result);
	data.textureState = Texture::State::UNDEFINED;

	return (result == VK_SUCCESS);
}

void Device::destroyTextureImpl(uint32_t index)
{
	BackendData &bck = *backendData_;
	BackendData::TextureData &data = bck.textureData_[index];

	ASSERT(data.image != VK_NULL_HANDLE && data.allocation != VK_NULL_HANDLE);
	vmaDestroyImage(bck.vmaAllocator_, data.image, data.allocation);
	data.image = VK_NULL_HANDLE;
	data.allocation = VK_NULL_HANDLE;
	data.textureState = Texture::State::UNDEFINED;
}

} // namespace grail
} // namespace ncine
