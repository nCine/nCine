#ifndef GRAIL_TYPES
#define GRAIL_TYPES

#include <cstdint>

namespace ncine {
namespace grail {

struct Extent2D
{
	uint32_t width;
	uint32_t height;
};

struct Extent3D
{
	uint32_t width;
	uint32_t height;
	uint32_t depth;
};

struct Offset2D
{
	int32_t x;
	int32_t y;
};

struct Offset3D
{
	int32_t x;
	int32_t y;
	int32_t z;
};

struct Rect2D
{
	Offset2D offset;
	Extent2D extent;
};

union ClearColorValue
{
	float float32[4];
	int32_t int32[4];
	uint32_t uint32[4];
};

struct ClearDepthStencilValue
{
	float depth;
	uint32_t stencil;
};

} // namespace grail
} // namespace ncine

#endif
