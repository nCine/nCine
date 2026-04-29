#include "grail/vulkan/vlk_utils.h"
#include "common_macros.h"

namespace ncine {
namespace grail {

namespace {

const char *messageTypeToString(VkDebugUtilsMessageTypeFlagsEXT messageType)
{
	switch (messageType)
	{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: return "general";
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: return "validation";
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: return "performance";
		case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT: return "device address binding";

		default: return "general";
	}
}

void setObjectName(VkDevice device, VkObjectType objectType, uint64_t objectHandle, const char *name)
{
	if (device == VK_NULL_HANDLE || objectHandle == 0)
		return;

	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = objectType;
	nameInfo.objectHandle = objectHandle;
	nameInfo.pObjectName = name;
	vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
}

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

const char *vlkResultToString(VkResult result)
{
	switch (result)
	{
		case VK_SUCCESS: return "VK_SUCCESS";
		case VK_NOT_READY: return "VK_NOT_READY";
		case VK_TIMEOUT: return "VK_TIMEOUT";
		case VK_EVENT_SET: return "VK_EVENT_SET";
		case VK_EVENT_RESET: return "VK_EVENT_RESET";
		case VK_INCOMPLETE: return "VK_INCOMPLETE";

		case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
		case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
		case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
		case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
		case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
		case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
		case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
		case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
		case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
		case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
		case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
		case VK_ERROR_UNKNOWN: return "VK_ERROR_UNKNOWN";

		case VK_ERROR_VALIDATION_FAILED: return "VK_ERROR_VALIDATION_FAILED";
		case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
		case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
		case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
		case VK_ERROR_FRAGMENTATION: return "VK_ERROR_FRAGMENTATION";
		case VK_PIPELINE_COMPILE_REQUIRED: return "VK_PIPELINE_COMPILE_REQUIRED";
		case VK_ERROR_NOT_PERMITTED: return "VK_ERROR_NOT_PERMITTED";

		case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
		case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";

		case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";

		case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR: return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
		case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
		case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
		case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
		case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
		case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";

		case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
		case VK_ERROR_PRESENT_TIMING_QUEUE_FULL_EXT: return "VK_ERROR_PRESENT_TIMING_QUEUE_FULL_EXT";
		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";

		case VK_THREAD_IDLE_KHR: return "VK_THREAD_IDLE_KHR";
		case VK_THREAD_DONE_KHR: return "VK_THREAD_DONE_KHR";
		case VK_OPERATION_DEFERRED_KHR: return "VK_OPERATION_DEFERRED_KHR";
		case VK_OPERATION_NOT_DEFERRED_KHR: return "VK_OPERATION_NOT_DEFERRED_KHR";

		case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR: return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
		case VK_ERROR_COMPRESSION_EXHAUSTED_EXT: return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";

		case VK_INCOMPATIBLE_SHADER_BINARY_EXT: return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";

		case VK_PIPELINE_BINARY_MISSING_KHR: return "VK_PIPELINE_BINARY_MISSING_KHR";
		case VK_ERROR_NOT_ENOUGH_SPACE_KHR: return "VK_ERROR_NOT_ENOUGH_SPACE_KHR";

		default: return "VK_UNKNOWN_RESULT";
	}
}

void vlkAssert(VkResult result)
{
	ASSERT_MSG_X(result == VK_SUCCESS, "VkResult is %s", vlkResultToString(result));
}

void vlkFatalAssert(VkResult result)
{
	FATAL_ASSERT_MSG_X(result == VK_SUCCESS, "VkResult is %s", vlkResultToString(result));
}

VKAPI_ATTR VkBool32 VKAPI_CALL vlkDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
	ncine::ILogger &log = ncine::theServiceLocator().logger();

