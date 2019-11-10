#include "NuklearContext.h"
#include "nuklear.h"

namespace ncine {

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

struct nk_context *NuklearContext::context()
{
	return &ctx_;
}

struct nk_font_atlas *NuklearContext::atlas()
{
	return &atlas_;
}

}
