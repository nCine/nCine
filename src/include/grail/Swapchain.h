#ifndef CLASS_GRAIL_SWAPCHAIN
#define CLASS_GRAIL_SWAPCHAIN

#include <cstdint>
#include <nctl/StaticArray.h>
#include "grail/handle_types.h"
#include "grail/formats.h"

namespace ncine {
namespace grail {

class Swapchain
{
  public:
	struct Desc
	{
		Format format = Format::UNDEFINED;

		uint32_t width = 0;
		uint32_t height = 0;

		uint32_t textureCount = 2;

		bool vsync = true;
	};

	enum class AcquireResult
	{
		SUCCESS = 0,
		SUBOPTIMAL,
		OUT_OF_DATE,
		UNKNOWN
	};

	struct AcquireDesc
	{
		AcquireResult result = AcquireResult::SUCCESS;
		uint32_t textureViewIndex = 0;
		TextureView::Handle textureView;
	};

	Swapchain(const Swapchain &) = delete;
	Swapchain &operator=(const Swapchain &) = delete;

	Swapchain(Swapchain &&) noexcept = default;
	Swapchain &operator=(Swapchain &&) noexcept = default;

	inline const Desc &desc() const { return desc_; }

	/// Tries to acquire the next texture view to present the current frame
	/*! \warning This method acts as a "beginFrame" marker, it should be called at the beginning of a frame */
	AcquireDesc acquireNextTextureView();

	/// Presents the frame to the swapchain
	bool present();

  private:
	Desc desc_;

	static constexpr uint32_t MaxTextures = 4;
	nctl::StaticArray<Texture::Handle, MaxTextures> textures_;
	nctl::StaticArray<TextureView::Handle, MaxTextures> textureViews_;

	struct BackendData;
	nctl::UniquePtr<BackendData> backendData_;

	Swapchain() = default;

	friend class Device;
};

} // namespace grail
} // namespace ncine

#endif