	switch (messageSeverity)
	{
		default:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			log.write(ncine::ILogger::LogLevel::VERBOSE, "Vulkan msg #%d - %s (%s): %s", pCallbackData->messageIdNumber, pCallbackData->pMessageIdName, messageTypeToString(messageType), pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			log.write(ncine::ILogger::LogLevel::INFO, "Vulkan msg #%d - %s (%s): %s", pCallbackData->messageIdNumber, pCallbackData->pMessageIdName, messageTypeToString(messageType), pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			log.write(ncine::ILogger::LogLevel::WARN, "Vulkan msg #%d - %s (%s): %s", pCallbackData->messageIdNumber, pCallbackData->pMessageIdName, messageTypeToString(messageType), pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			log.write(ncine::ILogger::LogLevel::ERROR, "Vulkan msg #%d - %s (%s): %s", pCallbackData->messageIdNumber, pCallbackData->pMessageIdName, messageTypeToString(messageType), pCallbackData->pMessage);
			break;
	}

	return VK_FALSE;
}

void setObjectName(VkDevice device, VkInstance instanceHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_INSTANCE, uint64_t(instanceHandle), name);
}

void setObjectName(VkDevice device, VkPhysicalDevice physicalDeviceHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_PHYSICAL_DEVICE, uint64_t(physicalDeviceHandle), name);
}

void setObjectName(VkDevice device, VkDevice deviceHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_DEVICE, uint64_t(deviceHandle), name);
}

void setObjectName(VkDevice device, VkQueue queueHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_QUEUE, uint64_t(queueHandle), name);
}

void setObjectName(VkDevice device, VkSemaphore semaphoreHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_SEMAPHORE, uint64_t(semaphoreHandle), name);
}

void setObjectName(VkDevice device, VkCommandBuffer commandBufferHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_COMMAND_BUFFER, uint64_t(commandBufferHandle), name);
}

void setObjectName(VkDevice device, VkFence fenceHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_FENCE, uint64_t(fenceHandle), name);
}

void setObjectName(VkDevice device, VkBuffer bufferHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_BUFFER, uint64_t(bufferHandle), name);
}

void setObjectName(VkDevice device, VkImage imageHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_IMAGE, uint64_t(imageHandle), name);
}

void setObjectName(VkDevice device, VkImageView imageViewHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_IMAGE_VIEW, uint64_t(imageViewHandle), name);
}

void setObjectName(VkDevice device, VkShaderModule shaderModuleHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_SHADER_MODULE, uint64_t(shaderModuleHandle), name);
}

void setObjectName(VkDevice device, VkRenderPass renderPassHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_RENDER_PASS, uint64_t(renderPassHandle), name);
}

void setObjectName(VkDevice device, VkPipeline pipelineHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_PIPELINE, uint64_t(pipelineHandle), name);
}

void setObjectName(VkDevice device, VkDescriptorSetLayout descriptorSetLayoutHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, uint64_t(descriptorSetLayoutHandle), name);
}

void setObjectName(VkDevice device, VkSampler samplerHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_SAMPLER, uint64_t(samplerHandle), name);
}

void setObjectName(VkDevice device, VkDescriptorPool descriptorPoolHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_DESCRIPTOR_POOL, uint64_t(descriptorPoolHandle), name);
}

void setObjectName(VkDevice device, VkDescriptorSet descriptorSetHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_DESCRIPTOR_SET, uint64_t(descriptorSetHandle), name);
}

void setObjectName(VkDevice device, VkCommandPool commandPoolHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_COMMAND_POOL, uint64_t(commandPoolHandle), name);
}

void setObjectName(VkDevice device, VkSurfaceKHR surfaceHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_SURFACE_KHR, uint64_t(surfaceHandle), name);
}

void setObjectName(VkDevice device, VkSwapchainKHR swapchainHandle, const char *name)
{
	setObjectName(device, VK_OBJECT_TYPE_SWAPCHAIN_KHR, uint64_t(swapchainHandle), name);
}

