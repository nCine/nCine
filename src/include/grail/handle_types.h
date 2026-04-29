#ifndef GRAIL_HANDLETYPES
#define GRAIL_HANDLETYPES

#include "grail/Handle.h"

#define GRL_DEFINE_HANDLE_NAMESPACE(Name) \
namespace Name { \
	struct Tag {}; \
	using Handle = Handle<Tag>; \
}

namespace ncine {
namespace grail {

GRL_DEFINE_HANDLE_NAMESPACE(BindGroupLayout)
GRL_DEFINE_HANDLE_NAMESPACE(BindGroup)
GRL_DEFINE_HANDLE_NAMESPACE(GraphicsPipeline)
GRL_DEFINE_HANDLE_NAMESPACE(ComputePipeline)
GRL_DEFINE_HANDLE_NAMESPACE(Buffer)
GRL_DEFINE_HANDLE_NAMESPACE(Texture)
GRL_DEFINE_HANDLE_NAMESPACE(TextureView)

} // namespace grail
} // namespace ncine

#undef GRL_DEFINE_HANDLE_NAMESPACE

#endif
