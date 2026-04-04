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
