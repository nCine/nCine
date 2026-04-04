#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include <nctl/String.h>
#include "grail/Device.h"
#include "grail/vulkan/Device_backend.h"

#include "Application.h"
#include "IGfxDevice.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// Device::BackendData
///////////////////////////////////////////////////////////

bool Device::BackendData::createSurface()
{
	ASSERT(surface_ == VK_NULL_HANDLE);
	if (surface_ != VK_NULL_HANDLE)
		return false;

	ASSERT(instance_ != VK_NULL_HANDLE);
	if (instance_ == VK_NULL_HANDLE)
		return false;

	const bool result = theApplication().gfxDevice().createVulkanSurface(instance_, allocator_, &surface_);
	FATAL_ASSERT_MSG(result == true, "Cannot create the Vulkan presentation surface");

	return true;
}

void Device::BackendData::destroySurface()
{
	if (surface_ != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(instance_, surface_, allocator_);
		surface_ = VK_NULL_HANDLE;
	}
}

} // namespace grail
} // namespace ncine