VkFormat grlFormatToVk(Format format)
{
	switch (format)
	{
		case Format::UNDEFINED: return VK_FORMAT_UNDEFINED;

		case Format::R4G4_UNORM_PACK8: return VK_FORMAT_R4G4_UNORM_PACK8;
		case Format::R4G4B4A4_UNORM_PACK16: return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
		case Format::B4G4R4A4_UNORM_PACK16: return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
		case Format::R5G6B5_UNORM_PACK16: return VK_FORMAT_R5G6B5_UNORM_PACK16;
		case Format::B5G6R5_UNORM_PACK16: return VK_FORMAT_B5G6R5_UNORM_PACK16;
		case Format::R5G5B5A1_UNORM_PACK16: return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
		case Format::B5G5R5A1_UNORM_PACK16: return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
		case Format::A1R5G5B5_UNORM_PACK16: return VK_FORMAT_A1R5G5B5_UNORM_PACK16;

		case Format::R8_UNORM: return VK_FORMAT_R8_UNORM;
		case Format::R8_SNORM: return VK_FORMAT_R8_SNORM;
		case Format::R8_USCALED: return VK_FORMAT_R8_USCALED;
		case Format::R8_SSCALED: return VK_FORMAT_R8_SSCALED;
		case Format::R8_UINT: return VK_FORMAT_R8_UINT;
		case Format::R8_SINT: return VK_FORMAT_R8_SINT;
		case Format::R8_SRGB: return VK_FORMAT_R8_SRGB;

		case Format::R8G8_UNORM: return VK_FORMAT_R8G8_UNORM;
		case Format::R8G8_SNORM: return VK_FORMAT_R8G8_SNORM;
		case Format::R8G8_USCALED: return VK_FORMAT_R8G8_USCALED;
		case Format::R8G8_SSCALED: return VK_FORMAT_R8G8_SSCALED;
		case Format::R8G8_UINT: return VK_FORMAT_R8G8_UINT;
		case Format::R8G8_SINT: return VK_FORMAT_R8G8_SINT;
		case Format::R8G8_SRGB: return VK_FORMAT_R8G8_SRGB;

		case Format::R8G8B8_UNORM: return VK_FORMAT_R8G8B8_UNORM;
		case Format::R8G8B8_SNORM: return VK_FORMAT_R8G8B8_SNORM;
		case Format::R8G8B8_USCALED: return VK_FORMAT_R8G8B8_USCALED;
		case Format::R8G8B8_SSCALED: return VK_FORMAT_R8G8B8_SSCALED;
		case Format::R8G8B8_UINT: return VK_FORMAT_R8G8B8_UINT;
		case Format::R8G8B8_SINT: return VK_FORMAT_R8G8B8_SINT;
		case Format::R8G8B8_SRGB: return VK_FORMAT_R8G8B8_SRGB;

		case Format::B8G8R8_UNORM: return VK_FORMAT_B8G8R8_UNORM;
		case Format::B8G8R8_SNORM: return VK_FORMAT_B8G8R8_SNORM;
		case Format::B8G8R8_USCALED: return VK_FORMAT_B8G8R8_USCALED;
		case Format::B8G8R8_SSCALED: return VK_FORMAT_B8G8R8_SSCALED;
		case Format::B8G8R8_UINT: return VK_FORMAT_B8G8R8_UINT;
		case Format::B8G8R8_SINT: return VK_FORMAT_B8G8R8_SINT;
		case Format::B8G8R8_SRGB: return VK_FORMAT_B8G8R8_SRGB;

		case Format::R8G8B8A8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
		case Format::R8G8B8A8_SNORM: return VK_FORMAT_R8G8B8A8_SNORM;
		case Format::R8G8B8A8_USCALED: return VK_FORMAT_R8G8B8A8_USCALED;
		case Format::R8G8B8A8_SSCALED: return VK_FORMAT_R8G8B8A8_SSCALED;
		case Format::R8G8B8A8_UINT: return VK_FORMAT_R8G8B8A8_UINT;
		case Format::R8G8B8A8_SINT: return VK_FORMAT_R8G8B8A8_SINT;
		case Format::R8G8B8A8_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;

		case Format::B8G8R8A8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM;
		case Format::B8G8R8A8_SNORM: return VK_FORMAT_B8G8R8A8_SNORM;
		case Format::B8G8R8A8_USCALED: return VK_FORMAT_B8G8R8A8_USCALED;
		case Format::B8G8R8A8_SSCALED: return VK_FORMAT_B8G8R8A8_SSCALED;
		case Format::B8G8R8A8_UINT: return VK_FORMAT_B8G8R8A8_UINT;
		case Format::B8G8R8A8_SINT: return VK_FORMAT_B8G8R8A8_SINT;
		case Format::B8G8R8A8_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;

		case Format::A8B8G8R8_UNORM_PACK32: return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
		case Format::A8B8G8R8_SNORM_PACK32: return VK_FORMAT_A8B8G8R8_SNORM_PACK32;
		case Format::A8B8G8R8_USCALED_PACK32: return VK_FORMAT_A8B8G8R8_USCALED_PACK32;
		case Format::A8B8G8R8_SSCALED_PACK32: return VK_FORMAT_A8B8G8R8_SSCALED_PACK32;
		case Format::A8B8G8R8_UINT_PACK32: return VK_FORMAT_A8B8G8R8_UINT_PACK32;
		case Format::A8B8G8R8_SINT_PACK32: return VK_FORMAT_A8B8G8R8_SINT_PACK32;
		case Format::A8B8G8R8_SRGB_PACK32: return VK_FORMAT_A8B8G8R8_SRGB_PACK32;

		case Format::A2R10G10B10_UNORM_PACK32: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
		case Format::A2R10G10B10_SNORM_PACK32: return VK_FORMAT_A2R10G10B10_SNORM_PACK32;
		case Format::A2R10G10B10_USCALED_PACK32: return VK_FORMAT_A2R10G10B10_USCALED_PACK32;
		case Format::A2R10G10B10_SSCALED_PACK32: return VK_FORMAT_A2R10G10B10_SSCALED_PACK32;
		case Format::A2R10G10B10_UINT_PACK32: return VK_FORMAT_A2R10G10B10_UINT_PACK32;
		case Format::A2R10G10B10_SINT_PACK32: return VK_FORMAT_A2R10G10B10_SINT_PACK32;

		case Format::A2B10G10R10_UNORM_PACK32: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
		case Format::A2B10G10R10_SNORM_PACK32: return VK_FORMAT_A2B10G10R10_SNORM_PACK32;
		case Format::A2B10G10R10_USCALED_PACK32: return VK_FORMAT_A2B10G10R10_USCALED_PACK32;
		case Format::A2B10G10R10_SSCALED_PACK32: return VK_FORMAT_A2B10G10R10_SSCALED_PACK32;
		case Format::A2B10G10R10_UINT_PACK32: return VK_FORMAT_A2B10G10R10_UINT_PACK32;
		case Format::A2B10G10R10_SINT_PACK32: return VK_FORMAT_A2B10G10R10_SINT_PACK32;

		case Format::R16_UNORM: return VK_FORMAT_R16_UNORM;
		case Format::R16_SNORM: return VK_FORMAT_R16_SNORM;
		case Format::R16_USCALED: return VK_FORMAT_R16_USCALED;
		case Format::R16_SSCALED: return VK_FORMAT_R16_SSCALED;
		case Format::R16_UINT: return VK_FORMAT_R16_UINT;
		case Format::R16_SINT: return VK_FORMAT_R16_SINT;
		case Format::R16_SFLOAT: return VK_FORMAT_R16_SFLOAT;

		case Format::R16G16_UNORM: return VK_FORMAT_R16G16_UNORM;
		case Format::R16G16_SNORM: return VK_FORMAT_R16G16_SNORM;
		case Format::R16G16_USCALED: return VK_FORMAT_R16G16_USCALED;
		case Format::R16G16_SSCALED: return VK_FORMAT_R16G16_SSCALED;
		case Format::R16G16_UINT: return VK_FORMAT_R16G16_UINT;
		case Format::R16G16_SINT: return VK_FORMAT_R16G16_SINT;
		case Format::R16G16_SFLOAT: return VK_FORMAT_R16G16_SFLOAT;

		case Format::R16G16B16_UNORM: return VK_FORMAT_R16G16B16_UNORM;
		case Format::R16G16B16_SNORM: return VK_FORMAT_R16G16B16_SNORM;
		case Format::R16G16B16_USCALED: return VK_FORMAT_R16G16B16_USCALED;
		case Format::R16G16B16_SSCALED: return VK_FORMAT_R16G16B16_SSCALED;
		case Format::R16G16B16_UINT: return VK_FORMAT_R16G16B16_UINT;
		case Format::R16G16B16_SINT: return VK_FORMAT_R16G16B16_SINT;
		case Format::R16G16B16_SFLOAT: return VK_FORMAT_R16G16B16_SFLOAT;

		case Format::R16G16B16A16_UNORM: return VK_FORMAT_R16G16B16A16_UNORM;
		case Format::R16G16B16A16_SNORM: return VK_FORMAT_R16G16B16A16_SNORM;
		case Format::R16G16B16A16_USCALED: return VK_FORMAT_R16G16B16A16_USCALED;
		case Format::R16G16B16A16_SSCALED: return VK_FORMAT_R16G16B16A16_SSCALED;
		case Format::R16G16B16A16_UINT: return VK_FORMAT_R16G16B16A16_UINT;
		case Format::R16G16B16A16_SINT: return VK_FORMAT_R16G16B16A16_SINT;
		case Format::R16G16B16A16_SFLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;

		case Format::R32_UINT: return VK_FORMAT_R32_UINT;
		case Format::R32_SINT: return VK_FORMAT_R32_SINT;
		case Format::R32_SFLOAT: return VK_FORMAT_R32_SFLOAT;

		case Format::R32G32_UINT: return VK_FORMAT_R32G32_UINT;
		case Format::R32G32_SINT: return VK_FORMAT_R32G32_SINT;
		case Format::R32G32_SFLOAT: return VK_FORMAT_R32G32_SFLOAT;

		case Format::R32G32B32_UINT: return VK_FORMAT_R32G32B32_UINT;
		case Format::R32G32B32_SINT: return VK_FORMAT_R32G32B32_SINT;
		case Format::R32G32B32_SFLOAT: return VK_FORMAT_R32G32B32_SFLOAT;

		case Format::R32G32B32A32_UINT: return VK_FORMAT_R32G32B32A32_UINT;
		case Format::R32G32B32A32_SINT: return VK_FORMAT_R32G32B32A32_SINT;
		case Format::R32G32B32A32_SFLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;

		case Format::R64_UINT: return VK_FORMAT_R64_UINT;
		case Format::R64_SINT: return VK_FORMAT_R64_SINT;
		case Format::R64_SFLOAT: return VK_FORMAT_R64_SFLOAT;

		case Format::R64G64_UINT: return VK_FORMAT_R64G64_UINT;
		case Format::R64G64_SINT: return VK_FORMAT_R64G64_SINT;
		case Format::R64G64_SFLOAT: return VK_FORMAT_R64G64_SFLOAT;

		case Format::R64G64B64_UINT: return VK_FORMAT_R64G64B64_UINT;
		case Format::R64G64B64_SINT: return VK_FORMAT_R64G64B64_SINT;
		case Format::R64G64B64_SFLOAT: return VK_FORMAT_R64G64B64_SFLOAT;

		case Format::R64G64B64A64_UINT: return VK_FORMAT_R64G64B64A64_UINT;
		case Format::R64G64B64A64_SINT: return VK_FORMAT_R64G64B64A64_SINT;
		case Format::R64G64B64A64_SFLOAT: return VK_FORMAT_R64G64B64A64_SFLOAT;

		case Format::B10G11R11_UFLOAT_PACK32: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
		case Format::E5B9G9R9_UFLOAT_PACK32: return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;

		case Format::D16_UNORM: return VK_FORMAT_D16_UNORM;
		case Format::X8_D24_UNORM_PACK32: return VK_FORMAT_X8_D24_UNORM_PACK32;
		case Format::D32_SFLOAT: return VK_FORMAT_D32_SFLOAT;
		case Format::S8_UINT: return VK_FORMAT_S8_UINT;
		case Format::D16_UNORM_S8_UINT: return VK_FORMAT_D16_UNORM_S8_UINT;
		case Format::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
		case Format::D32_SFLOAT_S8_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;

		default: return VK_FORMAT_UNDEFINED;
	}
}

