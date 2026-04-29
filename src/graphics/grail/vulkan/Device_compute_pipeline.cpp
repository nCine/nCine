#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include "grail/Device.h"
#include "grail/private/hash_functions.h"
#include "grail/private/Device_frontend.h"
#include "grail/vulkan/Device_backend.h"
#include "grail/vulkan/vlk_utils.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

bool Device::createComputePipelineImpl(uint32_t index, const ComputePipeline::CreateDesc &desc)
{
	BackendData &bck = *backendData_;
	BackendData::ComputePipelineData &data = bck.computePipelineData_[index];
	ASSERT(data.pipeline == VK_NULL_HANDLE && data.pipelineLayout == VK_NULL_HANDLE);

	// A pipeline layout is needed even with no associated descriptor sets
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	const FrontendData &frn = *frontendData_;
	const uint64_t hashValue = hashDesc(desc.pipelineLayout, frn.bindGroupLayoutFromHandleToHash_);

	VkPipelineLayout *pipelineLayoutPtr = bck.hashToVkPipelineLayout_.find(hashValue);
	if (pipelineLayoutPtr != nullptr)
		pipelineLayout = *pipelineLayoutPtr;
	else
	{
		pipelineLayout = bck.createPipelineLayout(desc.pipelineLayout);
		bck.hashToVkPipelineLayout_.insert(hashValue, pipelineLayout);
	}

	VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
	if (desc.computeShader.isValid())
	{
		VkShaderModule computeShaderModule = bck.createShaderModule(desc.computeShader.codeData, desc.computeShader.codeSize);
		ASSERT(computeShaderModule != VK_NULL_HANDLE);

		computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		computeShaderStageInfo.module = computeShaderModule;
		computeShaderStageInfo.pName = desc.computeShader.entryName;

		if (bck.caps_.debugUtils)
			setObjectName(bck.device_, computeShaderModule, desc.computeShader.debugName);
	}

	VkComputePipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.stage = computeShaderStageInfo;
	createInfo.layout = pipelineLayout;
	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = -1;

	VkPipeline pipeline = VK_NULL_HANDLE;
	const VkResult result = vkCreateComputePipelines(bck.device_, VK_NULL_HANDLE, 1, &createInfo, bck.allocator_, &pipeline);
	vlkFatalAssert(result);
	data.pipeline = pipeline;
	data.pipelineLayout = pipelineLayout;

	if (bck.caps_.debugUtils && desc.debugName)
		setObjectName(bck.device_, data.pipeline, desc.debugName);

	if (computeShaderStageInfo.module != VK_NULL_HANDLE)
		bck.destroyShaderModule(computeShaderStageInfo.module);

	return (result == VK_SUCCESS);
}

void Device::destroyComputePipelineImpl(uint32_t index)
{
	BackendData &bck = *backendData_;
	BackendData::ComputePipelineData &data = bck.computePipelineData_[index];

	ASSERT(data.pipeline != VK_NULL_HANDLE && data.pipelineLayout != VK_NULL_HANDLE);
	vkDestroyPipeline(bck.device_, data.pipeline, bck.allocator_);
	data.pipeline = VK_NULL_HANDLE;
	data.pipelineLayout = VK_NULL_HANDLE;
}

} // namespace grail
} // namespace ncine
