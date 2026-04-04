#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"

#include <nctl/HashMapIterator.h>
#include <nctl/HashSet.h>
#include "grail/Device.h"
#include "grail/vulkan/vlk_utils.h"
#include "grail/vulkan/Device_backend.h"

namespace ncine {
namespace grail {

namespace {

struct QueueProperties
{
	VkQueueFlags flags = 0;
	uint32_t count = 0;
	bool hasPresentSupport = false;

	bool hasGraphics() const { return flags & VK_QUEUE_GRAPHICS_BIT; }
	bool hasCompute() const { return flags & VK_QUEUE_COMPUTE_BIT; }
	bool hasTransfer() const { return flags & VK_QUEUE_TRANSFER_BIT; }
	bool hasPresent() const { return hasPresentSupport; }
};

struct PhysicalDeviceQueryData
{
	explicit PhysicalDeviceQueryData(uint32_t extensionCount)
	    : availableExtensions(extensionCount) {}

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	nctl::Array<VkExtensionProperties> extensionProps;
	nctl::HashSet<const char *> availableExtensions;
	VkPhysicalDeviceFeatures features{};
	VkPhysicalDeviceProperties properties{};

	nctl::HashMap<VkFormat, VkFormatProperties> depthFormatProps{ 8 };

	nctl::Array<VkQueueFamilyProperties> queueFamilyProps{ 8 };
	nctl::Array<QueueProperties> queueProps{ 16 };
	int32_t graphicsQueueIndex = -1;

	int32_t score = 0;
};

VkDevice createLogicalDevice(const PhysicalDeviceQueryData &physicalDeviceQueryData, const VkAllocationCallbacks *allocator_)
{
	VkDevice device = VK_NULL_HANDLE;

	FATAL_ASSERT_MSG(physicalDeviceQueryData.graphicsQueueIndex >= 0, "The physical device has no graphics queue");

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = physicalDeviceQueryData.graphicsQueueIndex;
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};

	nctl::Array<const char *> enabledExtensionsNames(1);
	ASSERT(physicalDeviceQueryData.availableExtensions.contains(VK_KHR_SWAPCHAIN_EXTENSION_NAME));
	enabledExtensionsNames.pushBack(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = enabledExtensionsNames.size();
	createInfo.ppEnabledExtensionNames = enabledExtensionsNames.data();

	void *pNextChain = nullptr;

	// Add Vulkan 1.1 features
	VkPhysicalDeviceVulkan11Features features11{};
	if (physicalDeviceQueryData.properties.apiVersion >= VK_API_VERSION_1_1)
	{
		features11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		features11.shaderDrawParameters = VK_TRUE;
		features11.pNext = pNextChain;
		pNextChain = &features11;
	}

	createInfo.pNext = pNextChain;

	const VkResult result = vkCreateDevice(physicalDeviceQueryData.physicalDevice, &createInfo, allocator_, &device);
	vlkFatalAssert(result);

	return device;
}

void queryDevice(VkPhysicalDevice physicalDevice, PhysicalDeviceQueryData &queryData)
{
	queryData.physicalDevice = physicalDevice;

	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	queryData.extensionProps.setSize(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, queryData.extensionProps.data());

	for (unsigned int i = 0; i < extensionCount; i++)
		queryData.availableExtensions.insert(queryData.extensionProps[i].extensionName);

	vkGetPhysicalDeviceFeatures(physicalDevice, &queryData.features);
	vkGetPhysicalDeviceProperties(physicalDevice, &queryData.properties);

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	queryData.queueFamilyProps.setSize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queryData.queueFamilyProps.data());

	const VkFormat depthFormatCandidates[] = {
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_X8_D24_UNORM_PACK32,
		VK_FORMAT_D16_UNORM
	};

