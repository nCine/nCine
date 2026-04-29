#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include "grail/Device.h"
#include "grail/private/Device_frontend.h"
#include "grail/vulkan/Device_backend.h"
#include "grail/vulkan/vlk_utils.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////

bool Device::createBindGroupImpl(uint32_t index, const BindGroup::Desc &desc)
{
	BackendData &bck = *backendData_;
	BackendData::BindGroupData &data = bck.bindGroupData_[index];
	ASSERT(data.descriptorSet == VK_NULL_HANDLE && data.descriptorPool == VK_NULL_HANDLE);

	const BindGroupLayout::Desc &layoutDesc = bindGroupLayoutDesc(desc.layout);
	ASSERT(desc.entryCount == layoutDesc.entries.size());

	data.descriptorSet = bck.allocateFromDescriptorPools(desc, data.descriptorPool);
	if (bck.caps_.debugUtils)
		setObjectName(bck.device_, data.descriptorSet, desc.debugName);

	VkWriteDescriptorSet descriptorWrites[ExtraLimits::MaxWriteDescriptorBatchSize]{};
	VkDescriptorBufferInfo bufferInfos[ExtraLimits::MaxWriteDescriptorBatchSize]{};
	VkDescriptorImageInfo imageInfos[ExtraLimits::MaxWriteDescriptorBatchSize]{};
	FATAL_ASSERT(desc.entryCount <= ExtraLimits::MaxWriteDescriptorBatchSize);

	uint32_t descWriteIndex = 0;
	uint32_t bufferInfoIndex = 0;
	uint32_t imageInfoIndex = 0;
	for (uint32_t i = 0; i < desc.entryCount; i++)
	{
		const BindGroupLayout::Entry &layoutEntry = layoutDesc.entries[i];
		const BindGroup::Entry &entry = desc.entries[i];

		ASSERT(entry.binding == layoutEntry.binding);
		if (i > 0)
			ASSERT(desc.entries[i - 1].binding < entry.binding);
		ASSERT(entry.arrayIndex < layoutEntry.count);

		if (layoutEntry.type == BindGroupLayout::BindingType::UNIFORM_BUFFER ||
		    layoutEntry.type == BindGroupLayout::BindingType::STORAGE_BUFFER ||
		    layoutEntry.type == BindGroupLayout::BindingType::READONLY_STORAGE_BUFFER)
		{
			ASSERT(entry.buffer.isValid());
			ASSERT(entry.textureView.isValid() == false);
			if (layoutEntry.dynamicOffset == true)
				ASSERT_MSG(entry.offset == 0, "The binding offset should be zero if using a dynamic offset");
		}
		else if (layoutEntry.type == BindGroupLayout::BindingType::COMBINED_TEXTURE_SAMPLER)
		{
			ASSERT_MSG(layoutEntry.dynamicOffset == false, "Dynamic offset is only valid with buffers");
			ASSERT(entry.buffer.isValid() == false);
			ASSERT(entry.textureView.isValid());
		}

		if (entry.buffer.isValid())
		{
			Device::BackendData::BufferData &bufferData = bck.bufferData_[entry.buffer.index()];

			VkDescriptorBufferInfo &bufferInfo = bufferInfos[bufferInfoIndex];
			bufferInfo.buffer = bufferData.buffer;
			bufferInfo.offset = layoutEntry.dynamicOffset ? 0 : entry.offset;
			bufferInfo.range = entry.size;

			VkWriteDescriptorSet &descriptorWrite = descriptorWrites[descWriteIndex];
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = data.descriptorSet;
			descriptorWrite.dstBinding = entry.binding;
			descriptorWrite.dstArrayElement = entry.arrayIndex;

			VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			if (layoutEntry.type == BindGroupLayout::BindingType::UNIFORM_BUFFER)
			{
				descriptorType = layoutEntry.dynamicOffset
				        ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
				        : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			}
			else if (layoutEntry.type == BindGroupLayout::BindingType::STORAGE_BUFFER ||
			         layoutEntry.type == BindGroupLayout::BindingType::READONLY_STORAGE_BUFFER)
			{
				descriptorType = layoutEntry.dynamicOffset
				        ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
				        : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			}

			descriptorWrite.descriptorType = descriptorType;
			descriptorWrite.descriptorCount = 1;

			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;

			bufferInfoIndex++;
			descWriteIndex++;
		}
		else if (entry.textureView.isValid())
		{
			BackendData::TextureViewData &textureViewData = bck.textureViewData_[entry.textureView.index()];
			const TextureView::Desc &textureView = textureViewDesc(entry.textureView);

			ASSERT(textureView.texture.isValid());
			const BackendData::TextureData &textureData = bck.textureData_[textureView.texture.index()];

			VkDescriptorImageInfo &imageInfo = imageInfos[imageInfoIndex];
			imageInfo.imageLayout = grlTextureStateToVkImageLayout(textureData.textureState);
			imageInfo.imageView = textureViewData.imageView;
			imageInfo.sampler = textureViewData.sampler;

			VkWriteDescriptorSet &descriptorWrite = descriptorWrites[descWriteIndex];
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = data.descriptorSet;
			descriptorWrite.dstBinding = entry.binding;
			descriptorWrite.dstArrayElement = entry.arrayIndex;

			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = 1;

			descriptorWrite.pBufferInfo = nullptr;
			descriptorWrite.pImageInfo = &imageInfo;
			descriptorWrite.pTexelBufferView = nullptr;

			imageInfoIndex++;
			descWriteIndex++;
		}
	}

	vkUpdateDescriptorSets(bck.device_, descWriteIndex, descriptorWrites, 0, nullptr);

	return true;
}

void Device::destroyBindGroupImpl(uint32_t index)
{
	BackendData &bck = *backendData_;
	BackendData::BindGroupData &data = bck.bindGroupData_[index];

	ASSERT(data.descriptorSet != VK_NULL_HANDLE && data.descriptorPool != VK_NULL_HANDLE);
	// Cannot free individual descriptor set without the `VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT` flag
	data.descriptorSet = VK_NULL_HANDLE;
	data.descriptorPool = VK_NULL_HANDLE;
}

} // namespace grail
} // namespace ncine
