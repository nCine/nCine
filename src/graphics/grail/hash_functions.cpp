#include <nctl/HashFunctions.h>
#include "grail/private/hash_functions.h"
#include "grail/BindGroupLayout.h"
#include "grail/GraphicsPipeline.h"
#include "grail/ComputePipeline.h"
#include "grail/RenderPass.h"
#include "grail/TextureView.h"

namespace ncine {
namespace grail {

namespace {
	constexpr uint64_t InitialSeed = 0x9e3779b97f4a7c15ULL;
}

// ---------------------------------------------------------

uint64_t hashDesc(const BindGroupLayout::CreateDesc &desc, uint64_t seed)
{
	FATAL_ASSERT(desc.entryCount > 0);

	uint32_t currentBinding = 0;
	uint64_t hashValue = seed;
	hashValue = nctl::fasthash64(&desc.entryCount, sizeof(desc.entryCount), hashValue);
	for (unsigned int i = 0; i < desc.entryCount; i++)
	{
		const BindGroupLayout::Entry &entry = desc.entries[i];
		FATAL_ASSERT_MSG(entry.binding > currentBinding || i == 0, "Bindings should be ordered for hash alue to be stable");
		currentBinding = entry.binding;

		hashValue = nctl::fasthash64(&entry.binding, sizeof(entry.binding), hashValue);
		hashValue = nctl::fasthash64(&entry.type, sizeof(entry.type), hashValue);
		hashValue = nctl::fasthash64(&entry.visibility, sizeof(entry.visibility), hashValue);
		hashValue = nctl::fasthash64(&entry.count, sizeof(entry.count), hashValue);

		const uint8_t dynamicOffset = entry.dynamicOffset ? 1 : 0;
		hashValue = nctl::fasthash64(&dynamicOffset, sizeof(dynamicOffset), hashValue);
	}

	return hashValue;
}

uint64_t hashDesc(const BindGroupLayout::CreateDesc &desc)
{
	return hashDesc(desc, InitialSeed);
}

// ---------------------------------------------------------

uint64_t hashDesc(const GraphicsPipeline::PipelineLayoutCreateDesc &desc,
                  const GrlHandleHashMap<BindGroupLayout::Tag> &fromHandleToHash,
                  uint64_t seed)
{
	uint64_t hashValue = seed;
	hashValue = nctl::fasthash64(&desc.bindGroupLayoutCount, sizeof(desc.bindGroupLayoutCount), hashValue);
	for (unsigned int i = 0; i < desc.bindGroupLayoutCount; i++)
	{
		BindGroupLayout::Handle bindGroupLayoutHandle = desc.bindGroupLayouts[i];
		const uint64_t *bindGroupLayoutHashPtr = fromHandleToHash.find(bindGroupLayoutHandle);
		FATAL_ASSERT(bindGroupLayoutHashPtr != nullptr);
		if (bindGroupLayoutHashPtr != nullptr)
		{
			const uint64_t bindGroupLayoutHash = *bindGroupLayoutHashPtr;
			hashValue = nctl::fasthash64(&bindGroupLayoutHash, sizeof(bindGroupLayoutHash), hashValue);
		}
	}

	return hashValue;
}

uint64_t hashDesc(const GraphicsPipeline::PipelineLayoutCreateDesc &desc,
                  const GrlHandleHashMap<BindGroupLayout::Tag> &fromHandleToHash)
{
	return hashDesc(desc, fromHandleToHash, InitialSeed);
}

// ---------------------------------------------------------

uint64_t hashDesc(const GraphicsPipeline::VertexInputLayoutCreateDesc &desc, uint64_t seed)
{
	uint64_t hashValue = seed;

	FATAL_ASSERT((desc.bindingCount == 0 && !desc.bindings) || (desc.bindingCount > 0 && desc.bindings));
	for (uint32_t i = 0; i < desc.bindingCount; i++)
	{
		const GraphicsPipeline::VertexBinding &binding = desc.bindings[i];
		hashValue = nctl::fasthash64(&binding.binding, sizeof(binding.binding), hashValue);
		hashValue = nctl::fasthash64(&binding.stride, sizeof(binding.stride), hashValue);
		hashValue = nctl::fasthash64(&binding.inputRate, sizeof(binding.inputRate), hashValue);
	}

	FATAL_ASSERT((desc.attributeCount == 0 && !desc.attributes) || (desc.attributeCount > 0 && desc.attributes));
	for (uint32_t i = 0; i < desc.attributeCount; i++)
	{
		const GraphicsPipeline::VertexAttribute &attribute = desc.attributes[i];
		hashValue = nctl::fasthash64(&attribute.location, sizeof(attribute.location), hashValue);
		hashValue = nctl::fasthash64(&attribute.binding, sizeof(attribute.binding), hashValue);
		hashValue = nctl::fasthash64(&attribute.format, sizeof(attribute.format), hashValue);
		hashValue = nctl::fasthash64(&attribute.offset, sizeof(attribute.offset), hashValue);
	}

	return hashValue;
}

uint64_t hashDesc(const GraphicsPipeline::ColorBlendAttachmentDesc &desc, uint64_t seed)
{
	uint64_t hashValue = seed;

	const uint8_t blendEnable = desc.blendEnable ? 1 : 0;
	hashValue = nctl::fasthash64(&blendEnable, sizeof(blendEnable), hashValue);

	hashValue = nctl::fasthash64(&desc.srcColorFactor, sizeof(desc.srcColorFactor), hashValue);
	hashValue = nctl::fasthash64(&desc.dstColorFactor, sizeof(desc.dstColorFactor), hashValue);
	hashValue = nctl::fasthash64(&desc.colorOp, sizeof(desc.colorOp), hashValue);

	hashValue = nctl::fasthash64(&desc.srcAlphaFactor, sizeof(desc.srcAlphaFactor), hashValue);
	hashValue = nctl::fasthash64(&desc.dstAlphaFactor, sizeof(desc.dstAlphaFactor), hashValue);
	hashValue = nctl::fasthash64(&desc.alphaOp, sizeof(desc.alphaOp), hashValue);

	hashValue = nctl::fasthash64(&desc.colorWriteMask, sizeof(desc.colorWriteMask), hashValue);

	return hashValue;
}

uint64_t hashDesc(const GraphicsPipeline::ColorBlendStateCreateDesc &desc, uint64_t seed)
{
	uint64_t hashValue = seed;

	hashValue = nctl::fasthash64(&desc.attachmentCount, sizeof(desc.attachmentCount), hashValue);

	for (uint32_t i = 0; i < desc.attachmentCount; i++)
		hashValue = hashDesc(desc.attachments[i], hashValue);

	hashValue = nctl::fasthash64(desc.blendConstants, sizeof(desc.blendConstants), hashValue);

	return hashValue;
}

uint64_t hashDesc(const GraphicsPipeline::CreateDesc &desc,
                  const GrlHandleHashMap<BindGroupLayout::Tag> &fromHandleToHash,
                  uint64_t seed)
{
	uint64_t hashValue = seed;
	if (desc.vertexShader.isValid())
	{
		hashValue = nctl::fasthash64(&desc.vertexShader.codeSize, sizeof(desc.vertexShader.codeSize), hashValue);
		hashValue = nctl::fasthash64(&desc.vertexShader.codeData, desc.vertexShader.codeSize, hashValue);
		hashValue = nctl::fasthash64(&desc.vertexShader.entryName, strlen(desc.vertexShader.entryName), hashValue);
	}
	if (desc.fragmentShader.isValid())
	{
		hashValue = nctl::fasthash64(&desc.fragmentShader.codeSize, sizeof(desc.fragmentShader.codeSize), hashValue);
		hashValue = nctl::fasthash64(&desc.fragmentShader.codeData, desc.fragmentShader.codeSize, hashValue);
		hashValue = nctl::fasthash64(&desc.fragmentShader.entryName, strlen(desc.fragmentShader.entryName), hashValue);
	}

	if (desc.vertexInputLayout.bindingCount > 0 || desc.vertexInputLayout.attributeCount > 0)
		hashValue = hashDesc(desc.vertexInputLayout, hashValue);

	hashValue = hashDesc(desc.pipelineLayout, fromHandleToHash, hashValue);
	hashValue = hashDesc(desc.renderTargetLayout, hashValue);

	hashValue = nctl::fasthash64(&desc.primitiveTopology, sizeof(desc.primitiveTopology), hashValue);
	hashValue = nctl::fasthash64(&desc.cullMode, sizeof(desc.cullMode), hashValue);

	const uint8_t depthTestEnable = desc.depthTestEnable ? 1 : 0;
	hashValue = nctl::fasthash64(&depthTestEnable, sizeof(depthTestEnable), hashValue);
	const uint8_t depthWriteEnable = desc.depthWriteEnable ? 1 : 0;
	hashValue = nctl::fasthash64(&depthWriteEnable, sizeof(depthWriteEnable), hashValue);
	hashValue = nctl::fasthash64(&desc.depthCompareOp, sizeof(desc.depthCompareOp), hashValue);

	hashValue = hashDesc(desc.blendState, hashValue);

	return hashValue;
}

uint64_t hashDesc(const GraphicsPipeline::CreateDesc &desc,
                  const GrlHandleHashMap<BindGroupLayout::Tag> &fromHandleToHash)
{
	return hashDesc(desc, fromHandleToHash, InitialSeed);
}

// ---------------------------------------------------------

uint64_t hashDesc(const ComputePipeline::CreateDesc &desc,
                  const GrlHandleHashMap<BindGroupLayout::Tag> &fromHandleToHash,
                  uint64_t seed)
{
	uint64_t hashValue = seed;
	if (desc.computeShader.isValid())
	{
		hashValue = nctl::fasthash64(&desc.computeShader.codeSize, sizeof(desc.computeShader.codeSize), hashValue);
		hashValue = nctl::fasthash64(&desc.computeShader.codeData, desc.computeShader.codeSize, hashValue);
		hashValue = nctl::fasthash64(&desc.computeShader.entryName, strlen(desc.computeShader.entryName), hashValue);
	}

	hashValue = hashDesc(desc.pipelineLayout, fromHandleToHash, hashValue);

	return hashValue;
}

uint64_t hashDesc(const ComputePipeline::CreateDesc &desc,
                  const GrlHandleHashMap<BindGroupLayout::Tag> &fromHandleToHash)
{
	return hashDesc(desc, fromHandleToHash, InitialSeed);
}

// ---------------------------------------------------------

uint64_t hashDesc(const RenderPass::RenderTargetLayoutDesc &desc, uint64_t seed)
{
	uint64_t hashValue = seed;
	hashValue = nctl::fasthash64(&desc.colorCount, sizeof(desc.colorCount), hashValue);
	for (unsigned int i = 0; i < desc.colorCount; i++)
	{
		const RenderPass::ColorTargetDesc &entry = desc.colors[i];
		hashValue = nctl::fasthash64(&entry.format, sizeof(entry.format), hashValue);
	}
	hashValue = nctl::fasthash64(&desc.depth.format, sizeof(desc.depth.format), hashValue);

	return hashValue;
}

uint64_t hashDesc(const RenderPass::RenderTargetLayoutDesc &desc)
{
	return hashDesc(desc, InitialSeed);
}

// ---------------------------------------------------------

uint64_t hashDesc(const RenderPass::RenderPassDesc &desc, bool *isSwapchainAttachment, uint64_t seed)
{
	uint64_t hashValue = hashDesc(desc.layout, seed);

	const uint32_t colorCount = desc.layout.colorCount;
	hashValue = nctl::fasthash64(desc.colorLoadOps, colorCount * sizeof(desc.colorLoadOps[0]), hashValue);
	hashValue = nctl::fasthash64(desc.colorStoreOps, colorCount * sizeof(desc.colorStoreOps[0]), hashValue);

	hashValue = nctl::fasthash64(&desc.depthLoadOp, sizeof(desc.depthLoadOp), hashValue);
	hashValue = nctl::fasthash64(&desc.depthStoreOp, sizeof(desc.depthStoreOp), hashValue);

	uint8_t swapchainFlags[RenderPass::MaxColorAttachments];
	for (uint32_t i = 0; i < colorCount; i++)
		swapchainFlags[i] = isSwapchainAttachment[i] ? 1 : 0;

	hashValue = nctl::fasthash64(swapchainFlags, colorCount * sizeof(uint8_t), hashValue);

	return hashValue;
}

uint64_t hashDesc(const RenderPass::RenderPassDesc &desc, bool *isSwapchainAttachment)
{
	return hashDesc(desc, isSwapchainAttachment, InitialSeed);
}

// ---------------------------------------------------------

uint64_t hashDesc(const RenderPass::RenderTargetDesc &desc, uint64_t seed)
{
	uint64_t hashValue = seed;

	hashValue = nctl::fasthash64(desc.color, RenderPass::MaxColorAttachments * sizeof(desc.color), hashValue);
	hashValue = nctl::fasthash64(&desc.depth, sizeof(desc.depth), hashValue);

	hashValue = nctl::fasthash64(&desc.width, sizeof(desc.width), hashValue);
	hashValue = nctl::fasthash64(&desc.height, sizeof(desc.height), hashValue);

	return hashValue;
}

uint64_t hashDesc(const RenderPass::RenderTargetDesc &desc)
{
	return hashDesc(desc, InitialSeed);
}

// ---------------------------------------------------------

uint64_t hashDesc(const TextureView::Sampler &desc, uint64_t seed)
{
	uint64_t hashValue = seed;
	hashValue = nctl::fasthash64(&desc.magFilter, sizeof(desc.magFilter), hashValue);
	hashValue = nctl::fasthash64(&desc.minFilter, sizeof(desc.minFilter), hashValue);
	hashValue = nctl::fasthash64(&desc.wrapU, sizeof(desc.wrapU), hashValue);
	hashValue = nctl::fasthash64(&desc.wrapV, sizeof(desc.wrapV), hashValue);
	hashValue = nctl::fasthash64(&desc.wrapW, sizeof(desc.wrapW), hashValue);

	return hashValue;
}

uint64_t hashDesc(const TextureView::Sampler &desc)
{
	return hashDesc(desc, InitialSeed);
}

} // namespace grail
} // namespace ncine