VkFormat grlFormatToVk(GraphicsPipeline::VertexFormat format)
{
	using VertexFormat = GraphicsPipeline::VertexFormat;

	switch (format)
	{
		case VertexFormat::FLOAT: return VK_FORMAT_R32_SFLOAT;
		case VertexFormat::FLOAT2: return VK_FORMAT_R32G32_SFLOAT;
		case VertexFormat::FLOAT3: return VK_FORMAT_R32G32B32_SFLOAT;
		case VertexFormat::FLOAT4: return VK_FORMAT_R32G32B32A32_SFLOAT;

		case VertexFormat::UINT: return VK_FORMAT_R32_UINT;
		case VertexFormat::UINT2: return VK_FORMAT_R32G32_UINT;
		case VertexFormat::UINT3: return VK_FORMAT_R32G32B32_UINT;
		case VertexFormat::UINT4: return VK_FORMAT_R32G32B32A32_UINT;

		case VertexFormat::SINT: return VK_FORMAT_R32_SINT;
		case VertexFormat::SINT2: return VK_FORMAT_R32G32_SINT;
		case VertexFormat::SINT3: return VK_FORMAT_R32G32B32_SINT;
		case VertexFormat::SINT4: return VK_FORMAT_R32G32B32A32_SINT;

		case VertexFormat::UNORM4: return VK_FORMAT_R8G8B8A8_UNORM;

		default: return VK_FORMAT_UNDEFINED;
	}
}

