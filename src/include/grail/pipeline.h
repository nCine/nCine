#ifndef CLASS_GRAIL_SPIPELINE
#define CLASS_GRAIL_SPIPELINE

#include <cstddef>
#include <cstdint>
#include <nctl/Array.h>
#include "grail/handle_types.h"

namespace ncine {
namespace grail {

namespace pipeline {

constexpr uint8_t MaxBindGroups = 4;
struct PipelineLayoutCreateDesc
{
	uint32_t bindGroupLayoutCount = 0;
	BindGroupLayout::Handle bindGroupLayouts[MaxBindGroups];
};

struct PipelineLayoutDesc
{
	nctl::Array<BindGroupLayout::Handle> bindGroupLayouts;
};

struct ShaderDesc
{
	const uint8_t *codeData = nullptr;
	size_t codeSize = 0;
	const char *entryName = "main";

	const char *debugName = nullptr;

	inline bool isValid() const { return (codeData && codeSize > 0 && entryName); }
};

}

} // namespace grail
} // namespace ncine

#endif
