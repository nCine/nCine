#ifndef CLASS_GRAIL_BINDGROUPLAYOUT
#define CLASS_GRAIL_BINDGROUPLAYOUT

#include <nctl/Array.h>

namespace ncine {
namespace grail {

namespace BindGroupLayout {

enum class BindingType
{
	UNIFORM_BUFFER,
	STORAGE_BUFFER,
	READONLY_STORAGE_BUFFER,

	SAMPLED_TEXTURE,
	STORAGE_TEXTURE,

	SAMPLER,

	COMBINED_TEXTURE_SAMPLER
};

enum class ShaderStage : uint8_t
{
	VERTEX = 1 << 0,
	FRAGMENT = 1 << 1,
	COMPUTE = 1 << 2
};

inline ShaderStage operator|(ShaderStage a, ShaderStage b)
{
	return static_cast<ShaderStage>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

struct Entry
{
	uint32_t binding = 0;
	BindingType type = BindingType::UNIFORM_BUFFER;
	ShaderStage visibility = ShaderStage::VERTEX;
	uint32_t count = 1;
	bool dynamicOffset = false;
};

struct CreateDesc
{
	const Entry *entries;
	uint32_t entryCount;
};

struct Desc
{
	nctl::Array<Entry> entries;
};

}

} // namespace grail
} // namespace ncine

#endif
