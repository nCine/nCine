#include <nctl/HashMap.h>
#include "grail/Device.h"
#include "grail/config.h"
#include "grail/private/HandlePool.h"
#include "grail/handle_types.h"
#include "grail/private/hash_types.h"

namespace ncine {
namespace grail {

struct Device::FrontendData
{
	HandlePool<BindGroupLayout::Handle> bindGroupLayoutHandles_{ PoolLimits::BindGroupLayouts };
	BindGroupLayout::Desc bindGroupLayoutDescs_[PoolLimits::BindGroupLayouts];

	GrlHandleHashMap<BindGroupLayout::Tag> bindGroupLayoutFromHandleToHash_{ PoolLimits::BindGroupLayouts };
	Desc64HashMap<BindGroupLayout::Handle> bindGroupLayoutFromHashToHandle_{ PoolLimits::BindGroupLayouts };

	HandlePool<BindGroup::Handle> bindGroupHandles_{ PoolLimits::BindGroups };
	BindGroup::Desc bindGroupDescs_[PoolLimits::BindGroups];

	HandlePool<GraphicsPipeline::Handle> graphicsPipelineHandles_{ PoolLimits::GraphicsPipelines };
	GraphicsPipeline::Desc graphicsPipelineDescs_[PoolLimits::GraphicsPipelines];

	GrlHandleHashMap<GraphicsPipeline::Tag> graphicsPipelineFromHandleToHash_{ PoolLimits::GraphicsPipelines };
	Desc64HashMap<GraphicsPipeline::Handle> graphicsPipelineFromHashToHandle_{ PoolLimits::GraphicsPipelines };

	HandlePool<Buffer::Handle> bufferHandles_{ PoolLimits::Buffers };
	Buffer::Desc bufferDescs_[PoolLimits::Buffers];

	HandlePool<Texture::Handle> textureHandles_{ PoolLimits::Textures };
	Texture::Desc textureDescs_[PoolLimits::Textures];

	HandlePool<TextureView::Handle> textureViewHandles_{ PoolLimits::TextureViews };
	TextureView::Desc textureViewDescs_[PoolLimits::TextureViews];

	/// Creates a texture handle without creating the implementation resource
	/*! \note Used by the Vulkan swapchain creation process. */
	NODISCARD Texture::Handle createTextureHandle(const Texture::Desc &desc);

	bool isAlive(BindGroupLayout::Handle handle) const;
	bool isAlive(BindGroup::Handle handle) const;
	bool isAlive(GraphicsPipeline::Handle handle) const;
	bool isAlive(Buffer::Handle handle) const;
	bool isAlive(Texture::Handle handle) const;
	bool isAlive(TextureView::Handle handle) const;
};

} // namespace grail
} // namespace ncine