Format vlkFormatToGrl(VkFormat format)
{
	switch (format)
	{
		case VK_FORMAT_UNDEFINED: return Format::UNDEFINED;

		case VK_FORMAT_R4G4_UNORM_PACK8: return Format::R4G4_UNORM_PACK8;
		case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return Format::R4G4B4A4_UNORM_PACK16;
		case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return Format::B4G4R4A4_UNORM_PACK16;
		case VK_FORMAT_R5G6B5_UNORM_PACK16: return Format::R5G6B5_UNORM_PACK16;
		case VK_FORMAT_B5G6R5_UNORM_PACK16: return Format::B5G6R5_UNORM_PACK16;
		case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return Format::R5G5B5A1_UNORM_PACK16;
		case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return Format::B5G5R5A1_UNORM_PACK16;
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return Format::A1R5G5B5_UNORM_PACK16;

		case VK_FORMAT_R8_UNORM: return Format::R8_UNORM;
		case VK_FORMAT_R8_SNORM: return Format::R8_SNORM;
		case VK_FORMAT_R8_USCALED: return Format::R8_USCALED;
		case VK_FORMAT_R8_SSCALED: return Format::R8_SSCALED;
		case VK_FORMAT_R8_UINT: return Format::R8_UINT;
		case VK_FORMAT_R8_SINT: return Format::R8_SINT;
		case VK_FORMAT_R8_SRGB: return Format::R8_SRGB;

		case VK_FORMAT_R8G8_UNORM: return Format::R8G8_UNORM;
		case VK_FORMAT_R8G8_SNORM: return Format::R8G8_SNORM;
		case VK_FORMAT_R8G8_USCALED: return Format::R8G8_USCALED;
		case VK_FORMAT_R8G8_SSCALED: return Format::R8G8_SSCALED;
		case VK_FORMAT_R8G8_UINT: return Format::R8G8_UINT;
		case VK_FORMAT_R8G8_SINT: return Format::R8G8_SINT;
		case VK_FORMAT_R8G8_SRGB: return Format::R8G8_SRGB;

		case VK_FORMAT_R8G8B8_UNORM: return Format::R8G8B8_UNORM;
		case VK_FORMAT_R8G8B8_SNORM: return Format::R8G8B8_SNORM;
		case VK_FORMAT_R8G8B8_USCALED: return Format::R8G8B8_USCALED;
		case VK_FORMAT_R8G8B8_SSCALED: return Format::R8G8B8_SSCALED;
		case VK_FORMAT_R8G8B8_UINT: return Format::R8G8B8_UINT;
		case VK_FORMAT_R8G8B8_SINT: return Format::R8G8B8_SINT;
		case VK_FORMAT_R8G8B8_SRGB: return Format::R8G8B8_SRGB;

		case VK_FORMAT_B8G8R8_UNORM: return Format::B8G8R8_UNORM;
		case VK_FORMAT_B8G8R8_SNORM: return Format::B8G8R8_SNORM;
		case VK_FORMAT_B8G8R8_USCALED: return Format::B8G8R8_USCALED;
		case VK_FORMAT_B8G8R8_SSCALED: return Format::B8G8R8_SSCALED;
		case VK_FORMAT_B8G8R8_UINT: return Format::B8G8R8_UINT;
		case VK_FORMAT_B8G8R8_SINT: return Format::B8G8R8_SINT;
		case VK_FORMAT_B8G8R8_SRGB: return Format::B8G8R8_SRGB;

		case VK_FORMAT_R8G8B8A8_UNORM: return Format::R8G8B8A8_UNORM;
		case VK_FORMAT_R8G8B8A8_SNORM: return Format::R8G8B8A8_SNORM;
		case VK_FORMAT_R8G8B8A8_USCALED: return Format::R8G8B8A8_USCALED;
		case VK_FORMAT_R8G8B8A8_SSCALED: return Format::R8G8B8A8_SSCALED;
		case VK_FORMAT_R8G8B8A8_UINT: return Format::R8G8B8A8_UINT;
		case VK_FORMAT_R8G8B8A8_SINT: return Format::R8G8B8A8_SINT;
		case VK_FORMAT_R8G8B8A8_SRGB: return Format::R8G8B8A8_SRGB;

		case VK_FORMAT_B8G8R8A8_UNORM: return Format::B8G8R8A8_UNORM;
		case VK_FORMAT_B8G8R8A8_SNORM: return Format::B8G8R8A8_SNORM;
		case VK_FORMAT_B8G8R8A8_USCALED: return Format::B8G8R8A8_USCALED;
		case VK_FORMAT_B8G8R8A8_SSCALED: return Format::B8G8R8A8_SSCALED;
		case VK_FORMAT_B8G8R8A8_UINT: return Format::B8G8R8A8_UINT;
		case VK_FORMAT_B8G8R8A8_SINT: return Format::B8G8R8A8_SINT;
		case VK_FORMAT_B8G8R8A8_SRGB: return Format::B8G8R8A8_SRGB;

		case VK_FORMAT_A8B8G8R8_UNORM_PACK32: return Format::A8B8G8R8_UNORM_PACK32;
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32: return Format::A8B8G8R8_SNORM_PACK32;
		case VK_FORMAT_A8B8G8R8_USCALED_PACK32: return Format::A8B8G8R8_USCALED_PACK32;
		case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: return Format::A8B8G8R8_SSCALED_PACK32;
		case VK_FORMAT_A8B8G8R8_UINT_PACK32: return Format::A8B8G8R8_UINT_PACK32;
		case VK_FORMAT_A8B8G8R8_SINT_PACK32: return Format::A8B8G8R8_SINT_PACK32;
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32: return Format::A8B8G8R8_SRGB_PACK32;

		case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return Format::A2R10G10B10_UNORM_PACK32;
		case VK_FORMAT_A2R10G10B10_SNORM_PACK32: return Format::A2R10G10B10_SNORM_PACK32;
		case VK_FORMAT_A2R10G10B10_USCALED_PACK32: return Format::A2R10G10B10_USCALED_PACK32;
		case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: return Format::A2R10G10B10_SSCALED_PACK32;
		case VK_FORMAT_A2R10G10B10_UINT_PACK32: return Format::A2R10G10B10_UINT_PACK32;
		case VK_FORMAT_A2R10G10B10_SINT_PACK32: return Format::A2R10G10B10_SINT_PACK32;

		case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return Format::A2B10G10R10_UNORM_PACK32;
		case VK_FORMAT_A2B10G10R10_SNORM_PACK32: return Format::A2B10G10R10_SNORM_PACK32;
		case VK_FORMAT_A2B10G10R10_USCALED_PACK32: return Format::A2B10G10R10_USCALED_PACK32;
		case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: return Format::A2B10G10R10_SSCALED_PACK32;
		case VK_FORMAT_A2B10G10R10_UINT_PACK32: return Format::A2B10G10R10_UINT_PACK32;
		case VK_FORMAT_A2B10G10R10_SINT_PACK32: return Format::A2B10G10R10_SINT_PACK32;

		case VK_FORMAT_R16_UNORM: return Format::R16_UNORM;
		case VK_FORMAT_R16_SNORM: return Format::R16_SNORM;
		case VK_FORMAT_R16_USCALED: return Format::R16_USCALED;
		case VK_FORMAT_R16_SSCALED: return Format::R16_SSCALED;
		case VK_FORMAT_R16_UINT: return Format::R16_UINT;
		case VK_FORMAT_R16_SINT: return Format::R16_SINT;
		case VK_FORMAT_R16_SFLOAT: return Format::R16_SFLOAT;

		case VK_FORMAT_R16G16_UNORM: return Format::R16G16_UNORM;
		case VK_FORMAT_R16G16_SNORM: return Format::R16G16_SNORM;
		case VK_FORMAT_R16G16_USCALED: return Format::R16G16_USCALED;
		case VK_FORMAT_R16G16_SSCALED: return Format::R16G16_SSCALED;
		case VK_FORMAT_R16G16_UINT: return Format::R16G16_UINT;
		case VK_FORMAT_R16G16_SINT: return Format::R16G16_SINT;
		case VK_FORMAT_R16G16_SFLOAT: return Format::R16G16_SFLOAT;

		case VK_FORMAT_R16G16B16_UNORM: return Format::R16G16B16_UNORM;
		case VK_FORMAT_R16G16B16_SNORM: return Format::R16G16B16_SNORM;
		case VK_FORMAT_R16G16B16_USCALED: return Format::R16G16B16_USCALED;
		case VK_FORMAT_R16G16B16_SSCALED: return Format::R16G16B16_SSCALED;
		case VK_FORMAT_R16G16B16_UINT: return Format::R16G16B16_UINT;
		case VK_FORMAT_R16G16B16_SINT: return Format::R16G16B16_SINT;
		case VK_FORMAT_R16G16B16_SFLOAT: return Format::R16G16B16_SFLOAT;

		case VK_FORMAT_R16G16B16A16_UNORM: return Format::R16G16B16A16_UNORM;
		case VK_FORMAT_R16G16B16A16_SNORM: return Format::R16G16B16A16_SNORM;
		case VK_FORMAT_R16G16B16A16_USCALED: return Format::R16G16B16A16_USCALED;
		case VK_FORMAT_R16G16B16A16_SSCALED: return Format::R16G16B16A16_SSCALED;
		case VK_FORMAT_R16G16B16A16_UINT: return Format::R16G16B16A16_UINT;
		case VK_FORMAT_R16G16B16A16_SINT: return Format::R16G16B16A16_SINT;
		case VK_FORMAT_R16G16B16A16_SFLOAT: return Format::R16G16B16A16_SFLOAT;

		case VK_FORMAT_R32_UINT: return Format::R32_UINT;
		case VK_FORMAT_R32_SINT: return Format::R32_SINT;
		case VK_FORMAT_R32_SFLOAT: return Format::R32_SFLOAT;

		case VK_FORMAT_R32G32_UINT: return Format::R32G32_UINT;
		case VK_FORMAT_R32G32_SINT: return Format::R32G32_SINT;
		case VK_FORMAT_R32G32_SFLOAT: return Format::R32G32_SFLOAT;

		case VK_FORMAT_R32G32B32_UINT: return Format::R32G32B32_UINT;
		case VK_FORMAT_R32G32B32_SINT: return Format::R32G32B32_SINT;
		case VK_FORMAT_R32G32B32_SFLOAT: return Format::R32G32B32_SFLOAT;

		case VK_FORMAT_R32G32B32A32_UINT: return Format::R32G32B32A32_UINT;
		case VK_FORMAT_R32G32B32A32_SINT: return Format::R32G32B32A32_SINT;
		case VK_FORMAT_R32G32B32A32_SFLOAT: return Format::R32G32B32A32_SFLOAT;

		case VK_FORMAT_R64_UINT: return Format::R64_UINT;
		case VK_FORMAT_R64_SINT: return Format::R64_SINT;
		case VK_FORMAT_R64_SFLOAT: return Format::R64_SFLOAT;

		case VK_FORMAT_R64G64_UINT: return Format::R64G64_UINT;
		case VK_FORMAT_R64G64_SINT: return Format::R64G64_SINT;
		case VK_FORMAT_R64G64_SFLOAT: return Format::R64G64_SFLOAT;

		case VK_FORMAT_R64G64B64_UINT: return Format::R64G64B64_UINT;
		case VK_FORMAT_R64G64B64_SINT: return Format::R64G64B64_SINT;
		case VK_FORMAT_R64G64B64_SFLOAT: return Format::R64G64B64_SFLOAT;

		case VK_FORMAT_R64G64B64A64_UINT: return Format::R64G64B64A64_UINT;
		case VK_FORMAT_R64G64B64A64_SINT: return Format::R64G64B64A64_SINT;
		case VK_FORMAT_R64G64B64A64_SFLOAT: return Format::R64G64B64A64_SFLOAT;

		case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return Format::B10G11R11_UFLOAT_PACK32;
		case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: return Format::E5B9G9R9_UFLOAT_PACK32;

		case VK_FORMAT_D16_UNORM: return Format::D16_UNORM;
		case VK_FORMAT_X8_D24_UNORM_PACK32: return Format::X8_D24_UNORM_PACK32;
		case VK_FORMAT_D32_SFLOAT: return Format::D32_SFLOAT;
		case VK_FORMAT_S8_UINT: return Format::S8_UINT;

		default:
			ASSERT_MSG(false, "Unsupported format conversion");
			return Format::UNDEFINED;
	}
}

VkImageLayout grlTextureStateToVkImageLayout(Texture::State state)
{
	switch (state)
	{
		case Texture::State::UNDEFINED: return VK_IMAGE_LAYOUT_UNDEFINED;

		case Texture::State::TRANSFER_SRC: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		case Texture::State::TRANSFER_DST: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

		case Texture::State::SHADER_READ: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		case Texture::State::SHADER_WRITE: return VK_IMAGE_LAYOUT_GENERAL;

		case Texture::State::RENDER_TARGET: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		case Texture::State::DEPTH_READ: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		case Texture::State::DEPTH_WRITE: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		case Texture::State::PRESENT: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		default: return VK_IMAGE_LAYOUT_UNDEFINED;
	}
}

} // namespace grail
} // namespace ncine
