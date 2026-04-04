#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include "grail/Device.h"
#include "grail/vulkan/Device_backend.h"
#include "grail/vulkan/vlk_utils.h"

namespace ncine {
namespace grail {

namespace {

VkDescriptorType grlBindingTypeToVk(BindGroupLayout::BindingType type, bool dynamicOffset)
{
	switch (type)
	{
		case BindGroupLayout::BindingType::SAMPLED_TEXTURE:
		case BindGroupLayout::BindingType::STORAGE_TEXTURE:
		case BindGroupLayout::BindingType::SAMPLER:
		case BindGroupLayout::BindingType::COMBINED_TEXTURE_SAMPLER:
			ASSERT(dynamicOffset == false);

		default:
			break;
	}

	switch (type)
	{
		case BindGroupLayout::BindingType::UNIFORM_BUFFER:
			return (dynamicOffset ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		case BindGroupLayout::BindingType::STORAGE_BUFFER:
		case BindGroupLayout::BindingType::READONLY_STORAGE_BUFFER:
			return (dynamicOffset ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		case BindGroupLayout::BindingType::SAMPLED_TEXTURE: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case BindGroupLayout::BindingType::STORAGE_TEXTURE: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		case BindGroupLayout::BindingType::SAMPLER: return VK_DESCRIPTOR_TYPE_SAMPLER;
		case BindGroupLayout::BindingType::COMBINED_TEXTURE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		default:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	}
}

VkShaderStageFlags grlShaderStageToVk(BindGroupLayout::ShaderStage stageFlags)
{
	VkShaderStageFlags flags = 0;
	const uint8_t mask = static_cast<uint8_t>(stageFlags);

	if (mask & static_cast<uint8_t>(BindGroupLayout::ShaderStage::VERTEX))
		flags |= VK_SHADER_STAGE_VERTEX_BIT;
	if (mask & static_cast<uint8_t>(BindGroupLayout::ShaderStage::FRAGMENT))
		flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
	if (mask & static_cast<uint8_t>(BindGroupLayout::ShaderStage::COMPUTE))
		flags |= VK_SHADER_STAGE_COMPUTE_BIT;

	return flags;
}

}

///////////////////////////////////////////////////////////
// Device::BackendData
///////////////////////////////////////////////////////////

VkDescriptorPool Device::BackendData::createDescriptorPool(VkDescriptorPoolSize *poolSizes, uint32_t poolSizeCount, uint32_t maxSets)
{
	VkDescriptorPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = poolSizeCount;
	createInfo.pPoolSizes = poolSizes;
	createInfo.maxSets = maxSets;

	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	const VkResult result = vkCreateDescriptorPool(device_, &createInfo, allocator_, &descriptorPool);
	vlkFatalAssert(result);

	return descriptorPool;
}

void Device::BackendData::destroyDescriptorPool(VkDescriptorPool descriptorPool)
{
	vkDestroyDescriptorPool(device_, descriptorPool, allocator_);
}

VkDescriptorSet Device::BackendData::allocateDescriptorSet(VkDescriptorPool descriptorPool, VkDescriptorSetLayout setLayout)
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &setLayout;

	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
	const VkResult result = vkAllocateDescriptorSets(device_, &allocInfo, &descriptorSet);
	//vlkFatalAssert(result);

	return descriptorSet;
}

void Device::BackendData::freeDescriptorSet(VkDescriptorPool descriptorPool, VkDescriptorSet descriptorSet)
{
	vkFreeDescriptorSets(device_, descriptorPool, 1, &descriptorSet);
}

void Device::BackendData::addDescriptorPool()
{
	DescriptorPoolCreateDesc createDesc;
	createDesc.maxSets = 1024;
	createDesc.combinedImageSamplerCount = 1024;
	createDesc.uniformBufferCount = 1024;
	createDesc.uniformBufferDynamicCount = 1024;

	VkDescriptorPoolSize poolSizes[3];
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[0].descriptorCount = createDesc.combinedImageSamplerCount;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[1].descriptorCount = createDesc.uniformBufferCount;
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	poolSizes[2].descriptorCount = createDesc.uniformBufferDynamicCount;

	VkDescriptorPool descriptorPool = createDescriptorPool(poolSizes, nctl::arraySize(poolSizes), createDesc.maxSets);

	descriptorPoolData_.emplaceBack();
	DescriptorPoolData &descriptorPoolData = descriptorPoolData_.back();
	descriptorPoolData.descriptorPool = descriptorPool;
	descriptorPoolData.combinedImageSamplerCount = createDesc.combinedImageSamplerCount;
	descriptorPoolData.uniformBufferCount = createDesc.uniformBufferCount;
	descriptorPoolData.uniformBufferDynamicCount = createDesc.uniformBufferDynamicCount;
	descriptorPoolData.maxSets = createDesc.maxSets;
}

VkDescriptorSet Device::BackendData::allocateFromDescriptorPools(const BindGroup::Desc &desc, VkDescriptorPool &descriptorPool)
{
	BindGroupLayout::Handle layoutHandle = desc.layout;

	VkDescriptorSetLayout descriptorSetLayout = bindGroupLayoutData_[layoutHandle.index()].descriptorSetLayout;
	DescriptorPoolData &poolData = descriptorPoolData_.back();
	VkDescriptorSet descriptorSet = allocateDescriptorSet(poolData.descriptorPool, descriptorSetLayout);

	if (descriptorSet == VK_NULL_HANDLE)
	{
		addDescriptorPool();
		poolData = descriptorPoolData_.back();
		descriptorSet = allocateDescriptorSet(poolData.descriptorPool, descriptorSetLayout);
		FATAL_ASSERT(descriptorSet != VK_NULL_HANDLE);
	}
	descriptorPool = poolData.descriptorPool;

	return descriptorSet;
}

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

bool Device::createBindGroupLayoutImpl(uint32_t index, const BindGroupLayout::CreateDesc &desc)
{
	BackendData &bck = *backendData_;
	BackendData::BindGroupLayoutData &data = bck.bindGroupLayoutData_[index];
	ASSERT(data.descriptorSetLayout == VK_NULL_HANDLE);

	nctl::Array<VkDescriptorSetLayoutBinding> layoutBindings(desc.entryCount);

	for (uint32_t i = 0; i < desc.entryCount; i++)
	{
		const BindGroupLayout::Entry &src = desc.entries[i];
		VkDescriptorSetLayoutBinding dst;

		dst.binding = src.binding;
		dst.descriptorType = grlBindingTypeToVk(src.type, src.dynamicOffset);
		dst.descriptorCount = src.count;
		dst.stageFlags = grlShaderStageToVk(src.visibility);
		dst.pImmutableSamplers = nullptr;
		layoutBindings.pushBack(dst);
	}

	VkDescriptorSetLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = layoutBindings.size();
	createInfo.pBindings = layoutBindings.data();

	const VkResult result = vkCreateDescriptorSetLayout(bck.device_, &createInfo, nullptr, &data.descriptorSetLayout);
	vlkFatalAssert(result);

	return (result == VK_SUCCESS);
}

void Device::destroyBindGroupLayoutImpl(uint32_t index)
{
	BackendData &bck = *backendData_;
	Device::BackendData::BindGroupLayoutData &data = bck.bindGroupLayoutData_[index];

	ASSERT(data.descriptorSetLayout != VK_NULL_HANDLE);
	vkDestroyDescriptorSetLayout(bck.device_, data.descriptorSetLayout, bck.allocator_);
	data.descriptorSetLayout = VK_NULL_HANDLE;
}

} // namespace grail
} // namespace ncine
