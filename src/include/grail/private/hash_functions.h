#ifndef GRAIL_HASHFUNCTIONS
#define GRAIL_HASHFUNCTIONS

#include "grail/private/hash_types.h"
#include "grail/handle_types.h"

namespace ncine {
namespace grail {

namespace BindGroupLayout {
	struct CreateDesc;
}

namespace GraphicsPipeline {
	struct PipelineLayoutCreateDesc;
	struct CreateDesc;
}

namespace RenderPass {
	struct RenderTargetLayoutDesc;
	struct RenderPassDesc;
	struct RenderTargetDesc;
}

namespace TextureView {
	struct Sampler;
}

uint64_t hashDesc(const BindGroupLayout::CreateDesc &desc, uint64_t seed);
uint64_t hashDesc(const BindGroupLayout::CreateDesc &desc);

uint64_t hashDesc(const GraphicsPipeline::PipelineLayoutCreateDesc &desc,
                  const GrlHandleHashMap<BindGroupLayout::Tag> &fromHandleToHash,
                  uint64_t seed);
uint64_t hashDesc(const GraphicsPipeline::PipelineLayoutCreateDesc &desc,
                  const GrlHandleHashMap<BindGroupLayout::Tag> &fromHandleToHash);

uint64_t hashDesc(const GraphicsPipeline::CreateDesc &desc,
                  const GrlHandleHashMap<BindGroupLayout::Tag> &fromHandleToHash,
                  uint64_t seed);
uint64_t hashDesc(const GraphicsPipeline::CreateDesc &desc,
                  const GrlHandleHashMap<BindGroupLayout::Tag> &fromHandleToHash);

uint64_t hashDesc(const RenderPass::RenderTargetLayoutDesc &desc, uint64_t seed);
uint64_t hashDesc(const RenderPass::RenderTargetLayoutDesc &desc);

uint64_t hashDesc(const RenderPass::RenderPassDesc &desc, uint64_t seed);
uint64_t hashDesc(const RenderPass::RenderPassDesc &desc);

uint64_t hashDesc(const RenderPass::RenderTargetDesc &desc, uint64_t seed);
uint64_t hashDesc(const RenderPass::RenderTargetDesc &desc);

uint64_t hashDesc(const TextureView::Sampler &desc, uint64_t seed);
uint64_t hashDesc(const TextureView::Sampler &desc);

} // namespace grail
} // namespace ncine

#endif
