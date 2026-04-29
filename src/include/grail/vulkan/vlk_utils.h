#ifndef NCINE_VLKUTILS
#define NCINE_VLKUTILS

#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include "grail/formats.h"
#include "grail/GraphicsPipeline.h"
#include "grail/Texture.h"

namespace ncine {
namespace grail {

/// Returns the name of the specified `VkResult` enumeration value
const char *vlkResultToString(VkResult result);
/// Asserts if the result is not `VK_SUCCESS`
void vlkAssert(VkResult result);
/// Fatal asserts if the result is not `VK_SUCCESS`
void vlkFatalAssert(VkResult result);

/// Debug callback for the `VK_EXT_DEBUG_UTILS_EXTENSION_NAME` extension
VKAPI_ATTR VkBool32 VKAPI_CALL vlkDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData);

/** @defgroup group1 Debug Object Names
 *  Functions to set the debug name for various object types
 *  @{
 */

void setObjectName(VkDevice device, VkInstance instanceHandle, const char *name);
void setObjectName(VkDevice device, VkPhysicalDevice physicalDeviceHandle, const char *name);
void setObjectName(VkDevice device, VkDevice deviceHandle, const char *name);
void setObjectName(VkDevice device, VkQueue queueHandle, const char *name);
void setObjectName(VkDevice device, VkSemaphore semaphoreHandle, const char *name);
void setObjectName(VkDevice device, VkCommandBuffer commandBufferHandle, const char *name);
void setObjectName(VkDevice device, VkFence fenceHandle, const char *name);
void setObjectName(VkDevice device, VkBuffer bufferHandle, const char *name);
void setObjectName(VkDevice device, VkImage imageHandle, const char *name);
void setObjectName(VkDevice device, VkImageView imageViewHandle, const char *name);
void setObjectName(VkDevice device, VkShaderModule shaderModuleHandle, const char *name);
void setObjectName(VkDevice device, VkRenderPass renderPassHandle, const char *name);
void setObjectName(VkDevice device, VkPipeline pipelineHandle, const char *name);
void setObjectName(VkDevice device, VkDescriptorSetLayout descriptorSetLayoutHandle, const char *name);
void setObjectName(VkDevice device, VkSampler samplerHandle, const char *name);
void setObjectName(VkDevice device, VkDescriptorPool descriptorPoolHandle, const char *name);
void setObjectName(VkDevice device, VkDescriptorSet descriptorSetHandle, const char *name);
void setObjectName(VkDevice device, VkCommandPool commandPoolHandle, const char *name);
void setObjectName(VkDevice device, VkSurfaceKHR surfaceHandle, const char *name);
void setObjectName(VkDevice device, VkSwapchainKHR swapchainHandle, const char *name);

/** @} */

/// Returns the Vulkan image format from the corresponding GrAIL one
VkFormat grlFormatToVk(Format format);
/// Returns the Vulkan vertex format from the corresponding GrAIL one
VkFormat grlFormatToVk(GraphicsPipeline::VertexFormat format);

/// Returns the GrAIL image format from the corresponding Vulkan one
Format vlkFormatToGrl(VkFormat format);

VkImageLayout grlTextureStateToVkImageLayout(Texture::State state);

} // namespace grail
} // namespace ncine

#endif