	for (VkFormat depthFormat : depthFormatCandidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, depthFormat, &props);
		queryData.depthFormatProps.insert(depthFormat, props);
	}
}

void queryDeviceQueues(VkSurfaceKHR surface, PhysicalDeviceQueryData &queryData)
{
	for (unsigned int i = 0; i < queryData.queueFamilyProps.size(); i++)
	{
		const VkQueueFamilyProperties &queueFamilyProps = queryData.queueFamilyProps[i];
		queryData.queueProps.emplaceBack();

		QueueProperties &queueProps = queryData.queueProps.back();
		queueProps.flags = queueFamilyProps.queueFlags;
		queueProps.count = queueFamilyProps.queueCount;

		if (surface != VK_NULL_HANDLE)
		{
			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(queryData.physicalDevice, i, surface, &presentSupport);
			if (presentSupport == VK_TRUE)
				queryData.queueProps.back().hasPresentSupport = true;
		}

		if (queueProps.hasGraphics() && queueProps.hasCompute() && queueProps.hasPresentSupport)
			queryData.graphicsQueueIndex = i;
	}
}

void scorePhysicalDevice(Device::Type preferredType, PhysicalDeviceQueryData &queryData)
{
	// Let's give a starting score higher than zero
	switch (queryData.properties.deviceType)
	{
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			queryData.score += 100;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			queryData.score += 50;
			break;
		default:
			queryData.score += 1;
			break;
	}

	if ((queryData.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && preferredType == Device::Type::INTEGRATED) ||
	    (queryData.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && preferredType == Device::Type::DISCRETE))
	{
		queryData.score += 1000;
	}

	if (queryData.availableExtensions.contains(VK_KHR_SWAPCHAIN_EXTENSION_NAME) == false)
		queryData.score = 0;

	if (queryData.properties.apiVersion < VK_API_VERSION_1_1)
		queryData.score = 0;
}

void enumeratePhysicalDevices(VkInstance instance, VkSurfaceKHR surface, Device::Type preferredType, nctl::Array<PhysicalDeviceQueryData> &queryData)
{
	FATAL_ASSERT(instance != VK_NULL_HANDLE);

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	FATAL_ASSERT_MSG(deviceCount > 0, "No Vulkan physical devices available");

	nctl::Array<VkPhysicalDevice> physicalDevices(deviceCount);
	physicalDevices.setSize(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

	queryData.setCapacity(deviceCount);
	for (unsigned int i = 0; i < deviceCount; i++)
	{
		queryData.emplaceBack(512);
		queryDevice(physicalDevices[i], queryData[i]);
		queryDeviceQueues(surface, queryData[i]);
		scorePhysicalDevice(preferredType, queryData[i]);
	}
}

VkPhysicalDevice pickPhysicalDevice(nctl::Array<PhysicalDeviceQueryData> &queryData, int32_t &index, Device::Desc &desc)
{
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	index = -1;

	// To select a physical device, it needs a score higher than zero
	int32_t maxScore = 0;
	for (int32_t i = 0; i < queryData.size(); i++)
	{
		if (queryData[i].score > maxScore)
		{
			maxScore = queryData[i].score;
			index = i;
		}
	}

	if (index >= 0)
	{
		physicalDevice = queryData[index].physicalDevice;
		if (queryData[index].properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			desc.type = Device::Type::INTEGRATED;
		else if (queryData[index].properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			desc.type = Device::Type::DISCRETE;
	}

	return physicalDevice;
}

}

///////////////////////////////////////////////////////////
// Device::BackendData
///////////////////////////////////////////////////////////

bool Device::BackendData::createDevice(const Device::CreateDesc &createDesc, Device::Desc &desc)
{
	ASSERT(device_ == VK_NULL_HANDLE);
	if (device_ != VK_NULL_HANDLE)
		return false;

	ASSERT(instance_ != VK_NULL_HANDLE);
	if (instance_ == VK_NULL_HANDLE)
		return false;

	ASSERT(surface_ != VK_NULL_HANDLE);
	if (surface_ == VK_NULL_HANDLE)
		return false;

	nctl::Array<PhysicalDeviceQueryData> queryData;
	enumeratePhysicalDevices(instance_, surface_, createDesc.preferredType, queryData);
	int32_t queryDataIndex = -1;
	physicalDevice_ = pickPhysicalDevice(queryData, queryDataIndex, desc);
	FATAL_ASSERT_MSG(physicalDevice_ != VK_NULL_HANDLE, "Cannot find a physical device");

	physicalDeviceFeatures_ = queryData[queryDataIndex].features;
	physicalDeviceProperties_ = queryData[queryDataIndex].properties;
	depthFormatProperties_ = queryData[queryDataIndex].depthFormatProps;
	graphicsQueue_.queueIndex = queryData[queryDataIndex].graphicsQueueIndex;

	caps_.vulkan11 = (physicalDeviceProperties_.apiVersion >= VK_API_VERSION_1_1);

	device_ = createLogicalDevice(queryData[queryDataIndex], allocator_);
	volkLoadDevice(device_);

	vkGetDeviceQueue(device_, graphicsQueue_.queueIndex, 0, &graphicsQueue_.queue);

	VmaVulkanFunctions vmaFunctions{};
	vmaFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	vmaFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.instance = instance_;
	allocatorInfo.physicalDevice = physicalDevice_;
	allocatorInfo.device = device_;
	allocatorInfo.pVulkanFunctions = &vmaFunctions;
	vmaCreateAllocator(&allocatorInfo, &vmaAllocator_);

	return true;
}

void Device::BackendData::destroyDevice()
{
	if (device_ != VK_NULL_HANDLE)
	{
		if (vmaAllocator_ != VK_NULL_HANDLE)
		{
			vmaDestroyAllocator(vmaAllocator_);
			vmaAllocator_ = VK_NULL_HANDLE;
		}

		vkDestroyDevice(device_, nullptr);
		device_ = VK_NULL_HANDLE;
		physicalDevice_ = VK_NULL_HANDLE;
	}
}

void Device::BackendData::createBackendData(Device &device)
{
	addDescriptorPool();

	// Create the uniform buffer before creating the frame contexts
	Buffer::Desc bufferDesc;
	bufferDesc.size = ExtraLimits::DynamicUploadBufferSize * NumFramesInFlight;
	bufferDesc.memoryType = Buffer::MemoryType::UPLOAD;
	bufferDesc.usage = Buffer::Usage::UNIFORM;
	dynamicUploadBuffer_ = device.createBuffer(bufferDesc);
	dynamicUploadBufferMapBase_ = static_cast<uint8_t *>(device.mapBuffer(dynamicUploadBuffer_));

	for (uint32_t i = 0; i < NumFramesInFlight; i++)
		createFrameContext(frameContexts_[i]);
}

void Device::BackendData::destroyBackendData(Device &device)
{
	device.unmapBuffer(dynamicUploadBuffer_);
	device.destroyBuffer(dynamicUploadBuffer_);
	dynamicUploadBufferMapBase_ = nullptr;

	for (uint32_t i = 0; i < descriptorPoolData_.size(); i++)
	{
		destroyDescriptorPool(descriptorPoolData_[i].descriptorPool);
		descriptorPoolData_[i].descriptorPool = VK_NULL_HANDLE;
	}

	for (uint32_t i = 0; i < NumFramesInFlight; i++)
		destroyFrameContext(frameContexts_[i]);

	for (Desc64HashMap<VkFramebuffer>::ConstIterator it = hashToVkFramebuffers_.begin(); it != hashToVkFramebuffers_.end(); ++it)
		destroyFramebuffer(it.value());
	for (Desc64HashMap<VkRenderPass>::ConstIterator it = hashToVkRenderPasses_.begin(); it != hashToVkRenderPasses_.end(); ++it)
		destroyRenderPass(it.value());
	for (Desc64HashMap<VkRenderPass>::ConstIterator it = hashToLayoutVkRenderPasses_.begin(); it != hashToLayoutVkRenderPasses_.end(); ++it)
		destroyRenderPass(it.value());
	for (Desc64HashMap<VkPipelineLayout>::ConstIterator it = hashToVkPipelineLayout_.begin(); it != hashToVkPipelineLayout_.end(); ++it)
		destroyPipelineLayout(it.value());
	for (Desc64HashMap<VkSampler>::ConstIterator it = hashToVkSampler_.begin(); it != hashToVkSampler_.end(); ++it)
		destroySampler(it.value());
}

} // namespace grail
} // namespace ncine
