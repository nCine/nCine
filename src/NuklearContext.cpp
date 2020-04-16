#include "NuklearContext.h"
#include "nuklear.h"

#if WITH_ALLOCATORS
	#include <nctl/AllocManager.h>
#endif

namespace ncine {

#if WITH_ALLOCATORS
void *nuklearAllocateFunc(nk_handle handle, void *old, nk_size sz)
{
	return nctl::theNuklearAllocator().reallocate(old, sz);
}

void nuklearFreeFunc(nk_handle handle, void *ptr)
{
	return nctl::theNuklearAllocator().deallocate(ptr);
}
#endif

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

int NuklearContext::lastFrameWidth_ = 0;
int NuklearContext::lastFrameHeight_ = 0;

struct nk_context NuklearContext::ctx_;
struct nk_buffer NuklearContext::cmds_;
struct nk_font_atlas NuklearContext::atlas_;
struct nk_vec2 NuklearContext::fbScale_;

int NuklearContext::width_;
int NuklearContext::height_;
int NuklearContext::displayWidth_;
int NuklearContext::displayHeight_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void NuklearContext::init()
{
#if WITH_ALLOCATORS
	nk_allocator nuklearAlloc{ { nullptr }, nuklearAllocateFunc, nuklearFreeFunc };
	nk_init(&ctx_, &nuklearAlloc, nullptr);
	nk_buffer_init(&cmds_, &nuklearAlloc, 4 * 1024); // NK_BUFFER_DEFAULT_INITIAL_SIZE
	nk_font_atlas_init(&atlas_, &nuklearAlloc);
#else
	nk_init_default(&ctx_, nullptr);
	nk_buffer_init_default(&cmds_);
	nk_font_atlas_init_default(&atlas_);
#endif
}

void NuklearContext::shutdown()
{
	nk_font_atlas_clear(&atlas_);
	nk_buffer_free(&cmds_);
	nk_free(&ctx_);
}

struct nk_context *NuklearContext::context()
{
	return &ctx_;
}

struct nk_font_atlas *NuklearContext::atlas()
{
	return &atlas_;
}

}
