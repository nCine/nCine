#ifndef CLASS_GRAIL_DEVICE
#define CLASS_GRAIL_DEVICE

#include "common_defines.h"
#include <nctl/UniquePtr.h>
#include "grail/handle_types.h"

#include "grail/Swapchain.h"
#include "grail/BindGroupLayout.h"
#include "grail/BindGroup.h"
#include "grail/GraphicsPipeline.h"
#include "grail/Buffer.h"
#include "grail/Texture.h"
#include "grail/TextureView.h"
#include "grail/CommandBuffer.h"

namespace ncine {
namespace grail {

class Device
{
  public:
	enum class Type
	{
		UNSPECIFIED,
		INTEGRATED,
		DISCRETE
	};

	struct CreateDesc
	{
		Type preferredType = Type::INTEGRATED;
		bool enableValidation = DefaultValidationEnabledValue;
	};

	struct Desc
	{
		Type type = Type::UNSPECIFIED;
		bool validationEnabled = DefaultValidationEnabledValue;
	};

	struct Caps
	{
		uint32_t maxBuffers = 0;
		uint64_t maxBufferSize = 0;
	};

	Device() = delete;
	~Device();
	Device(const Device &) = delete;
	Device &operator=(const Device &) = delete;

	Device(Device &&) noexcept;
	Device &operator=(Device &&) noexcept = default;

	/** @name Device */
	///@{

	/// Creates the GrAIL device
	NODISCARD static Device create(const CreateDesc &desc);
	/// Destroys the GrAIL device
	void destroy();

	/// Returns the description structure of the device
	inline const Desc &desc() const { return desc_; }
	/// Returns the capabilities of the device
	inline const Caps &caps() const { return caps_; }

	/// The data to perform a query of supported depth formats
	struct DepthFormatQuery
	{
		enum class Usage : uint8_t
		{
			NONE = 0,
			DEPTH_STENCIL_ATTACHMENT = 1 << 0,
			SAMPLED = 1 << 1,
			FILTERABLE = 1 << 2,
			TRANSFER_SRC = 1 << 3,
			TRANSFER_DST = 1 << 4
		};

		enum class DepthPrecision : uint8_t
		{
			LOW, //< prefer 16bit
			MEDIUM, //< prefer 24bit
			HIGH //< prefer 32bit
		};

		bool requireStencil = false;
		Usage usage = Usage::DEPTH_STENCIL_ATTACHMENT;
		DepthPrecision depthPrecision = DepthPrecision::HIGH;
	};

	/// Returns the first compatible match or an undefined format
	Format findDepthFormat(const DepthFormatQuery &query);

	///@}

	/** @name Swapchain */
	///@{

	/// Creates the swapchain
	Swapchain &createSwapchain(const Swapchain::Desc &desc);
	/// Returns the swapchain
	Swapchain &swapchain();

	///@}

	/// Waits for the device to be idle before returning
	void waitIdle();

	/** @name BindGroupLayout */
	///@{

	/// Creates a new bind group layout
	NODISCARD BindGroupLayout::Handle createBindGroupLayout(const BindGroupLayout::CreateDesc &desc);
	/// Destroys a bind group layout
	void destroyBindGroupLayout(BindGroupLayout::Handle handle);

	/// Retrieves the description structure of a bind group layout
	const BindGroupLayout::Desc &bindGroupLayoutDesc(BindGroupLayout::Handle handle);

	///@}

	/** @name BindGroup */
	///@{

	/// Creates a new bind group
	NODISCARD BindGroup::Handle createBindGroup(const BindGroup::Desc &desc);
	/// Destroys a bind group
	void destroyBindGroup(BindGroup::Handle handle);

	/// Retrieves the description structure of a bind group
	const BindGroup::Desc &bindGroupDesc(BindGroup::Handle handle);

	///@}

	/** @name GraphicsPipeline */
	///@{

	/// Creates a new graphics pipeline
	NODISCARD GraphicsPipeline::Handle createGraphicsPipeline(const GraphicsPipeline::CreateDesc &desc);
	/// Destroys a graphics pipeline
	void destroyGraphicsPipeline(GraphicsPipeline::Handle handle);

	/// Retrieves the description structure of a graphics pipeline
	const GraphicsPipeline::Desc &graphicsPipelineDesc(GraphicsPipeline::Handle handle);

