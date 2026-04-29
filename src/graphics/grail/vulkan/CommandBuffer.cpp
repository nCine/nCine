#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include "grail/CommandBuffer.h"
#include "grail/RenderPass.h"
#include "grail/private/hash_functions.h"
#include "grail/private/Device_frontend.h"
#include "grail/vulkan/Device_backend.h"
#include "grail/vulkan/vlk_utils.h"

namespace ncine {
namespace grail {

namespace {

VkAccessFlags grlTextureStateToVkAccess(Texture::State state)
{
	switch (state)
	{
		case Texture::State::UNDEFINED: return 0;

		case Texture::State::TRANSFER_SRC: return VK_ACCESS_TRANSFER_READ_BIT;
		case Texture::State::TRANSFER_DST: return VK_ACCESS_TRANSFER_WRITE_BIT;

		case Texture::State::SHADER_READ: return VK_ACCESS_SHADER_READ_BIT;
		case Texture::State::SHADER_WRITE: return VK_ACCESS_SHADER_WRITE_BIT;

		case Texture::State::RENDER_TARGET: return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		case Texture::State::DEPTH_READ: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		case Texture::State::DEPTH_WRITE:
			return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
			       VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		case Texture::State::PRESENT: return 0;

		default: return 0;
	}
}

VkPipelineStageFlags grlTextureStateToVkStage(Texture::State state)
{
	switch (state)
	{
		case Texture::State::UNDEFINED:
			return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		case Texture::State::TRANSFER_SRC:
		case Texture::State::TRANSFER_DST:
			return VK_PIPELINE_STAGE_TRANSFER_BIT;

		case Texture::State::SHADER_READ:
		case Texture::State::SHADER_WRITE:
			return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT; // conservative but safe

		case Texture::State::RENDER_TARGET:
			return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		case Texture::State::DEPTH_READ:
		case Texture::State::DEPTH_WRITE:
			return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
			       VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

		case Texture::State::PRESENT:
			return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

		default:
			return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	}
}

VkAccessFlags grlBufferStateToVkAccess(Buffer::State state)
{
	switch (state)
	{
		case Buffer::State::TRANSFER_SRC: return VK_ACCESS_TRANSFER_READ_BIT;
		case Buffer::State::TRANSFER_DST: return VK_ACCESS_TRANSFER_WRITE_BIT;

		case Buffer::State::VERTEX_BUFFER: return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		case Buffer::State::INDEX_BUFFER: return VK_ACCESS_INDEX_READ_BIT;

		case Buffer::State::UNIFORM_BUFFER: return VK_ACCESS_UNIFORM_READ_BIT;

		case Buffer::State::STORAGE_READ: return VK_ACCESS_SHADER_READ_BIT;
		case Buffer::State::STORAGE_WRITE: return VK_ACCESS_SHADER_WRITE_BIT;

		case Buffer::State::INDIRECT_BUFFER: return VK_ACCESS_INDIRECT_COMMAND_READ_BIT;

		default: return 0;
	}
}

VkPipelineStageFlags grlBufferStateToVkStage(Buffer::State state)
{
	switch (state)
	{
		case Buffer::State::UNDEFINED:
			return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		case Buffer::State::TRANSFER_SRC:
		case Buffer::State::TRANSFER_DST:
			return VK_PIPELINE_STAGE_TRANSFER_BIT;

		case Buffer::State::VERTEX_BUFFER:
			return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		case Buffer::State::INDEX_BUFFER:
			return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

		case Buffer::State::UNIFORM_BUFFER:
			return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		case Buffer::State::STORAGE_READ:
		case Buffer::State::STORAGE_WRITE:
			return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
				VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;

		case Buffer::State::INDIRECT_BUFFER:
			return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;

		default:
			return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	}
}

}

///////////////////////////////////////////////////////////
// CommandBuffer::BackendData
///////////////////////////////////////////////////////////

bool CommandBuffer::BackendData::tryBindDescriptorSet()
{
	if (bindGroup.isValid() == false)
		return false;

	Device::BackendData &devBck = *deviceBackend;
	VkDescriptorSet descriptorSet = devBck.bindGroupData_[bindGroup.index()].descriptorSet;

	switch (activeBindPoint)
	{
		case BindPoint::NONE:
			return false;
		case BindPoint::GRAPHICS:
		{
			if (graphicsPipeline.isValid() == false)
				return false;

			VkPipelineLayout pipelineLayout = devBck.graphicsPipelineData_[graphicsPipeline.index()].pipelineLayout;
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, numDynamicOffsets, dynamicOffsets);
			break;
		}

		case BindPoint::COMPUTE:
		{
			if (computePipeline.isValid() == false)
				return false;

			VkPipelineLayout pipelineLayout = devBck.computePipelineData_[computePipeline.index()].pipelineLayout;
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, numDynamicOffsets, dynamicOffsets);
			break;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////
// CommandBuffer
///////////////////////////////////////////////////////////

void CommandBuffer::begin()
{
	BackendData &bck = *backendData_;
	ASSERT(bck.commandBuffer != VK_NULL_HANDLE);
	ASSERT(bck.deviceBackend != nullptr);
	ASSERT(bck.deviceFrontend != nullptr);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = nullptr;

	const VkResult result = vkBeginCommandBuffer(bck.commandBuffer, &beginInfo);
	vlkFatalAssert(result);
}

void CommandBuffer::beginRenderPass(const RenderPass::RenderPassDesc &passDesc, const RenderPass::RenderTargetDesc &targetDesc, const char *debugName)
{
	BackendData &bck = *backendData_;
	Device::BackendData &devBck = *bck.deviceBackend;

	bool hasSwapchainAttachments = false;
	bool isSwapchainAttachment[RenderPass::MaxColorAttachments] = { false, false, false, false };
	for (uint32_t i = 0; i < passDesc.layout.colorCount; i++)
	{
		const Device::BackendData::TextureViewData &data = devBck.textureViewData_[targetDesc.color[i].index()];
		isSwapchainAttachment[i] = data.isSwapchain;
		if (data.isSwapchain)
			hasSwapchainAttachments = true;
	}

	const uint64_t renderPassHash = hashDesc(passDesc, isSwapchainAttachment);
	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkRenderPass *renderPassPtr = devBck.hashToVkRenderPasses_.find(renderPassHash);
	if (renderPassPtr != nullptr)
		renderPass = *renderPassPtr;
	else
	{
		renderPass = devBck.createRenderPass(passDesc, isSwapchainAttachment);
		devBck.hashToVkRenderPasses_.insert(renderPassHash, renderPass);
	}

	if (devBck.caps_.debugUtils)
		setObjectName(devBck.device_, renderPass, debugName);

	// Note: render pass hash is combined with framebuffer hash, as a VkFramebuffer references a VkRenderPass
	const uint64_t framebufferHash = hashDesc(targetDesc, renderPassHash);
	VkFramebuffer framebuffer = VK_NULL_HANDLE;
	VkFramebuffer *framebufferPtr = devBck.hashToVkFramebuffers_.find(framebufferHash);
	if (framebufferPtr != nullptr)
		framebuffer = *framebufferPtr;
	else
	{
		framebuffer = devBck.createFramebuffer(targetDesc, renderPass);
		devBck.hashToVkFramebuffers_.insert(framebufferHash, framebuffer);
		if (hasSwapchainAttachments)
			devBck.swapchainVkFramebuffersHashes_.pushBack(framebufferHash);
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = { targetDesc.width, targetDesc.height };

	VkClearValue clearValues[RenderPass::MaxColorAttachments + 1]{};
	uint32_t clearIndex = 0;

	for (uint32_t i = 0; i < passDesc.layout.colorCount; i++)
	{
		const ClearColorValue &col = passDesc.clearColors[i];
		clearValues[clearIndex].color = { { col.float32[0], col.float32[1], col.float32[2], col.float32[3] } };
		clearIndex++;
	}

	if (passDesc.layout.depth.format != Format::UNDEFINED)
	{
		clearValues[clearIndex].depthStencil = { passDesc.clearDepth, 0 };
		clearIndex++;
	}

	renderPassInfo.clearValueCount = clearIndex;
	renderPassInfo.pClearValues = clearValues;

	vkCmdBeginRenderPass(bck.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::beginRenderPass(const RenderPass::RenderPassDesc &passDesc, const RenderPass::RenderTargetDesc &targetDesc)
{
	return beginRenderPass(passDesc, targetDesc, nullptr);
}

void CommandBuffer::bindGraphicsPipeline(GraphicsPipeline::Handle handle)
{
	BackendData &bck = *backendData_;
	Device::BackendData &devBck = *bck.deviceBackend;

	VkPipeline pipeline = devBck.graphicsPipelineData_[handle.index()].pipeline;
	vkCmdBindPipeline(bck.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	// Try binding descriptors if a bind group is present and this is the first pipeline to be bound
	const bool bindDescriptors = (bck.activeBindPoint == BackendData::BindPoint::NONE && bck.bindGroup.isValid());

	bck.graphicsPipeline = handle;
	bck.activeBindPoint = BackendData::BindPoint::GRAPHICS;
	if (bindDescriptors)
		bck.tryBindDescriptorSet();
}

void CommandBuffer::bindComputePipeline(ComputePipeline::Handle handle)
{
	BackendData &bck = *backendData_;
	Device::BackendData &devBck = *bck.deviceBackend;

	VkPipeline pipeline = devBck.computePipelineData_[handle.index()].pipeline;
	vkCmdBindPipeline(bck.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

	// Try binding descriptors if a bind group is present and this is the first pipeline to be bound
	const bool bindDescriptors = (bck.activeBindPoint == BackendData::BindPoint::NONE && bck.bindGroup.isValid());

	bck.computePipeline = handle;
	bck.activeBindPoint = BackendData::BindPoint::COMPUTE;
	if (bindDescriptors)
		bck.tryBindDescriptorSet();
}

void CommandBuffer::bindVertexBuffer(Buffer::Handle handle)
{
	BackendData &bck = *backendData_;
	Device::BackendData &devBck = *bck.deviceBackend;

	VkBuffer buffer = devBck.bufferData_[handle.index()].buffer;

	VkBuffer vertexBuffers[] = { buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(bck.commandBuffer, 0, 1, vertexBuffers, offsets);
}

void CommandBuffer::bindIndexBuffer(Buffer::Handle handle)
{
	BackendData &bck = *backendData_;
	Device::BackendData &devBck = *bck.deviceBackend;

	VkBuffer buffer = devBck.bufferData_[handle.index()].buffer;
	vkCmdBindIndexBuffer(bck.commandBuffer, buffer, 0, VK_INDEX_TYPE_UINT16);
}

void CommandBuffer::bindBindGroup(BindGroup::Handle handle)
{
	BackendData &bck = *backendData_;

	bck.numDynamicOffsets = 0;
	bck.bindGroup = handle;
	bck.tryBindDescriptorSet();
}

void CommandBuffer::bindBindGroup(BindGroup::Handle handle, const uint32_t *offsets, uint32_t offsetCount)
{
	BackendData &bck = *backendData_;

	bck.numDynamicOffsets = offsetCount;
	for (uint32_t i = 0; i < offsetCount; i++)
		bck.dynamicOffsets[i] = offsets[i];

	bck.bindGroup = handle;
	bck.tryBindDescriptorSet();
}

void CommandBuffer::setViewport(Offset2D offset, Extent2D extent, float minDepth, float maxDepth)
{
	BackendData &bck = *backendData_;

	VkViewport viewport{};
	viewport.x = offset.x;
	viewport.y = offset.y;
	viewport.width = static_cast<float>(extent.width);
	viewport.height = static_cast<float>(extent.height);
	viewport.minDepth = minDepth;
	viewport.maxDepth = maxDepth;

	vkCmdSetViewport(bck.commandBuffer, 0, 1, &viewport);
}

void CommandBuffer::setScissor(Offset2D offset, Extent2D extent)
{
	BackendData &bck = *backendData_;

	VkRect2D scissor{};
	scissor.offset = { offset.x, offset.y };
	scissor.extent = { extent.width, extent.height };

	vkCmdSetScissor(bck.commandBuffer, 0, 1, &scissor);
}

void CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
	BackendData &bck = *backendData_;
	vkCmdDraw(bck.commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
	BackendData &bck = *backendData_;
	vkCmdDrawIndexed(bck.commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	BackendData &bck = *backendData_;
	vkCmdDispatch(bck.commandBuffer, groupCountX, groupCountY, groupCountZ);
}

void CommandBuffer::copyBuffer(Buffer::Handle srcHandle, Buffer::Handle dstHandle, uint64_t srcOffset, uint64_t dstOffset, uint64_t size)
{
	BackendData &bck = *backendData_;
	Device::BackendData &devBck = *bck.deviceBackend;

	VkBuffer srcBuffer = devBck.bufferData_[srcHandle.index()].buffer;
	VkBuffer dstBuffer = devBck.bufferData_[dstHandle.index()].buffer;

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = srcOffset;
	copyRegion.dstOffset = dstOffset;
	copyRegion.size = size;
	vkCmdCopyBuffer(bck.commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
}

void CommandBuffer::copyBuffer(Buffer::Handle srcHandle, Buffer::Handle dstHandle)
{
	BackendData &bck = *backendData_;
	const Device::FrontendData &devFrn = *bck.deviceFrontend;

	const Buffer::Desc &srcDesc = devFrn.bufferDescs_[srcHandle.index()];
	return copyBuffer(srcHandle, dstHandle, 0, 0, srcDesc.size);
}

void CommandBuffer::copyBufferToTexture(Buffer::Handle srcHandle, Texture::Handle dstHandle, uint64_t srcOffset, Offset3D imgOffset, Extent3D imgExtent)
{
	BackendData &bck = *backendData_;
	Device::BackendData &devBck = *bck.deviceBackend;

	VkBuffer srcBuffer = devBck.bufferData_[srcHandle.index()].buffer;
	VkImage dstImage = devBck.textureData_[dstHandle.index()].image;

	VkBufferImageCopy copyRegion{};
	copyRegion.bufferOffset = srcOffset;
	copyRegion.bufferRowLength = 0;
	copyRegion.bufferImageHeight = 0;

	copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.imageSubresource.mipLevel = 0;
	copyRegion.imageSubresource.baseArrayLayer = 0;
	copyRegion.imageSubresource.layerCount = 1;

	copyRegion.imageOffset = { imgOffset.x, imgOffset.y, imgOffset.z };
	copyRegion.imageExtent = { imgExtent.width, imgExtent.height, imgExtent.depth };

	vkCmdCopyBufferToImage(bck.commandBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
}

void CommandBuffer::copyBufferToTexture(Buffer::Handle srcHandle, Texture::Handle dstHandle)
{
	BackendData &bck = *backendData_;
	const Device::FrontendData &devFrn = *bck.deviceFrontend;

	const Texture::Desc &dstDesc = devFrn.textureDescs_[dstHandle.index()];

	const Offset3D imgOffset{};
	const Extent3D imgExtent{ dstDesc.width, dstDesc.height, dstDesc.depth };
	return copyBufferToTexture(srcHandle, dstHandle, 0, imgOffset, imgExtent);
}

void CommandBuffer::transitionTexture(Texture::Handle handle, Texture::State newState)
{
	BackendData &bck = *backendData_;
	Device::BackendData &devBck = *bck.deviceBackend;

	Device::BackendData::TextureData &data = devBck.textureData_[handle.index()];

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

	barrier.oldLayout = grlTextureStateToVkImageLayout(data.textureState);
	barrier.newLayout = grlTextureStateToVkImageLayout(newState);

	barrier.srcAccessMask = grlTextureStateToVkAccess(data.textureState);
	barrier.dstAccessMask = grlTextureStateToVkAccess(newState);

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = data.image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage = grlTextureStateToVkStage(data.textureState);
	VkPipelineStageFlags destinationStage = grlTextureStateToVkStage(newState);

	vkCmdPipelineBarrier(bck.commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	data.textureState = newState;
}

void CommandBuffer::transitionBuffer(Buffer::Handle handle, Buffer::State newState)
{
	BackendData &bck = *backendData_;
	Device::BackendData &devBck = *bck.deviceBackend;
	Device::FrontendData &devFrn = *bck.deviceFrontend;

	Device::BackendData::BufferData &data = devBck.bufferData_[handle.index()];
	const Buffer::Desc &desc = devFrn.bufferDescs_[handle.index()];

	VkBufferMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;

	barrier.srcAccessMask = grlBufferStateToVkAccess(data.bufferState);
	barrier.dstAccessMask = grlBufferStateToVkAccess(newState);

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.buffer = data.buffer;
	barrier.offset = 0;
	barrier.size = desc.size;

	VkPipelineStageFlags sourceStage = grlBufferStateToVkStage(data.bufferState);
	VkPipelineStageFlags destinationStage = grlBufferStateToVkStage(newState);

	vkCmdPipelineBarrier(bck.commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 1, &barrier, 0, nullptr);
	data.bufferState = newState;
}

void CommandBuffer::endRenderPass()
{
	BackendData &bck = *backendData_;
	vkCmdEndRenderPass(bck.commandBuffer);
}

void CommandBuffer::beginLabel(const char *labelName)
{
	BackendData &bck = *backendData_;
	if (bck.deviceBackend->caps_.debugUtils)
	{
		VkDebugUtilsLabelEXT labelInfo{};
		labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		labelInfo.pLabelName = labelName;

		vkCmdBeginDebugUtilsLabelEXT(bck.commandBuffer, &labelInfo);
	}
}

void CommandBuffer::insertLabel(const char *labelName)
{
	BackendData &bck = *backendData_;
	if (bck.deviceBackend->caps_.debugUtils)
	{
		VkDebugUtilsLabelEXT labelInfo{};
		labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		labelInfo.pLabelName = labelName;

		vkCmdInsertDebugUtilsLabelEXT(bck.commandBuffer, &labelInfo);
	}
}

void CommandBuffer::endLabel()
{
	BackendData &bck = *backendData_;
	if (bck.deviceBackend->caps_.debugUtils)
		vkCmdEndDebugUtilsLabelEXT(bck.commandBuffer);
}

void CommandBuffer::end()
{
	BackendData &bck = *backendData_;
	const VkResult result = vkEndCommandBuffer(bck.commandBuffer);
	vlkFatalAssert(result);

	bck.graphicsPipeline = GraphicsPipeline::Handle::Invalid();
	bck.bindGroup = BindGroup::Handle::Invalid();
}

} // namespace grail
} // namespace ncine
