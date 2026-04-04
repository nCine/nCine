#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include <nctl/algorithms.h> // for `nctl::clamp()`
#include "grail/private/Device_frontend.h"
#include "grail/vulkan/Device_backend.h"
#include "grail/vulkan/vlk_utils.h"

namespace ncine {
namespace grail {

namespace {

Swapchain::AcquireResult vlkResultToAcquireResult(VkResult result)
{
	switch (result)
	{
		case VK_SUCCESS: return Swapchain::AcquireResult::SUCCESS;
		case VK_SUBOPTIMAL_KHR: return Swapchain::AcquireResult::SUBOPTIMAL;
		case VK_ERROR_OUT_OF_DATE_KHR: return Swapchain::AcquireResult::OUT_OF_DATE;

		default: return Swapchain::AcquireResult::UNKNOWN;
	}
}

}

///////////////////////////////////////////////////////////
// Swapchain::BackendData
///////////////////////////////////////////////////////////

void Swapchain::BackendData::populateSwapchainCreateInfo(VkSwapchainCreateInfoKHR &createInfo)
{
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = settings_.surface;
	createInfo.minImageCount = settings_.imageCount;
	createInfo.imageFormat = settings_.surfaceFormat.format;
	createInfo.imageColorSpace = settings_.surfaceFormat.colorSpace;
	createInfo.imageExtent = settings_.extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;
	createInfo.preTransform = supportDetails_.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = settings_.presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
}

void Swapchain::BackendData::updateDesc(Desc &desc)
{
	desc.format = vlkFormatToGrl(settings_.surfaceFormat.format);
	desc.width = settings_.extent.width;
	desc.height = settings_.extent.height;
	desc.textureCount = settings_.imageCount;
	desc.vsync = (settings_.presentMode == VK_PRESENT_MODE_FIFO_KHR);
}

bool Swapchain::BackendData::canBeCreated(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const Desc &desc)
{
	// Query swapchain support
	supportDetails_.physicalDevice = physicalDevice;
	supportDetails_.surface = surface;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &supportDetails_.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		supportDetails_.surfaceFormats.setSize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, supportDetails_.surfaceFormats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		supportDetails_.presentModes.setSize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, supportDetails_.presentModes.data());
	}

	const bool success = !supportDetails_.surfaceFormats.isEmpty() && !supportDetails_.presentModes.isEmpty();

	settings_.physicalDevice = VK_NULL_HANDLE;
	settings_.surface = VK_NULL_HANDLE;
	if (success)
	{
		// Choose swapchain settings
		const VkSurfaceCapabilitiesKHR &caps = supportDetails_.capabilities;

		VkSurfaceFormatKHR surfaceFormat{ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		VkFormat format = grlFormatToVk(desc.format);
		if (format != VK_FORMAT_UNDEFINED)
			surfaceFormat.format = format;

		VkExtent2D extent{ desc.width, desc.height };
		if (extent.width == 0 || extent.height == 0)
			extent = caps.currentExtent;

		uint32_t imageCount = desc.textureCount;
		if (imageCount == 0)
			imageCount = caps.minImageCount + 1;
		else if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount) // maxImageCount can be 0 for no hard limit
			imageCount = caps.maxImageCount;
		else if (imageCount < caps.minImageCount)
			imageCount = caps.minImageCount;

		settings_.physicalDevice = supportDetails_.physicalDevice;
		settings_.surface = supportDetails_.surface;
		settings_.surfaceFormat = surfaceFormat;
		settings_.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		settings_.extent = caps.currentExtent;
		settings_.imageCount = imageCount;

		bool surfaceFormatFound = false;
		for (const auto &supportedSurfaceFormat : supportDetails_.surfaceFormats)
		{
			if (supportedSurfaceFormat.format == surfaceFormat.format &&
			    supportedSurfaceFormat.colorSpace == surfaceFormat.colorSpace)
			{
				surfaceFormatFound = true;
				break;
			}
		}
		if (surfaceFormatFound == false)
			return false;

		// The `VK_PRESENT_MODE_FIFO_KHR` present mode is always available
		if (desc.vsync == false)
		{
			bool hasMailbox = false;
			bool hasImmediate = false;

			for (const auto &presentMode : supportDetails_.presentModes)
			{
				if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
					hasMailbox = true;
				else if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
					hasImmediate = true;
			}

			if (hasMailbox)
				settings_.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			else if (hasImmediate)
				settings_.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}

		if (caps.currentExtent.width == uint32_t(-1))
		{
			extent.width = nctl::clamp(extent.width, caps.minImageExtent.width, caps.maxImageExtent.width);
			extent.height = nctl::clamp(extent.height, caps.minImageExtent.height, caps.maxImageExtent.height);
			settings_.extent = extent;
		}
	}

	return success;
}

