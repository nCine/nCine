#include "grail/private/Device_frontend.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// Device::FrontendData
///////////////////////////////////////////////////////////

bool Device::FrontendData::isAlive(BindGroupLayout::Handle handle) const
{
	const bool isAlive = bindGroupLayoutHandles_.isAlive(handle);
	if (isAlive == false)
		LOGE_X("BindGroupLayout handle is not alive: %u index, %u gen", handle.index(), handle.generation());
	ASSERT(isAlive == true);
	return isAlive;
}

bool Device::FrontendData::isAlive(BindGroup::Handle handle) const
{
	const bool isAlive = bindGroupHandles_.isAlive(handle);
	if (isAlive == false)
		LOGE_X("BindGroup handle is not alive: %u index, %u gen", handle.index(), handle.generation());
	ASSERT(isAlive == true);
	return isAlive;
}

bool Device::FrontendData::isAlive(GraphicsPipeline::Handle handle) const
{
	const bool isAlive = graphicsPipelineHandles_.isAlive(handle);
	if (isAlive == false)
		LOGE_X("GraphicsPipeline handle is not alive: %u index, %u gen", handle.index(), handle.generation());
	ASSERT(isAlive == true);
	return isAlive;
}

bool Device::FrontendData::isAlive(ComputePipeline::Handle handle) const
{
	const bool isAlive = computePipelineHandles_.isAlive(handle);
	if (isAlive == false)
		LOGE_X("ComputePipeline handle is not alive: %u index, %u gen", handle.index(), handle.generation());
	ASSERT(isAlive == true);
	return isAlive;
}

bool Device::FrontendData::isAlive(Buffer::Handle handle) const
{
	const bool isAlive = bufferHandles_.isAlive(handle);
	if (isAlive == false)
		LOGE_X("Buffer handle is not alive: %u index, %u gen", handle.index(), handle.generation());
	ASSERT(isAlive == true);
	return isAlive;
}

bool Device::FrontendData::isAlive(Texture::Handle handle) const
{
	const bool isAlive = textureHandles_.isAlive(handle);
	if (isAlive == false)
		LOGE_X("Texture handle is not alive: %u index, %u gen", handle.index(), handle.generation());
	ASSERT(isAlive == true);
	return isAlive;
}

bool Device::FrontendData::isAlive(TextureView::Handle handle) const
{
	const bool isAlive = textureViewHandles_.isAlive(handle);
	if (isAlive == false)
		LOGE_X("TextureView handle is not alive: %u index, %u gen", handle.index(), handle.generation());
	ASSERT(isAlive == true);
	return isAlive;
}

} // namespace grail
} // namespace ncine
