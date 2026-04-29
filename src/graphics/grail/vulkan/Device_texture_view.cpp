#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include "grail/Device.h"
#include "grail/private/hash_functions.h"
#include "grail/vulkan/Device_backend.h"
#include "grail/vulkan/vlk_utils.h"

namespace ncine {
namespace grail {

namespace {

VkImageViewType grlTextureViewTypeToVk(TextureView::Type type)
{
	switch (type)
	{
		case TextureView::Type::TEXTURE_1D: return VK_IMAGE_VIEW_TYPE_1D;
		case TextureView::Type::TEXTURE_1D_ARRAY: return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

		case TextureView::Type::TEXTURE_2D: return VK_IMAGE_VIEW_TYPE_2D;
		case TextureView::Type::TEXTURE_2D_ARRAY: return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

		case TextureView::Type::TEXTURE_3D: return VK_IMAGE_VIEW_TYPE_3D;

		case TextureView::Type::TEXTURE_CUBE: return VK_IMAGE_VIEW_TYPE_CUBE;
		case TextureView::Type::TEXTURE_CUBE_ARRAY: return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

		default: return VK_IMAGE_VIEW_TYPE_2D;
	}
}

VkFilter grlTextureViewFilterToVk(TextureView::Filter filter)
{
	switch (filter)
	{
		case TextureView::Filter::NEAREST: return VK_FILTER_NEAREST;
		case TextureView::Filter::LINEAR: return VK_FILTER_LINEAR;

		default: return VK_FILTER_NEAREST;
	}
}

VkSamplerAddressMode grlTextureViewWrapModeToVk(TextureView::WrapMode wrapMode)
{
	switch (wrapMode)
	{
		case TextureView::WrapMode::REPEAT: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case TextureView::WrapMode::MIRRORED_REPEAT: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case TextureView::WrapMode::CLAMP_TO_EDGE: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		case TextureView::WrapMode::CLAMP_TO_BORDER: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

		default: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
	}
}

VkImageAspectFlags grlFormatToVkAspectMask(Format format)
{
	switch (format)
	{
		case Format::D16_UNORM:
		case Format::X8_D24_UNORM_PACK32:
		case Format::D32_SFLOAT:
			return VK_IMAGE_ASPECT_DEPTH_BIT;

		case Format::S8_UINT:
			return VK_IMAGE_ASPECT_STENCIL_BIT;

		case Format::D16_UNORM_S8_UINT:
		case Format::D24_UNORM_S8_UINT:
		case Format::D32_SFLOAT_S8_UINT:
			return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

		default:
			return VK_IMAGE_ASPECT_COLOR_BIT;
	}
}

}

///////////////////////////////////////////////////////////
// Device::BackendData
///////////////////////////////////////////////////////////

VkSampler Device::BackendData::createSampler(const TextureView::Sampler &desc)
{
	VkSamplerCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.magFilter = grlTextureViewFilterToVk(desc.magFilter);
	createInfo.minFilter = grlTextureViewFilterToVk(desc.minFilter);
	createInfo.addressModeU = grlTextureViewWrapModeToVk(desc.wrapU);
	createInfo.addressModeV = grlTextureViewWrapModeToVk(desc.wrapV);
	createInfo.addressModeW = grlTextureViewWrapModeToVk(desc.wrapW);
	createInfo.anisotropyEnable = VK_FALSE;
	createInfo.maxAnisotropy = 1.0f;
	createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	createInfo.unnormalizedCoordinates = VK_FALSE;
	createInfo.compareEnable = VK_FALSE;
	createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	createInfo.mipLodBias = 0.0f;
	createInfo.minLod = 0.0f;
	createInfo.maxLod = 0.0f;

	VkSampler sampler = VK_NULL_HANDLE;
	const VkResult result = vkCreateSampler(device_, &createInfo, allocator_, &sampler);
	vlkFatalAssert(result);

	return sampler;
}

void Device::BackendData::destroySampler(VkSampler sampler)
{
	vkDestroySampler(device_, sampler, allocator_);
}

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

bool Device::createTextureViewImpl(uint32_t index, const TextureView::Desc &desc)
{
	BackendData &bck = *backendData_;
	BackendData::TextureViewData &data = bck.textureViewData_[index];
	ASSERT(data.imageView == VK_NULL_HANDLE && data.image == VK_NULL_HANDLE);

	Device::BackendData::TextureData &textureData = bck.textureData_[desc.texture.index()];
	VkImage image = textureData.image;

	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = grlTextureViewTypeToVk(desc.type);
	createInfo.format = grlFormatToVk(desc.format);
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = grlFormatToVkAspectMask(desc.format);
	createInfo.subresourceRange.baseMipLevel = desc.baseMip;
	createInfo.subresourceRange.levelCount = desc.mipCount;
	createInfo.subresourceRange.baseArrayLayer = desc.baseLayer;
	createInfo.subresourceRange.layerCount = desc.layerCount;

	const VkResult result = vkCreateImageView(bck.device_, &createInfo, bck.allocator_, &data.imageView);
	vlkFatalAssert(result);
	data.image = image;

	const uint64_t samplerHash = hashDesc(desc.sampler);
	VkSampler *samplerPtr = bck.hashToVkSampler_.find(samplerHash);
	if (samplerPtr != nullptr)
		data.sampler = *samplerPtr;
	else
	{
		data.sampler = bck.createSampler(desc.sampler);
		bck.hashToVkSampler_.insert(samplerHash, data.sampler);
	}

	if (bck.caps_.debugUtils && desc.debugName)
	{
		setObjectName(bck.device_, data.imageView, desc.debugName);
		setObjectName(bck.device_, data.sampler, desc.debugName);
	}

	return (result == VK_SUCCESS);
}

void Device::destroyTextureViewImpl(uint32_t index)
{
	BackendData &bck = *backendData_;
	BackendData::TextureViewData &data = bck.textureViewData_[index];

	ASSERT(data.imageView != VK_NULL_HANDLE && data.image != VK_NULL_HANDLE);
	vkDestroyImageView(bck.device_, data.imageView, bck.allocator_);
	data.imageView = VK_NULL_HANDLE;
	data.image = VK_NULL_HANDLE;
	data.sampler = VK_NULL_HANDLE;
}

} // namespace grail
} // namespace ncine
