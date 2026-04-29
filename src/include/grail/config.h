#ifndef GRAIL_CONFIG_H
#define GRAIL_CONFIG_H

#include <cstdint>

namespace ncine {
namespace grail {

namespace PoolLimits {
	constexpr uint32_t BindGroupLayouts = 64;
	constexpr uint32_t BindGroups = 512;
	constexpr uint32_t GraphicsPipelines = 512;
	constexpr uint32_t ComputePipelines = 128;
	constexpr uint32_t Buffers = 1024;
	constexpr uint32_t Textures = 1024;
	constexpr uint32_t TextureViews = 1024;
}

#ifdef WITH_VULKAN
namespace BackendCacheLimits {
	constexpr uint32_t RenderPasses = 128;
	constexpr uint32_t PipelineLayouts = 128;
	constexpr uint32_t FrameBuffers = 256;
	constexpr uint32_t Samplers = 16;
}

namespace ExtraLimits {
	constexpr uint32_t DynamicUploadBufferSize = 64 * 1024; // 64kb UBOs
	constexpr uint32_t MaxDynamicOffsetsBindGroup = 16;
	constexpr uint32_t MaxWriteDescriptorBatchSize = 16;
}
#endif

} // namespace grail
} // namespace ncine

#endif
