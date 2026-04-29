#include "grail/Device.h"
#include "grail/private/hash_functions.h"
#include "grail/private/Device_frontend.h"

namespace ncine {
namespace grail {

namespace {

void copyDesc(const GraphicsPipeline::CreateDesc &createDesc, GraphicsPipeline::Desc &desc)
{
	if (createDesc.vertexShader.isValid())
		desc.vertexShaderEntry = createDesc.vertexShader.entryName;
	if (createDesc.fragmentShader.isValid())
		desc.fragmentShaderEntry = createDesc.fragmentShader.entryName;

	if (createDesc.vertexInputLayout.bindingCount > 0 || createDesc.vertexInputLayout.attributeCount > 0)
	{
		const GraphicsPipeline::VertexInputLayoutCreateDesc &src = createDesc.vertexInputLayout;
		GraphicsPipeline::VertexInputLayoutDesc &dst = desc.vertexInputLayout;

		dst.bindings.setSize(src.bindingCount);
		memcpy(dst.bindings.data(), src.bindings, src.bindingCount * sizeof(GraphicsPipeline::VertexBinding));

		dst.attributes.setSize(src.attributeCount);
		memcpy(dst.attributes.data(), src.attributes, src.attributeCount * sizeof(GraphicsPipeline::VertexAttribute));
	}

	if (createDesc.pipelineLayout.bindGroupLayoutCount > 0)
	{
		const GraphicsPipeline::PipelineLayoutCreateDesc &src = createDesc.pipelineLayout;
		GraphicsPipeline::PipelineLayoutDesc &dst = desc.pipelineLayout;

		dst.bindGroupLayouts.setSize(src.bindGroupLayoutCount);
		memcpy(dst.bindGroupLayouts.data(), src.bindGroupLayouts, src.bindGroupLayoutCount * sizeof(BindGroupLayout::Handle));
	}

	desc.renderTargetLayout = createDesc.renderTargetLayout;

	desc.primitiveTopology = createDesc.primitiveTopology;
	desc.cullMode = createDesc.cullMode;

	desc.depthTestEnable = createDesc.depthTestEnable;
	desc.depthWriteEnable = createDesc.depthWriteEnable;
	desc.depthCompareOp = createDesc.depthCompareOp;

	if (createDesc.blendState.attachmentCount > 0)
	{
		const GraphicsPipeline::ColorBlendStateCreateDesc &src = createDesc.blendState;
		GraphicsPipeline::ColorBlendStateDesc &dst = desc.blendState;

		dst.attachments.setSize(src.attachmentCount);
		memcpy(dst.attachments.data(), src.attachments, src.attachmentCount * sizeof(GraphicsPipeline::ColorBlendAttachmentDesc));
		memcpy(dst.blendConstants, src.blendConstants, 4 * sizeof(float));
	}
}

}

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

GraphicsPipeline::Handle Device::createGraphicsPipeline(const GraphicsPipeline::CreateDesc &desc)
{
	FrontendData &frn = *frontendData_;
	const uint64_t hashValue = hashDesc(desc, frn.bindGroupLayoutFromHandleToHash_);

	GraphicsPipeline::Handle *handlePtr = frn.graphicsPipelineFromHashToHandle_.find(hashValue);
	// If another graphics pipeline with the same hash already exists, don't create a new one
	if (handlePtr != nullptr)
	{
		ASSERT(frn.graphicsPipelineHandles_.isAlive(*handlePtr) == true);
		return *handlePtr;
	}

	GraphicsPipeline::Handle handle = frn.graphicsPipelineHandles_.create();

	if (handle.isValid() == false)
		return handle;

	const uint32_t index = handle.index();
	const bool success = createGraphicsPipelineImpl(index, desc);
	if (success == false)
	{
		frn.graphicsPipelineHandles_.destroy(handle);
		return GraphicsPipeline::Handle::Invalid();
	}

	frn.graphicsPipelineFromHashToHandle_.insert(hashValue, handle);
	frn.graphicsPipelineFromHandleToHash_.insert(handle, hashValue);

	GraphicsPipeline::Desc &descOut = frn.graphicsPipelineDescs_[index];
	copyDesc(desc, descOut);

	return handle;
}

void Device::destroyGraphicsPipeline(GraphicsPipeline::Handle handle)
{
	if (handle.isValid() == false)
		return;

	FrontendData &frn = *frontendData_;
	if (frn.graphicsPipelineHandles_.isAlive(handle) == false)
		return;

	destroyGraphicsPipelineImpl(handle.index());

	frn.graphicsPipelineHandles_.destroy(handle);
}

const GraphicsPipeline::Desc &Device::graphicsPipelineDesc(GraphicsPipeline::Handle handle)
{
	ASSERT(handle.isValid() == true);

	const FrontendData &frn = *frontendData_;
	ASSERT(frn.graphicsPipelineHandles_.isAlive(handle) == true);

	return frn.graphicsPipelineDescs_[handle.index()];
}

} // namespace grail
} // namespace ncine
