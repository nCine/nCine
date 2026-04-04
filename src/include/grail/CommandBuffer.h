#ifndef CLASS_GRAIL_COMMANDBUFFER
#define CLASS_GRAIL_COMMANDBUFFER

#include "grail/handle_types.h"
#include "grail/types.h"
#include "grail/Texture.h"

namespace ncine {
namespace grail {

namespace RenderPass {
	struct RenderPassDesc;
	struct RenderTargetDesc;
}

class CommandBuffer
{
  public:
	void begin();
	void beginRenderPass(const RenderPass::RenderPassDesc &passDesc, const RenderPass::RenderTargetDesc &targetDesc);
	void bindGraphicsPipeline(GraphicsPipeline::Handle handle);
	void bindVertexBuffer(Buffer::Handle handle);
	void bindIndexBuffer(Buffer::Handle handle);
	void bindBindGroup(BindGroup::Handle handle);
	void bindBindGroup(BindGroup::Handle handle, const uint32_t *offsets, uint32_t offsetCount);
	void setViewport(Offset2D offset, Extent2D extent, float minDepth, float maxDepth);
	void setScissor(Offset2D offset, Extent2D extent);
	void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
	void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
	void copyBuffer(Buffer::Handle srcHandle, Buffer::Handle dstHandle, uint64_t srcOffset, uint64_t dstOffset, uint64_t size);
	void copyBuffer(Buffer::Handle srcHandle, Buffer::Handle dstHandle);
	void copyBufferToTexture(Buffer::Handle srcHandle, Texture::Handle dstHandle, uint64_t srcOffset, Offset3D imgOffset, Extent3D imgExtent);
	void copyBufferToTexture(Buffer::Handle srcHandle, Texture::Handle dstHandle);
	void transitionTexture(Texture::Handle handle, Texture::State newState);
	void endRenderPass();
	void end();

  private:
	struct BackendData;
	BackendData *backendData_ = nullptr;

	friend class Device;
};

} // namespace grail
} // namespace ncine

#endif
