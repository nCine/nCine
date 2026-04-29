#ifndef CLASS_GRAIL_BINDGROUP
#define CLASS_GRAIL_BINDGROUP

#include "grail/handle_types.h"

namespace ncine {
namespace grail {

namespace BindGroup {

struct Entry
{
	uint32_t binding = 0;

	uint32_t arrayIndex = 0;

	Buffer::Handle buffer;
	/// This field is only used if the binding is not dynamic
	uint64_t offset = 0;
	uint64_t size = 0;

	TextureView::Handle textureView;
};

struct Desc
{
	BindGroupLayout::Handle layout;

	const Entry *entries;
	uint32_t entryCount;

	const char *debugName = nullptr;
};

}

} // namespace grail
} // namespace ncine

#endif
