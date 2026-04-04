#ifndef CLASS_GRAIL_RENDERPASS
#define CLASS_GRAIL_RENDERPASS

#include "grail/formats.h"
#include "grail/types.h"
#include "grail/handle_types.h"

namespace ncine {
namespace grail {

namespace RenderPass {

struct ColorTargetDesc
{
	Format format = Format::UNDEFINED;
};

struct DepthTargetDesc
{
	Format format = Format::UNDEFINED;
};

constexpr uint8_t MaxColorAttachments = 4;
struct RenderTargetLayoutDesc
{
	uint32_t colorCount = 0;
	ColorTargetDesc colors[MaxColorAttachments];
	DepthTargetDesc depth;
};

enum class LoadOp : uint8_t
{
	DONT_CARE = 0,
	LOAD,
	CLEAR
};

enum class StoreOp : uint8_t
{
	DONT_CARE = 0,
	STORE
};

// Not used during pipeline creation, only in command buffers
struct RenderPassDesc
{
	RenderTargetLayoutDesc layout;

	LoadOp colorLoadOps[MaxColorAttachments] = { LoadOp::DONT_CARE, LoadOp::DONT_CARE, LoadOp::DONT_CARE, LoadOp::DONT_CARE };
	StoreOp colorStoreOps[MaxColorAttachments] = { StoreOp::STORE, StoreOp::STORE, StoreOp::STORE, StoreOp::STORE };
	ClearColorValue clearColors[MaxColorAttachments] = { ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f }, ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f },
	                                                     ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f }, ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f } };

	LoadOp depthLoadOp = LoadOp::DONT_CARE;
	StoreOp depthStoreOp = StoreOp::DONT_CARE;
	float clearDepth = 1.0f;
};

struct RenderTargetDesc
{
	TextureView::Handle color[MaxColorAttachments];
	TextureView::Handle depth;

	uint32_t width;
	uint32_t height;
};

}

} // namespace grail
} // namespace ncine

#endif
