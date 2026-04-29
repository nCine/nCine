#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include "grail/Device.h"
#include "grail/RenderPass.h"
#include "grail/private/hash_functions.h"
#include "grail/private/Device_frontend.h"
#include "grail/vulkan/Device_backend.h"
#include "grail/vulkan/vlk_utils.h"

namespace ncine {
namespace grail {

namespace {

VkPrimitiveTopology grlPrimitiveTopologyToVk(GraphicsPipeline::PrimitiveTopology primitiveTopology)
{
	switch (primitiveTopology)
	{
		case GraphicsPipeline::PrimitiveTopology::POINT_LIST: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

		case GraphicsPipeline::PrimitiveTopology::TRIANGLE_LIST: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case GraphicsPipeline::PrimitiveTopology::TRIANGLE_STRIP: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case GraphicsPipeline::PrimitiveTopology::TRIANGLE_FAN: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;

		default: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}
}

VkCullModeFlags grlCullModeToVk(GraphicsPipeline::CullMode cullMode)
{
	switch (cullMode)
	{
		case GraphicsPipeline::CullMode::FRONT: return VK_CULL_MODE_FRONT_BIT;
		case GraphicsPipeline::CullMode::BACK: return VK_CULL_MODE_BACK_BIT;
		case GraphicsPipeline::CullMode::FRONT_AND_BACK: return VK_CULL_MODE_FRONT_AND_BACK;

		default: return VK_CULL_MODE_BACK_BIT;
	}
}

VkCompareOp grlCompareOpToVk(GraphicsPipeline::CompareOp compareOp)
{
	switch (compareOp)
	{
		case GraphicsPipeline::CompareOp::NEVER: return VK_COMPARE_OP_NEVER;
		case GraphicsPipeline::CompareOp::LESS: return VK_COMPARE_OP_LESS;
		case GraphicsPipeline::CompareOp::EQUAL: return VK_COMPARE_OP_EQUAL;
		case GraphicsPipeline::CompareOp::LESS_OR_EQUAL: return VK_COMPARE_OP_LESS_OR_EQUAL;
		case GraphicsPipeline::CompareOp::GREATER: return VK_COMPARE_OP_GREATER;
		case GraphicsPipeline::CompareOp::NOT_EQUAL: return VK_COMPARE_OP_NOT_EQUAL;
		case GraphicsPipeline::CompareOp::GREATER_OR_EQUAL: return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case GraphicsPipeline::CompareOp::ALWAYS: return VK_COMPARE_OP_ALWAYS;

		default: return VK_COMPARE_OP_NEVER;
	}
}

VkAttachmentLoadOp grlLoadOpToVk(RenderPass::LoadOp loadOp)
{
	switch (loadOp)
	{
		case RenderPass::LoadOp::DONT_CARE: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		case RenderPass::LoadOp::LOAD: return VK_ATTACHMENT_LOAD_OP_LOAD;
		case RenderPass::LoadOp::CLEAR: return VK_ATTACHMENT_LOAD_OP_CLEAR;

		default: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	}
}

VkAttachmentStoreOp grlStoreOpToVk(RenderPass::StoreOp storeOp)
{
	switch (storeOp)
	{
		case RenderPass::StoreOp::DONT_CARE: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		case RenderPass::StoreOp::STORE: return VK_ATTACHMENT_STORE_OP_STORE;

		default: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	}
}

VkBlendFactor grlBlendFactorToVk(GraphicsPipeline::BlendFactor factor)
{
	switch (factor)
	{
		case GraphicsPipeline::BlendFactor::ZERO: return VK_BLEND_FACTOR_ZERO;
		case GraphicsPipeline::BlendFactor::ONE: return VK_BLEND_FACTOR_ONE;

		case GraphicsPipeline::BlendFactor::SRC_COLOR: return VK_BLEND_FACTOR_SRC_COLOR;
		case GraphicsPipeline::BlendFactor::ONE_MINUS_SRC_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

		case GraphicsPipeline::BlendFactor::DST_COLOR: return VK_BLEND_FACTOR_DST_COLOR;
		case GraphicsPipeline::BlendFactor::ONE_MINUS_DST_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

		case GraphicsPipeline::BlendFactor::SRC_ALPHA: return VK_BLEND_FACTOR_SRC_ALPHA;
		case GraphicsPipeline::BlendFactor::ONE_MINUS_SRC_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

		case GraphicsPipeline::BlendFactor::DST_ALPHA: return VK_BLEND_FACTOR_DST_ALPHA;
		case GraphicsPipeline::BlendFactor::ONE_MINUS_DST_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

		case GraphicsPipeline::BlendFactor::CONSTANT_COLOR: return VK_BLEND_FACTOR_CONSTANT_COLOR;
		case GraphicsPipeline::BlendFactor::ONE_MINUS_CONSTANT_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

		case GraphicsPipeline::BlendFactor::CONSTANT_ALPHA: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
		case GraphicsPipeline::BlendFactor::ONE_MINUS_CONSTANT_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;

		case GraphicsPipeline::BlendFactor::SRC_ALPHA_SATURATE: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;

		default: return VK_BLEND_FACTOR_ONE;
	}
}

VkBlendOp grlBlendOpToVk(GraphicsPipeline::BlendOp op)
{
	switch (op)
	{
		case GraphicsPipeline::BlendOp::ADD: return VK_BLEND_OP_ADD;
		case GraphicsPipeline::BlendOp::SUBTRACT: return VK_BLEND_OP_SUBTRACT;
		case GraphicsPipeline::BlendOp::REVERSE_SUBTRACT: return VK_BLEND_OP_REVERSE_SUBTRACT;
		case GraphicsPipeline::BlendOp::MIN: return VK_BLEND_OP_MIN;
		case GraphicsPipeline::BlendOp::MAX: return VK_BLEND_OP_MAX;

		default: return VK_BLEND_OP_ADD;
	}
}

VkColorComponentFlags grlColorComponentToVk(GraphicsPipeline::ColorComponent mask)
{
	VkColorComponentFlags flags = 0;

	if (uint8_t(mask) & uint8_t(GraphicsPipeline::ColorComponent::R))
		flags |= VK_COLOR_COMPONENT_R_BIT;
	if (uint8_t(mask) & uint8_t(GraphicsPipeline::ColorComponent::G))
		flags |= VK_COLOR_COMPONENT_G_BIT;
	if (uint8_t(mask) & uint8_t(GraphicsPipeline::ColorComponent::B))
		flags |= VK_COLOR_COMPONENT_B_BIT;
	if (uint8_t(mask) & uint8_t(GraphicsPipeline::ColorComponent::A))
		flags |= VK_COLOR_COMPONENT_A_BIT;

	return flags;
}

}

///////////////////////////////////////////////////////////
// Device::BackendData
///////////////////////////////////////////////////////////

VkRenderPass Device::BackendData::createRenderPass(const RenderPass::RenderTargetLayoutDesc &desc)
{
	RenderPass::RenderPassDesc renderPassDesc{};
	renderPassDesc.layout = desc;

	bool isSwapchainAttachment[RenderPass::MaxColorAttachments];
	for (uint32_t i = 0; i < desc.colorCount; i++)
	{
		renderPassDesc.colorLoadOps[i] = RenderPass::LoadOp::DONT_CARE;
		renderPassDesc.colorStoreOps[i] = RenderPass::StoreOp::DONT_CARE;
		isSwapchainAttachment[i] = false;
	}

	renderPassDesc.depthLoadOp = RenderPass::LoadOp::DONT_CARE;
	renderPassDesc.depthStoreOp = RenderPass::StoreOp::DONT_CARE;

	return createRenderPass(renderPassDesc, isSwapchainAttachment);
}

VkRenderPass Device::BackendData::createRenderPass(const RenderPass::RenderPassDesc &desc, bool *isSwapchainAttachment)
{
	VkAttachmentDescription attachments[RenderPass::MaxColorAttachments + 1]{};
	VkAttachmentReference colorAttachmentRefs[RenderPass::MaxColorAttachments]{};
	VkAttachmentReference depthAttachmentRef{};

	uint32_t attachmentIndex = 0;

	// Color attachments
	for (uint32_t i = 0; i < desc.layout.colorCount; i++)
	{
		VkAttachmentDescription &attachment = attachments[attachmentIndex];
		attachment = {};
		attachment.format = grlFormatToVk(desc.layout.colors[i].format);
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = grlLoadOpToVk(desc.colorLoadOps[i]);
		attachment.storeOp = grlStoreOpToVk(desc.colorStoreOps[i]);
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = isSwapchainAttachment[i] ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		colorAttachmentRefs[i].attachment = attachmentIndex;
		colorAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		attachmentIndex++;
	}

	const bool hasDepthAttachment = (desc.layout.depth.format != Format::UNDEFINED);

	// Depth attachment
	if (hasDepthAttachment)
	{
		VkAttachmentDescription &attachment = attachments[attachmentIndex];
		attachment = {};
		attachment.format = grlFormatToVk(desc.layout.depth.format);
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = grlLoadOpToVk(desc.depthLoadOp);
		attachment.storeOp = grlStoreOpToVk(desc.depthStoreOp);
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		depthAttachmentRef.attachment = attachmentIndex;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		attachmentIndex++;
	}

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = desc.layout.colorCount;
	subpass.pColorAttachments = (desc.layout.colorCount > 0) ? colorAttachmentRefs : nullptr;
	subpass.pDepthStencilAttachment = hasDepthAttachment ? &depthAttachmentRef : nullptr;

	VkSubpassDependency dependencies[2]{};

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask =
	    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
	    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
	    VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[0].dstStageMask = dependencies[0].srcStageMask;
	dependencies[0].srcAccessMask = 0;
	dependencies[0].dstAccessMask =
	    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
	    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask =
	    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
	    VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask =
	    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
	    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = 0;

	VkRenderPassCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = attachmentIndex;
	createInfo.pAttachments = attachments;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpass;
	createInfo.dependencyCount = 2;
	createInfo.pDependencies = dependencies;

	VkRenderPass renderPass = VK_NULL_HANDLE;
	const VkResult result = vkCreateRenderPass(device_, &createInfo, allocator_, &renderPass);
	vlkFatalAssert(result);

	return renderPass;
}

void Device::BackendData::destroyRenderPass(VkRenderPass renderPass)
{
	ASSERT(renderPass != VK_NULL_HANDLE);
	vkDestroyRenderPass(device_, renderPass, allocator_);
}

VkFramebuffer Device::BackendData::createFramebuffer(const RenderPass::RenderTargetDesc &desc, VkRenderPass renderPass)
{
	VkImageView imageViews[RenderPass::MaxColorAttachments + 1]{};
	uint32_t attachmentIndex = 0;
	for (uint32_t i = 0; i < RenderPass::MaxColorAttachments; i++)
	{
		if (desc.color[i].isValid())
		{
			const TextureViewData &data = textureViewData_[desc.color[i].index()];
			imageViews[attachmentIndex] = data.imageView;
			attachmentIndex++;
		}
	}

	if (desc.depth.isValid())
	{
		const TextureViewData &data = textureViewData_[desc.depth.index()];
		imageViews[attachmentIndex] = data.imageView;
		attachmentIndex++;
	}

	VkFramebufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.renderPass = renderPass;
	createInfo.attachmentCount = attachmentIndex;
	createInfo.pAttachments = imageViews;
	createInfo.width = desc.width;
	createInfo.height = desc.height;
	createInfo.layers = 1;

	VkFramebuffer framebuffer = VK_NULL_HANDLE;
	const VkResult result = vkCreateFramebuffer(device_, &createInfo, allocator_, &framebuffer);
	vlkFatalAssert(result);

	return framebuffer;
}

void Device::BackendData::destroyFramebuffer(VkFramebuffer framebuffer)
{
	ASSERT(framebuffer != VK_NULL_HANDLE);
	vkDestroyFramebuffer(device_, framebuffer, allocator_);
}

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

bool Device::createGraphicsPipelineImpl(uint32_t index, const GraphicsPipeline::CreateDesc &desc)
{
	BackendData &bck = *backendData_;
	BackendData::GraphicsPipelineData &data = bck.graphicsPipelineData_[index];
	ASSERT(data.pipeline == VK_NULL_HANDLE && data.pipelineLayout == VK_NULL_HANDLE);

	const uint64_t renderPassHash = hashDesc(desc.renderTargetLayout);
	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkRenderPass *renderPassPtr = bck.hashToLayoutVkRenderPasses_.find(renderPassHash);
	if (renderPassPtr != nullptr)
		renderPass = *renderPassPtr;
	else
	{
		renderPass = bck.createRenderPass(desc.renderTargetLayout);
		bck.hashToLayoutVkRenderPasses_.insert(renderPassHash, renderPass);
	}

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

	nctl::StaticArray<VkPipelineShaderStageCreateInfo, 2> shaderStages;

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	if (desc.vertexShader.isValid())
	{
		VkShaderModule vertexShaderModule = bck.createShaderModule(desc.vertexShader.codeData, desc.vertexShader.codeSize);
		ASSERT(vertexShaderModule != VK_NULL_HANDLE);

		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertexShaderModule;
		vertShaderStageInfo.pName = desc.vertexShader.entryName;
		shaderStages.pushBack(vertShaderStageInfo);

		if (bck.caps_.debugUtils)
			setObjectName(bck.device_, vertexShaderModule, desc.vertexShader.debugName);
	}

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	if (desc.fragmentShader.isValid())
	{
		VkShaderModule fragmentShaderModule = bck.createShaderModule(desc.fragmentShader.codeData, desc.fragmentShader.codeSize);
		ASSERT(fragmentShaderModule != VK_NULL_HANDLE);

		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragmentShaderModule;
		fragShaderStageInfo.pName = desc.fragmentShader.entryName;
		shaderStages.pushBack(fragShaderStageInfo);

		if (bck.caps_.debugUtils)
			setObjectName(bck.device_, fragmentShaderModule, desc.fragmentShader.debugName);
	}

	nctl::Array<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
	nctl::Array<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
	if (desc.vertexInputLayout.bindingCount > 0 || desc.vertexInputLayout.attributeCount > 0)
	{
		vertexInputBindingDescriptions.setCapacity(desc.vertexInputLayout.bindingCount);
		for (uint32_t i = 0; i < desc.vertexInputLayout.bindingCount; i++)
		{
			const GraphicsPipeline::VertexBinding &b = desc.vertexInputLayout.bindings[i];

			VkVertexInputBindingDescription vibd{};
			vibd.binding = b.binding;
			vibd.stride = b.stride;
			vibd.inputRate = (b.inputRate == GraphicsPipeline::VertexInputRate::PER_VERTEX)
			    ? VK_VERTEX_INPUT_RATE_VERTEX
			    : VK_VERTEX_INPUT_RATE_INSTANCE;

			vertexInputBindingDescriptions.pushBack(vibd);
		}

		vertexInputAttributeDescriptions.setCapacity(desc.vertexInputLayout.attributeCount);
		for (uint32_t i = 0; i < desc.vertexInputLayout.attributeCount; i++)
		{
			const GraphicsPipeline::VertexAttribute &a = desc.vertexInputLayout.attributes[i];

			VkVertexInputAttributeDescription viad{};
			viad.location = a.location;
			viad.binding = a.binding;
			viad.format = grlFormatToVk(a.format);
			viad.offset = a.offset;

			vertexInputAttributeDescriptions.pushBack(viad);
		}
	}

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = vertexInputBindingDescriptions.size();
	vertexInputInfo.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();
	vertexInputInfo.vertexAttributeDescriptionCount = vertexInputAttributeDescriptions.size();
	vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = grlPrimitiveTopologyToVk(desc.primitiveTopology);
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	constexpr uint32_t NumDynamicStates = 2;
	VkDynamicState dynamicStates[NumDynamicStates] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = NumDynamicStates;
	dynamicState.pDynamicStates = dynamicStates;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = nullptr;
	viewportState.scissorCount = 1;
	viewportState.pScissors = nullptr;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = grlCullModeToVk(desc.cullMode);
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = desc.depthTestEnable;
	depthStencil.depthWriteEnable = desc.depthWriteEnable;
	depthStencil.depthCompareOp = grlCompareOpToVk(desc.depthCompareOp);
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f;
	depthStencil.maxDepthBounds = 1.0f;
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {};
	depthStencil.back = {};

	nctl::Array<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
	if (desc.blendState.attachmentCount > 0)
	{
		colorBlendAttachments.setCapacity(desc.blendState.attachmentCount);
		for (uint32_t i = 0; i < desc.blendState.attachmentCount; i++)
		{
			const GraphicsPipeline::ColorBlendAttachmentDesc &src = desc.blendState.attachments[i];

			VkPipelineColorBlendAttachmentState dest{};
			dest.blendEnable = src.blendEnable ? VK_TRUE : VK_FALSE;

			dest.srcColorBlendFactor = grlBlendFactorToVk(src.srcColorFactor);
			dest.dstColorBlendFactor = grlBlendFactorToVk(src.dstColorFactor);
			dest.colorBlendOp = grlBlendOpToVk(src.colorOp);

			dest.srcAlphaBlendFactor = grlBlendFactorToVk(src.srcAlphaFactor);
			dest.dstAlphaBlendFactor = grlBlendFactorToVk(src.dstAlphaFactor);
			dest.alphaBlendOp = grlBlendOpToVk(src.alphaOp);

			dest.colorWriteMask = grlColorComponentToVk(src.colorWriteMask);

			colorBlendAttachments.pushBack(dest);
		}
	}
	else
	{
		VkPipelineColorBlendAttachmentState dummy{};
		dummy.blendEnable = VK_FALSE;
		dummy.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		dummy.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		dummy.colorBlendOp = VK_BLEND_OP_ADD;
		dummy.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		dummy.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		dummy.alphaBlendOp = VK_BLEND_OP_ADD;
		dummy.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		colorBlendAttachments.pushBack(dummy);
	}

	VkPipelineColorBlendStateCreateInfo blendState{};
	blendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendState.logicOpEnable = VK_FALSE;
	blendState.logicOp = VK_LOGIC_OP_COPY;
	blendState.attachmentCount = colorBlendAttachments.size();
	blendState.pAttachments = colorBlendAttachments.data();
	memcpy(blendState.blendConstants, desc.blendState.blendConstants, sizeof(float) * 4);

	VkGraphicsPipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = shaderStages.size();
	createInfo.pStages = shaderStages.data();

	createInfo.pVertexInputState = &vertexInputInfo;
	createInfo.pInputAssemblyState = &inputAssembly;
	createInfo.pViewportState = &viewportState;
	createInfo.pRasterizationState = &rasterizer;
	createInfo.pMultisampleState = &multisampling;
	createInfo.pDepthStencilState = &depthStencil;
	createInfo.pColorBlendState = &blendState;
	createInfo.pDynamicState = &dynamicState;

	createInfo.layout = pipelineLayout;
	createInfo.renderPass = renderPass;
	createInfo.subpass = 0;
	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = -1;

	VkPipeline pipeline = VK_NULL_HANDLE;
	const VkResult result = vkCreateGraphicsPipelines(bck.device_, VK_NULL_HANDLE, 1, &createInfo, bck.allocator_, &pipeline);
	vlkFatalAssert(result);
	data.pipeline = pipeline;
	data.pipelineLayout = pipelineLayout;

	if (bck.caps_.debugUtils && desc.debugName)
		setObjectName(bck.device_, data.pipeline, desc.debugName);

	if (vertShaderStageInfo.module != VK_NULL_HANDLE)
		bck.destroyShaderModule(vertShaderStageInfo.module);

	if (fragShaderStageInfo.module != VK_NULL_HANDLE)
		bck.destroyShaderModule(fragShaderStageInfo.module);

	return (result == VK_SUCCESS);
}

void Device::destroyGraphicsPipelineImpl(uint32_t index)
{
	BackendData &bck = *backendData_;
	BackendData::GraphicsPipelineData &data = bck.graphicsPipelineData_[index];

	ASSERT(data.pipeline != VK_NULL_HANDLE && data.pipelineLayout != VK_NULL_HANDLE);
	vkDestroyPipeline(bck.device_, data.pipeline, bck.allocator_);
	data.pipeline = VK_NULL_HANDLE;
	data.pipelineLayout = VK_NULL_HANDLE;
}

} // namespace grail
} // namespace ncine