bool Swapchain::BackendData::createImagesAndViews(const Desc &desc)
{
	ASSERT(swapchain_ != VK_NULL_HANDLE);
	if (swapchain_ == VK_NULL_HANDLE)
		return false;

	Device::BackendData &devBck = *theDevice_->backendData_;
	Device::FrontendData &devFrn = *theDevice_->frontendData_;

	VkImage images[MaxTextures];
	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(devBck.device_, swapchain_, &imageCount, nullptr);
	vkGetSwapchainImagesKHR(devBck.device_, swapchain_, &imageCount, images);

	Texture::Desc textureDesc;
	textureDesc.type = Texture::Type::TEXTURE_2D;
	textureDesc.width = desc.width;
	textureDesc.height = desc.height;
	textureDesc.depth = 1;
	textureDesc.mipLevels = 1;
	textureDesc.arrayLayers = 1;
	textureDesc.format = desc.format;
	textureDesc.usage = Texture::Usage::COLOR_TARGET;
	textureDesc.sampleCount = Texture::SampleCount::SAMPLE_1;

	TextureView::Desc textureViewDesc;
	textureViewDesc.format = vlkFormatToGrl(settings_.surfaceFormat.format);
	textureViewDesc.baseMip = 0;
	textureViewDesc.mipCount = 1;
	textureViewDesc.baseLayer = 0;
	textureViewDesc.layerCount = 1;
	textureViewDesc.type = TextureView::Type::TEXTURE_2D;

	for (uint32_t i = 0; i < imageCount; i++)
	{
		Texture::Handle textureHandle = devFrn.createTextureHandle(textureDesc);
		Device::BackendData::TextureData &textureData = devBck.textureData_[textureHandle.index()];
		textureData.image = images[i];
		textureData.allocation = VK_NULL_HANDLE;
		textureData.textureState = Texture::State::UNDEFINED;
		frontend_->textures_.pushBack(textureHandle);

		textureViewDesc.texture = textureHandle;
		TextureView::Handle textureViewHandle = theDevice_->createTextureView(textureViewDesc);

		frontend_->textureViews_.pushBack(textureViewHandle);

		presentSemaphores[i] = devBck.createSemaphore();
	}

	return true;
}

bool Swapchain::BackendData::create(const Desc &createDesc, Desc &desc)
{
	ASSERT(swapchain_ == VK_NULL_HANDLE);
	if (swapchain_ != VK_NULL_HANDLE)
		return false;

	Device::BackendData &devBck = *theDevice_->backendData_;

	// Check if cached swapchain settings are still relevant
	if (devBck.physicalDevice_ != settings_.physicalDevice || devBck.surface_ != settings_.surface)
	{
		ASSERT(devBck.physicalDevice_ != VK_NULL_HANDLE);
		ASSERT(devBck.surface_ != VK_NULL_HANDLE);

		const bool success = canBeCreated(devBck.physicalDevice_, devBck.surface_, createDesc);
		if (success == false)
			return false;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	populateSwapchainCreateInfo(createInfo);

	const VkResult result = vkCreateSwapchainKHR(devBck.device_, &createInfo, devBck.allocator_, &swapchain_);
	vlkFatalAssert(result);

	updateDesc(desc);

	const bool created = createImagesAndViews(desc);
	return created;
}

void Swapchain::BackendData::destroy()
{
	if (swapchain_ != VK_NULL_HANDLE)
	{
		destroyImagesAndViews();

		Device::BackendData &devBck = *theDevice_->backendData_;
		vkDestroySwapchainKHR(devBck.device_, swapchain_, devBck.allocator_);
		swapchain_ = VK_NULL_HANDLE;
	}
}

void Swapchain::BackendData::destroyImagesAndViews()
{
	ASSERT(swapchain_ != VK_NULL_HANDLE);
	Device::BackendData &devBck = *theDevice_->backendData_;

	const uint32_t numTextures = frontend_->textures_.size();
	ASSERT(numTextures > 0);

	for (uint32_t i = 0; i < numTextures; i++)
	{
		theDevice_->destroyTextureView(frontend_->textureViews_[i]);
		devBck.destroySemaphore(presentSemaphores[i]);
	}

	frontend_->textures_.clear();
	frontend_->textureViews_.clear();
}

///////////////////////////////////////////////////////////
// Swapchain
///////////////////////////////////////////////////////////

Swapchain::AcquireDesc Swapchain::acquireNextTextureView()
{
	BackendData &bck = *backendData_;
	Device::BackendData &devBck = *backendData_->theDevice_->backendData_;

	devBck.beginFrameContext();
	Device::BackendData::FrameContext &frameContext = devBck.frameContexts_[devBck.currentFrameIndex];
	VkSemaphore semaphore = frameContext.acquireSemaphore;

	const VkResult result = vkAcquireNextImageKHR(devBck.device_, bck.swapchain_, UINT64_MAX, semaphore, VK_NULL_HANDLE, &bck.currentImageIndex_);

	frameContext.swapchainHasAcquired = (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
		return { AcquireResult::OUT_OF_DATE, 0, TextureView::Handle() };

	return AcquireDesc{ vlkResultToAcquireResult(result), bck.currentImageIndex_, textureViews_[bck.currentImageIndex_] };
}

bool Swapchain::present()
{
	BackendData &bck = *backendData_;
	Device::BackendData &devBck = *backendData_->theDevice_->backendData_;
	Device::BackendData::FrameContext &frameContext = devBck.frameContexts_[devBck.currentFrameIndex];

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	VkSemaphore presentSemaphore = bck.presentSemaphores[bck.currentImageIndex_];
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &presentSemaphore;

	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &backendData_->swapchain_;
	presentInfo.pImageIndices = &bck.currentImageIndex_;
	presentInfo.pResults = nullptr;
	const VkResult result = vkQueuePresentKHR(devBck.graphicsQueue_.queue, &presentInfo);

	frameContext.swapchainHasAcquired = false;
	devBck.endFrameContext();

	return (result == VK_SUCCESS);
}

} // namespace grail
} // namespace ncine
