#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include <nctl/String.h>
#include "grail/Device.h"
#include "grail/private/Device_frontend.h"
#include "grail/vulkan/Device_backend.h"
#include "grail/vulkan/vlk_utils.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Device::Device(InternalTag)
{
}

Device::~Device() = default;

Device::Device(Device &&) noexcept = default;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

Device Device::create(const CreateDesc &desc)
{
	Device device(InternalTag{});
	device.frontendData_ = nctl::makeUnique<FrontendData>();
	device.backendData_ = nctl::makeUnique<BackendData>();

	BackendData &bck = *device.backendData_;
	const bool instanceCreated = bck.createInstance(desc, device.desc_);
	FATAL_ASSERT(instanceCreated == true);
	const bool surfaceCreated = bck.createSurface();
	FATAL_ASSERT(surfaceCreated == true);
	const bool deviceCreated = bck.createDevice(desc, device.desc_);
	FATAL_ASSERT(deviceCreated == true);

	bck.createBackendData(device);

	return device;
}

void Device::destroy()
{
	waitIdle();

	BackendData &bck = *backendData_;
	bck.destroyBackendData(*this);

	destroySwapchainImpl(swapchain_);

	bck.destroyDevice();
	bck.destroySurface();
	bck.destroyInstance();

	backendData_.reset(nullptr);
	frontendData_.reset(nullptr);
}

Format Device::findDepthFormat(const DepthFormatQuery &query)
{
	return findDepthFormatImpl(query);
}

Swapchain &Device::createSwapchain(const Swapchain::Desc &desc)
{
	// Prevent double creation
	ASSERT(swapchain_.backendData_ == nullptr);
	if (swapchain_.backendData_ != nullptr)
		return swapchain_;

	swapchain_.backendData_ = nctl::makeUnique<Swapchain::BackendData>();

	const bool success = createSwapchainImpl(swapchain_, desc);
	ASSERT(success == true);

	swapchain_.desc_ = desc;

	return swapchain_;
}

Swapchain &Device::swapchain()
{
	FATAL_ASSERT_MSG(swapchain_.backendData_ != nullptr, "Swapchain has not been created yet");
	return swapchain_;
}

void Device::waitIdle()
{
	BackendData &bck = *backendData_;
	const VkResult result = vkDeviceWaitIdle(bck.device_);
	vlkFatalAssert(result);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool Device::createSwapchainImpl(Swapchain &swapchain, const Swapchain::Desc &desc)
{
	swapchain.backendData_->theDevice_ = this;
	swapchain.backendData_->frontend_ = &swapchain;
	return swapchain.backendData_->create(desc, swapchain.desc_);
}

void Device::destroySwapchainImpl(Swapchain &swapchain)
{
	ASSERT(swapchain.backendData_ != nullptr);

	if (swapchain.backendData_ != nullptr)
	{
		swapchain.backendData_->destroy();
		swapchain.backendData_.reset(nullptr);
	}
}

} // namespace grail
} // namespace ncine
