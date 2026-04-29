#include "grail/Device.h"
#include "grail/private/hash_functions.h"
#include "grail/private/Device_frontend.h"

namespace ncine {
namespace grail {

namespace {

void copyDesc(const ComputePipeline::CreateDesc &createDesc, ComputePipeline::Desc &desc)
{
	if (createDesc.computeShader.isValid())
		desc.computeShaderEntry = createDesc.computeShader.entryName;

	if (createDesc.pipelineLayout.bindGroupLayoutCount > 0)
	{
		const ComputePipeline::PipelineLayoutCreateDesc &src = createDesc.pipelineLayout;

		desc.pipelineLayout.bindGroupLayouts.setSize(src.bindGroupLayoutCount);
		memcpy(desc.pipelineLayout.bindGroupLayouts.data(), src.bindGroupLayouts, src.bindGroupLayoutCount * sizeof(BindGroupLayout::Handle));
	}
}

}

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

ComputePipeline::Handle Device::createComputePipeline(const ComputePipeline::CreateDesc &desc)
{
	FrontendData &frn = *frontendData_;
	const uint64_t hashValue = hashDesc(desc, frn.bindGroupLayoutFromHandleToHash_);

	ComputePipeline::Handle *handlePtr = frn.computePipelineFromHashToHandle_.find(hashValue);
	// If another compute pipeline with the same hash already exists, don't create a new one
	if (handlePtr != nullptr)
	{
		ASSERT(frn.computePipelineHandles_.isAlive(*handlePtr) == true);
		return *handlePtr;
	}

	ComputePipeline::Handle handle = frn.computePipelineHandles_.create();

	if (handle.isValid() == false)
		return handle;

	const uint32_t index = handle.index();
	const bool success = createComputePipelineImpl(index, desc);
	if (success == false)
	{
		frn.computePipelineHandles_.destroy(handle);
		return ComputePipeline::Handle::Invalid();
	}

	frn.computePipelineFromHashToHandle_.insert(hashValue, handle);
	frn.computePipelineFromHandleToHash_.insert(handle, hashValue);

	ComputePipeline::Desc &descOut = frn.computePipelineDescs_[index];
	copyDesc(desc, descOut);

	return handle;
}

void Device::destroyComputePipeline(ComputePipeline::Handle handle)
{
	if (handle.isValid() == false)
		return;

	FrontendData &frn = *frontendData_;
	if (frn.computePipelineHandles_.isAlive(handle) == false)
		return;

	destroyComputePipelineImpl(handle.index());

	frn.computePipelineHandles_.destroy(handle);
}

const ComputePipeline::Desc &Device::computePipelineDesc(ComputePipeline::Handle handle)
{
	ASSERT(handle.isValid() == true);

	const FrontendData &frn = *frontendData_;
	ASSERT(frn.computePipelineHandles_.isAlive(handle) == true);

	return frn.computePipelineDescs_[handle.index()];
}

} // namespace grail
} // namespace ncine
