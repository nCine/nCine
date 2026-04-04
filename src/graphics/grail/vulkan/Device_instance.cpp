#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"
#include "version.h"

#include <nctl/HashSet.h>
#include <nctl/String.h>
#include "grail/Device.h"
#include "grail/vulkan/vlk_utils.h"
#include "grail/vulkan/Device_backend.h"

#include "Application.h"
#include "IGfxDevice.h"

namespace ncine {
namespace grail {

namespace {

struct InstanceEnumerationData
{
	InstanceEnumerationData(uint32_t layerCount, uint32_t extensionCount)
	    : availableLayers(layerCount), availableExtensions(extensionCount) {}

	nctl::Array<VkLayerProperties> layerProps;
	nctl::Array<VkExtensionProperties> extensionProps;

	nctl::HashSet<const char *> availableLayers;
	nctl::HashSet<const char *> availableExtensions;
};

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity =
	    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType =
	    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = vlkDebugCallback;
	createInfo.pUserData = nullptr;
}

uint32_t enumerateLayers(InstanceEnumerationData &enumData)
{
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	enumData.layerProps.setSize(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, enumData.layerProps.data());

	for (unsigned int i = 0; i < layerCount; i++)
		enumData.availableLayers.insert(enumData.layerProps[i].layerName);

	return layerCount;
}

uint32_t enumerateExtensions(InstanceEnumerationData &enumData)
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	enumData.extensionProps.setSize(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, enumData.extensionProps.data());

	for (unsigned int i = 0; i < extensionCount; i++)
		enumData.availableExtensions.insert(enumData.extensionProps[i].extensionName);

	return extensionCount;
}

}

///////////////////////////////////////////////////////////
// Device::BackendData
///////////////////////////////////////////////////////////

bool Device::BackendData::createInstance(const Device::CreateDesc &createDesc, Device::Desc &desc)
{
	VkResult result = volkInitialize();
	vlkFatalAssert(result);

	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "nCine on Vulkan";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(VersionNumbers::Major, VersionNumbers::Minor, VersionNumbers::Patch);
	applicationInfo.pEngineName = "nCine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(VersionNumbers::Major, VersionNumbers::Minor, VersionNumbers::Patch);
	applicationInfo.apiVersion = VK_API_VERSION_1_1;

	InstanceEnumerationData enumData(64, 64);
	enumerateLayers(enumData);
	enumerateExtensions(enumData);

	const bool validationExtAvailable = enumData.availableLayers.contains("VK_LAYER_KHRONOS_validation");
	const bool debugUtilsExtAvailable = enumData.availableExtensions.contains(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	const bool validationCanBeEnabled = (createDesc.enableValidation && validationExtAvailable && debugUtilsExtAvailable);

	nctl::Array<nctl::String> requiredLayers(4);
	if (validationCanBeEnabled)
		requiredLayers.pushBack("VK_LAYER_KHRONOS_validation");

	nctl::Array<nctl::String> requiredExtensions(8);
	nctl::Array<const char *> surfaceExtensions(4);
	const unsigned int numSurfaceExtensions = theApplication().gfxDevice().addVulkanInstanceExtensions(surfaceExtensions);
	ASSERT(numSurfaceExtensions > 0);

	for (uint32_t i = 0; i < numSurfaceExtensions; i++)
		requiredExtensions.pushBack(surfaceExtensions[i]);
	if (debugUtilsExtAvailable)
		requiredExtensions.pushBack(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (validationCanBeEnabled)
	{
		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = &debugCreateInfo;
	}

	createInfo.pApplicationInfo = &applicationInfo;

	nctl::Array<const char *> enabledLayerNames;
	enabledLayerNames.setCapacity(requiredLayers.size());
	for (unsigned int i = 0; i < requiredLayers.size(); i++)
		enabledLayerNames.pushBack(requiredLayers[i].data());

	if (enabledLayerNames.isEmpty() == false)
	{
		createInfo.enabledLayerCount = enabledLayerNames.size();
		createInfo.ppEnabledLayerNames = enabledLayerNames.data();
	}

	nctl::Array<const char *> enabledExtensionsNames;
	enabledExtensionsNames.setCapacity(requiredExtensions.size());
	for (unsigned int i = 0; i < requiredExtensions.size(); i++)
		enabledExtensionsNames.pushBack(requiredExtensions[i].data());

	if (enabledExtensionsNames.isEmpty() == false)
	{
		createInfo.enabledExtensionCount = enabledExtensionsNames.size();
		createInfo.ppEnabledExtensionNames = enabledExtensionsNames.data();
	}

	if (validationCanBeEnabled)
	{
		const VkBool32 verboseValue = true;
		const VkLayerSettingEXT layerSetting = { "VK_LAYER_KHRONOS_validation", "validate_sync", VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &verboseValue };
		VkLayerSettingsCreateInfoEXT layerSettingsCreateInfo = { VK_STRUCTURE_TYPE_LAYER_SETTINGS_CREATE_INFO_EXT, nullptr, 1, &layerSetting };

		createInfo.pNext = &layerSettingsCreateInfo;
	}

	result = vkCreateInstance(&createInfo, allocator_, &instance_);
	vlkFatalAssert(result);
	volkLoadInstance(instance_);

	if (validationCanBeEnabled)
		setupDebugMessenger();

	desc.validationEnabled = validationCanBeEnabled;
	caps_.debugUtils = debugUtilsExtAvailable;

	return true;
}

void Device::BackendData::destroyInstance()
{
	if (debugMessenger_ != VK_NULL_HANDLE)
	{
		vkDestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);
		debugMessenger_ = VK_NULL_HANDLE;
	}

	if (instance_ != VK_NULL_HANDLE)
	{
		vkDestroyInstance(instance_, allocator_);
		instance_ = VK_NULL_HANDLE;
	}
}

void Device::BackendData::setupDebugMessenger()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);

	const VkResult result = vkCreateDebugUtilsMessengerEXT(instance_, &createInfo, allocator_, &debugMessenger_);
	vlkAssert(result);
}

} // namespace grail
} // namespace ncine
