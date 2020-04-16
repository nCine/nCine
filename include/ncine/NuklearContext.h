#ifndef CLASS_NCINE_NUKLEARCONTEXT
#define CLASS_NCINE_NUKLEARCONTEXT

#include "common_macros.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

struct nk_context;
struct nk_buffer;
struct nk_font_atlas;
struct nk_vec2;

namespace ncine {

/// The class that stores Nuklear related variables
class DLL_PUBLIC NuklearContext
{
  public:
	static void init();
	static void shutdown();

	static struct nk_context *context();
	static struct nk_font_atlas *atlas();

  private:
	static int lastFrameWidth_;
	static int lastFrameHeight_;

	static struct nk_context ctx_;
	static struct nk_buffer cmds_;
	static struct nk_font_atlas atlas_;
	static struct nk_vec2 fbScale_;

	static int width_;
	static int height_;
	static int displayWidth_;
	static int displayHeight_;

	friend class NuklearDrawing;
	friend class NuklearGlfwInput;
	friend class NuklearSdlInput;
	friend class NuklearQt5Input;
	friend class NuklearAndroidInput;
};

}

#endif
