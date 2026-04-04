#define NCINE_INCLUDE_VULKAN
#include "common_headers.h"
#include <vma/vk_mem_alloc.h>

#include <nctl/StaticArray.h>
#include <nctl/Atomic.h>
#include "grail/config.h"
#include "grail/private/hash_types.h"
#include "grail/Device.h"
#include "grail/Swapchain.h"

namespace ncine {
namespace grail {

namespace RenderPass {
	struct RenderTargetLayoutDesc;
	struct RenderPassDesc;
	struct RenderTargetDesc;
}

struct Swapchain::BackendData
{
	struct SupportDetails
	{
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkSurfaceCapabilitiesKHR capabilities;
		nctl::Array<VkSurfaceFormatKHR> surfaceFormats;
		nctl::Array<VkPresentModeKHR> presentModes;
	};

	struct Settings
	{
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkSurfaceFormatKHR surfaceFormat;
		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
		VkExtent2D extent;
		uint32_t imageCount = 1;
	};

	SupportDetails supportDetails_;
	Settings settings_;
	VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
	uint32_t currentImageIndex_ = 0;
	VkSemaphore presentSemaphores[MaxTextures];
	Swapchain *frontend_ = nullptr;
	// The swapchain needs the device to create the TextureViews
	Device *theDevice_ = nullptr;

	void populateSwapchainCreateInfo(VkSwapchainCreateInfoKHR &createInfo);
	void updateDesc(Desc &desc);
	bool canBeCreated(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const Desc &desc);
	bool createImagesAndViews(const Desc &desc);
	bool create(const Desc &createDesc, Desc &desc);
	void destroy();
	void destroyImagesAndViews();
};

struct CommandBuffer::BackendData
{
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	GraphicsPipeline::Handle graphicsPipeline;
	BindGroup::Handle bindGroup;
	Device::BackendData *deviceBackend = nullptr;
	Device::FrontendData *deviceFrontend = nullptr;

	uint32_t numDynamicOffsets = 0;
	uint32_t dynamicOffsets[ExtraLimits::MaxDynamicOffsetsBindGroup];

	void bindDescriptorSet();
};

struct Device::BackendData
{
	static constexpr uint32_t NumFramesInFlight = 2;

	/// A persistent mapped host buffer used for per-frame uniforms
	/*! \note There is only one buffer, but it's partitioned between frames. */
	Buffer::Handle dynamicUploadBuffer_;
	uint8_t *dynamicUploadBufferMapBase_ = nullptr;

	struct Capabilities
	{
		bool vulkan11 = false;
		bool debugUtils = false;
	};

	struct QueueData
	{
		uint32_t queueIndex = uint32_t(-1);
		VkQueue queue = VK_NULL_HANDLE;
	};

	struct FrameContext
	{
		static constexpr uint32_t CommandBufferBatchSize = 32;

		bool swapchainHasAcquired = false;
		VkFence frameFence = VK_NULL_HANDLE;

		VkSemaphore acquireSemaphore = VK_NULL_HANDLE;

		VkCommandPool graphicsCmdPool = VK_NULL_HANDLE;
		nctl::Array<CommandBuffer::BackendData> freeCommandBuffers{ CommandBufferBatchSize };
		nctl::Array<CommandBuffer::BackendData> usedCommandBuffers{ CommandBufferBatchSize };
		VkCommandPool computeCmdPool = VK_NULL_HANDLE;
		VkCommandPool transferCmdPool = VK_NULL_HANDLE;

		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

		nctl::AtomicU32 dynamicUploadBufferOffset;
	};

	struct DescriptorPoolCreateDesc
	{
		uint32_t maxSets = 0;
		uint32_t combinedImageSamplerCount = 0;
		uint32_t uniformBufferCount = 0;
		uint32_t uniformBufferDynamicCount = 0;
	};

	struct DescriptorPoolData
	{
		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
		uint32_t maxSets = 0;
		uint32_t combinedImageSamplerCount = 0;
		uint32_t uniformBufferCount = 0;
		uint32_t uniformBufferDynamicCount = 0;
	};

	struct BindGroupLayoutData
	{
		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	};

	struct BindGroupData
	{
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	};

	struct GraphicsPipelineData
	{
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	};

	struct BufferData
	{
		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;
		VmaAllocationInfo allocationInfo;
	};

	struct TextureData
	{
		VkImage image = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;
		VmaAllocationInfo allocationInfo;
		Texture::State textureState = Texture::State::UNDEFINED;
	};

	struct TextureViewData
	{
		VkImageView imageView = VK_NULL_HANDLE;
		VkImage image = VK_NULL_HANDLE;
		VkSampler sampler = VK_NULL_HANDLE;
	};

	struct Capabilities caps_;

	uint32_t currentFrameIndex = 0;
	FrameContext frameContexts_[NumFramesInFlight];
	nctl::Array<DescriptorPoolData> descriptorPoolData_{ 4 };

	const VkAllocationCallbacks *allocator_ = nullptr;
	VmaAllocator vmaAllocator_ = VK_NULL_HANDLE;

	VkInstance instance_ = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger_ = VK_NULL_HANDLE;

	VkSurfaceKHR surface_ = VK_NULL_HANDLE;

	VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties physicalDeviceProperties_;
	VkPhysicalDeviceFeatures physicalDeviceFeatures_;
	VkPhysicalDeviceLimits physicalDeviceLimits_;
	nctl::HashMap<VkFormat, VkFormatProperties> depthFormatProperties_{ 8 };
	VkDevice device_ = VK_NULL_HANDLE;
	QueueData graphicsQueue_;

	// The handles index both the frontend and the backend arrays
	BindGroupLayoutData bindGroupLayoutData_[PoolLimits::BindGroupLayouts];
	BindGroupData bindGroupData_[PoolLimits::BindGroups];
	GraphicsPipelineData graphicsPipelineData_[PoolLimits::GraphicsPipelines];
	BufferData bufferData_[PoolLimits::Buffers];
	TextureData textureData_[PoolLimits::Textures];
	TextureViewData textureViewData_[PoolLimits::TextureViews];

	// Backend specific caches
	/// Hashes are created from `RenderTargetLayoutDesc` and render passes only used during pipeline creation
	Desc64HashMap<VkRenderPass> hashToLayoutVkRenderPasses_{ BackendCacheLimits::PipelineLayouts }; // Using PipelineLayouts for count
	/// Hashes are created from `RenderPassDesc` and render passes are used in command buffers
	Desc64HashMap<VkRenderPass> hashToVkRenderPasses_{ BackendCacheLimits::RenderPasses };
	/// Hashes are created from `RenderTargetDesc` and render passes are used in command buffers
	Desc64HashMap<VkFramebuffer> hashToVkFramebuffers_{ BackendCacheLimits::FrameBuffers };
	Desc64HashMap<VkPipelineLayout> hashToVkPipelineLayout_{ BackendCacheLimits::PipelineLayouts };
	Desc64HashMap<VkSampler> hashToVkSampler_{ BackendCacheLimits::Samplers };

	/** @name Instance */
	///@{

	bool createInstance(const CreateDesc &createDesc, Desc &desc);
	void destroyInstance();
	void setupDebugMessenger();

	///@}

	/** @name Surface */
	///@{

	bool createSurface();
	void destroySurface();

	///@}

	/** @name Device */
	///@{

	bool createDevice(const CreateDesc &createDesc, Desc &desc);
	void destroyDevice();
	void createBackendData(Device &device);
	void destroyBackendData(Device &device);

	///@}

	/** @name Synchronization */
	///@{

	VkFence createFence(bool signaled);
	void destroyFence(VkFence fence);
	void waitFence(VkFence fence);
	void resetFence(VkFence fence);

	VkSemaphore createSemaphore();
	void destroySemaphore(VkSemaphore semaphore);

	///@}

	/** @name FrameContext */
	///@{

	bool createFrameContext(FrameContext &frameContext);
	void destroyFrameContext(FrameContext &frameContext);
	void beginFrameContext();
	void endFrameContext();
	void allocateFreeCommandBuffers(FrameContext &frameContext);

	CommandBuffer::BackendData &obtainCommandBufferData(FrameContext &frameContext);

	VkCommandPool createCommandPool(uint32_t queueFamilyIndex);
	bool resetCommandPool(VkCommandPool commandPool);
	void destroyCommandPool(VkCommandPool commandPool);

	VkCommandBuffer allocateCommandBuffers(VkCommandPool commandPool, VkCommandBuffer *commandBuffers, uint32_t count);

	///@}

	/** @name BindGroupLayout */
	///@{

	VkDescriptorPool createDescriptorPool(VkDescriptorPoolSize *poolSizes, uint32_t poolSizeCount, uint32_t maxSets);
	void destroyDescriptorPool(VkDescriptorPool descriptorPool);

	VkDescriptorSet allocateDescriptorSet(VkDescriptorPool descriptorPool, VkDescriptorSetLayout setLayout);
	void freeDescriptorSet(VkDescriptorPool descriptorPool, VkDescriptorSet descriptorSet);

	void addDescriptorPool();
	VkDescriptorSet allocateFromDescriptorPools(const BindGroup::Desc &desc, VkDescriptorPool &descriptorPool);

	///@}

	/** @name Pipeline */
	///@{

	VkShaderModule createShaderModule(const uint8_t *codeData, size_t codeSize);
	void destroyShaderModule(VkShaderModule shaderModule);

	VkRenderPass createRenderPass(const RenderPass::RenderTargetLayoutDesc &desc);
	VkRenderPass createRenderPass(const RenderPass::RenderPassDesc &desc);
	void destroyRenderPass(VkRenderPass renderPass);

	VkFramebuffer createFramebuffer(const RenderPass::RenderTargetDesc &desc, VkRenderPass renderPass);
	void destroyFramebuffer(VkFramebuffer framebuffer);

	VkPipelineLayout createPipelineLayout(const GraphicsPipeline::PipelineLayoutCreateDesc &desc);
	void destroyPipelineLayout(VkPipelineLayout pipelineLayout);

	///@}

	/** @name TextureView */
	///@{

	VkSampler createSampler(const TextureView::Sampler &desc);
	void destroySampler(VkSampler sampler);

	///@}
};

} // namespace grail
} // namespace ncine
