#ifdef WITH_GLEW
	#include <GL/glew.h>
#endif

#include "common_macros.h"
#include "SdlGfxDevice.h"
#include "ITextureLoader.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

SDL_Window *SdlGfxDevice::windowHandle_ = nullptr;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

SdlGfxDevice::SdlGfxDevice(int width, int height, const GLContextInfo &contextInfo, const DisplayMode &mode, bool isFullScreen)
	: IGfxDevice(width, height, contextInfo, mode, isFullScreen)
{
	initGraphics();
	initDevice();
}

SdlGfxDevice::~SdlGfxDevice()
{
	SDL_GL_DeleteContext(glContextHandle_);
	glContextHandle_ = nullptr;
	SDL_DestroyWindow(windowHandle_);
	windowHandle_ = nullptr;
	SDL_Quit();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void SdlGfxDevice::setResolution(int width, int height)
{
	// change resolution only in the case it really changes
	if (width == width_ && height == height_)
		return;

	// asking for fullscreen mode that does not change current screen resolution
	if (width == 0 || height == 0)
	{
		SDL_SetWindowFullscreen(windowHandle_, SDL_WINDOW_FULLSCREEN_DESKTOP);
		SDL_GetWindowSize(windowHandle_, &width_, &height_);
	}
	else
	{
		width_ = width;
		height_ = height;

		initDevice();
	}
}

void SdlGfxDevice::toggleFullScreen()
{
	isFullScreen_ = !isFullScreen_;

	const int flags = isFullScreen_ ? SDL_WINDOW_FULLSCREEN : 0;
	SDL_SetWindowFullscreen(windowHandle_, flags);
}

void SdlGfxDevice::setWindowIcon(const char *windowIconFilename)
{
	nctl::UniquePtr<ITextureLoader> image = ITextureLoader::createFromFile(windowIconFilename);
	const Uint32 pixelFormat = (image->bpp() == 4) ? SDL_PIXELFORMAT_ABGR8888 : SDL_PIXELFORMAT_BGR888;

	SDL_Surface *surface = nullptr;
	const int pitch = image->width() * image->bpp();
	void *pixels = reinterpret_cast<void *>(const_cast<GLubyte *>(image->pixels()));
	surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, image->width(), image->height(), image->bpp() * 8, pitch, pixelFormat);
	SDL_SetWindowIcon(windowHandle_, surface);
	SDL_FreeSurface(surface);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void SdlGfxDevice::initGraphics()
{
	const int err = SDL_Init(SDL_INIT_VIDEO);
	FATAL_ASSERT_MSG_X(!err, "SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
}

void SdlGfxDevice::initDevice()
{
	// asking for a video mode that does not change current screen resolution
	if (width_ == 0 || height_ == 0)
	{
		width_ = 0;
		height_ = 0;
	}

	// setting OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, mode_.redBits());
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, mode_.greenBits());
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, mode_.blueBits());
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, mode_.alphaBits());
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, mode_.isDoubleBuffered());
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, mode_.depthBits());
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, mode_.stencilBits());
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, contextInfo_.majorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, contextInfo_.minorVersion);
	if (contextInfo_.debugContext)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	Uint32 flags = SDL_WINDOW_OPENGL;
	if (isFullScreen_)
		flags |= SDL_WINDOW_FULLSCREEN;
	else if (width_ == 0 || height_ == 0)
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	// Creating a window with SDL2
	windowHandle_ = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_, height_, flags);
	FATAL_ASSERT_MSG_X(windowHandle_, "SDL_CreateWindow failed: %s", SDL_GetError());

	// resolution should be set to current screen size
	if (width_ == 0 || height_ == 0)
		SDL_GetWindowSize(windowHandle_, &width_, &height_);

	glContextHandle_ = SDL_GL_CreateContext(windowHandle_);
	FATAL_ASSERT_MSG_X(glContextHandle_, "SDL_GL_CreateContext failed: %s", SDL_GetError());

	const int interval = mode_.hasVSync() ? 1 : 0;
	SDL_GL_SetSwapInterval(interval);

#ifdef WITH_GLEW
	const GLenum err = glewInit();
	FATAL_ASSERT_MSG_X(err == GLEW_OK, "GLEW error: %s", glewGetErrorString(err));
#endif
}

}
