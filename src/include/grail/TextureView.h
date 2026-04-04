#ifndef CLASS_GRAIL_TEXTUREVIEW
#define CLASS_GRAIL_TEXTUREVIEW

#include <cstdint>
#include "grail/formats.h"
#include "grail/handle_types.h"

namespace ncine {
namespace grail {

namespace TextureView {

enum class Type : uint8_t
{
	TEXTURE_1D,
	TEXTURE_1D_ARRAY,

	TEXTURE_2D,
	TEXTURE_2D_ARRAY,

	TEXTURE_3D,

	TEXTURE_CUBE,
	TEXTURE_CUBE_ARRAY
};

enum class Filter : uint8_t
{
	NEAREST,
	LINEAR
};

enum class WrapMode : uint8_t
{
	REPEAT,
	MIRRORED_REPEAT,
	CLAMP_TO_EDGE,
	CLAMP_TO_BORDER
};

struct Sampler
{
	Filter magFilter = Filter::LINEAR;
	Filter minFilter = Filter::LINEAR;
	WrapMode wrapU = WrapMode::REPEAT;
	WrapMode wrapV = WrapMode::REPEAT;
	WrapMode wrapW = WrapMode::REPEAT;
};

struct Desc
{
	Texture::Handle texture;

	Format format = Format::UNDEFINED;

	uint32_t baseMip = 0;
	uint32_t mipCount = 1;

	uint32_t baseLayer = 0;
	uint32_t layerCount = 1;

	Type type = Type::TEXTURE_2D;
	Sampler sampler;
};

}

} // namespace grail
} // namespace ncine

#endif