	///@}

	/** @name Buffer */
	///@{

	/// Creates a new buffer
	NODISCARD Buffer::Handle createBuffer(const Buffer::Desc &desc);
	/// Destroys a buffer
	void destroyBuffer(Buffer::Handle handle);

	/// Retrieves the description structure of a buffer
	const Buffer::Desc &bufferDesc(Buffer::Handle handle);

	/// Maps a buffer
	NODISCARD void *mapBuffer(Buffer::Handle handle);
	/// Unmaps a buffer
	void unmapBuffer(Buffer::Handle handle);

	/// Allocates the specified amount of bytes from a dynamic per-frame uniform buffer
	NODISCARD Buffer::DynamicAlloc allocateDynamicBuffer(uint32_t size);

	///@}

	/** @name Texture */
	///@{

	/// Creates a new texture
	NODISCARD Texture::Handle createTexture(const Texture::Desc &desc);
	/// Destroys a texture
	void destroyTexture(Texture::Handle handle);

	/// Retrieves the description structure of a texture
	const Texture::Desc &textureDesc(Texture::Handle handle);

	///@}

	/** @name TextureView */
	///@{

	/// Creates a new texture view
	NODISCARD TextureView::Handle createTextureView(const TextureView::Desc &desc);
	/// Destroys a texture view
	void destroyTextureView(TextureView::Handle handle);

	/// Retrieves the description structure of a texture view
	const TextureView::Desc &textureViewDesc(TextureView::Handle handle);

	///@}

	/** @name CommandBuffer*/
	///@{

	/// Creates a new command buffer (with a lifetime of a frame)
	NODISCARD CommandBuffer createCommandBuffer();
	/// Submits a command buffer for execution
	void submitCommandBuffer(CommandBuffer &commandBuffer);

	///@}

  private:
#ifdef NCINE_DEBUG
	static const bool DefaultValidationEnabledValue = true;
#else
	static const bool DefaultValidationEnabledValue = false;
#endif

	Desc desc_;
	Caps caps_;

	struct FrontendData;
	nctl::UniquePtr<FrontendData> frontendData_;

	struct BackendData;
	nctl::UniquePtr<BackendData> backendData_;

	Swapchain swapchain_;

	friend class Swapchain;
	friend class CommandBuffer;

	/// The tag prevents accidentally adding another public constructor
	struct InternalTag {};
	/// Private tagged constructor
	explicit Device(InternalTag);

	Format findDepthFormatImpl(const DepthFormatQuery &query);

	///** @name Swapchain implementation */
	///@{
	bool createSwapchainImpl(Swapchain &swapchain, const Swapchain::Desc &desc);
	void destroySwapchainImpl(Swapchain &swapchain);
	///@}

	/** @name BindGroupLayout implementation */
	///@{
	bool createBindGroupLayoutImpl(uint32_t index, const BindGroupLayout::CreateDesc &desc);
	void destroyBindGroupLayoutImpl(uint32_t index);
	///@}

	///** @name BindGroup implementation */
	///@{
	bool createBindGroupImpl(uint32_t index, const BindGroup::Desc &desc);
	void destroyBindGroupImpl(uint32_t index);
	///@}

	///** @name GraphicsPipeline implementation */
	///@{
	bool createGraphicsPipelineImpl(uint32_t index, const GraphicsPipeline::CreateDesc &desc);
	void destroyGraphicsPipelineImpl(uint32_t index);
	///@}

	///** @name Buffer implementation */
	///@{
	bool createBufferImpl(uint32_t index, const Buffer::Desc &desc);
	void destroyBufferImpl(uint32_t index);
	///@}

	///** @name Texture implementation */
	///@{
	bool createTextureImpl(uint32_t index, const Texture::Desc &desc);
	void destroyTextureImpl(uint32_t index);
	///@}

	///** @name TextureView implementation */
	///@{
	bool createTextureViewImpl(uint32_t index, const TextureView::Desc &desc);
	void destroyTextureViewImpl(uint32_t index);
	///@}

	///** @name CommandBuffer implementation */
	///@{
	bool createCommandBufferImpl(CommandBuffer &commandBuffer);
	///@}
};

} // namespace grail
} // namespace ncine

#endif
