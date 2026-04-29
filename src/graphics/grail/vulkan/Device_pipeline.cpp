#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include "grail/Device.h"
#include "grail/vulkan/Device_backend.h"
#include "grail/vulkan/vlk_utils.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// Device::BackendData
///////////////////////////////////////////////////////////

VkShaderModule Device::BackendData::createShaderModule(const uint8_t *codeData, size_t codeSize)
{
	if (codeSize == 0 || codeData == nullptr || codeSize % 4 != 0)
		return VK_NULL_HANDLE;

	const uint32_t magic = *reinterpret_cast<const uint32_t *>(codeData);
	ASSERT(magic == 0x07230203);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = codeSize;
	createInfo.pCode = reinterpret_cast<const uint32_t *>(codeData);

	VkShaderModule shaderModule = VK_NULL_HANDLE;
	const VkResult result = vkCreateShaderModule(device_, &createInfo, allocator_, &shaderModule);
	vlkFatalAssert(result);

	return shaderModule;
}

void Device::BackendData::destroyShaderModule(VkShaderModule shaderModule)
{
	ASSERT(shaderModule != VK_NULL_HANDLE);
	vkDestroyShaderModule(device_, shaderModule, allocator_);
}

VkPipelineLayout Device::BackendData::createPipelineLayout(const GraphicsPipeline::PipelineLayoutCreateDesc &desc)
{
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

	VkDescriptorSetLayout descriptorSetLayouts[GraphicsPipeline::MaxBindGroups];
	for (unsigned int i = 0; i < desc.bindGroupLayoutCount; i++)
	{
		BindGroupLayout::Handle bindGroupLayoutHandle = desc.bindGroupLayouts[i];
		Device::BackendData::BindGroupLayoutData &data = bindGroupLayoutData_[bindGroupLayoutHandle.index()];
		descriptorSetLayouts[i] = data.descriptorSetLayout;
	}

	VkPipelineLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.setLayoutCount = desc.bindGroupLayoutCount;
	createInfo.pSetLayouts = (desc.bindGroupLayoutCount > 0) ? descriptorSetLayouts : nullptr;
	createInfo.pushConstantRangeCount = 0;
	createInfo.pPushConstantRanges = nullptr;

	const VkResult result = vkCreatePipelineLayout(device_, &createInfo, allocator_, &pipelineLayout);
	vlkFatalAssert(result);

	return pipelineLayout;
}

void Device::BackendData::destroyPipelineLayout(VkPipelineLayout pipelineLayout)
{
	ASSERT(pipelineLayout != VK_NULL_HANDLE);
	vkDestroyPipelineLayout(device_, pipelineLayout, allocator_);
}

} // namespace grail
} // namespace ncine
