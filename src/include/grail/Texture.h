#ifndef CLASS_GRAIL_TEXTURE
#define CLASS_GRAIL_TEXTURE

#include <cstdint>
#include "grail/formats.h"

namespace ncine {
namespace grail {

namespace Texture {

enum class Type : uint8_t
{
	TEXTURE_1D,
	TEXTURE_2D,
	TEXTURE_3D
};

enum class Usage : uint8_t
{
	NONE = 0,
	SAMPLED = 1 << 0,
	STORAGE = 1 << 1,
	COLOR_TARGET = 1 << 2,
	DEPTH_STENCIL = 1 << 3,
	TRANSFER_SRC = 1 << 4,
	TRANSFER_DST = 1 << 5
};

enum class SampleCount : uint8_t
{
	SAMPLE_1 = 1,
	SAMPLE_2 = 2,
	SAMPLE_4 = 4,
	SAMPLE_8 = 8
};

enum class State : uint8_t
{
	UNDEFINED,
	TRANSFER_SRC,
	TRANSFER_DST,
	SHADER_READ,
	SHADER_WRITE,
	RENDER_TARGET,
	DEPTH_READ,
	DEPTH_WRITE,
	PRESENT
};

inline Usage operator|(Usage a, Usage b)
{
	return static_cast<Usage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline bool hasFlag(Usage value, Usage flag)
{
	return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
}

struct Desc
{
	Type type = Type::TEXTURE_2D;

	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t depth = 1;

	uint32_t mipLevels = 1;
	uint32_t arrayLayers = 1;

	Format format = Format::UNDEFINED;

	Usage usage = Usage::SAMPLED;
	SampleCount sampleCount = SampleCount::SAMPLE_1;
};

}

} // namespace grail
} // namespace ncine

#endif
