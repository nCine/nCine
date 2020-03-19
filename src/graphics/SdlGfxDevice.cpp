#ifdef WITH_GLEW
	#define GLEW_NO_GLU
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

SdlGfxDevice::SdlGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode)
    : IGfxDevice(windowMode, glContextInfo, displayMode)
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

void SdlGfxDevice::setSwapInterval(int interval)
{
	SDL_GL_SetSwapInterval(interval);
}

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

		unsigned int flags = SDL_GetWindowFlags(windowHandle_);
		if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
		{
			isFullScreen_ = true;
			SDL_SetWindowFullscreen(windowHandle_, SDL_WINDOW_FULLSCREEN);
		}
		SDL_SetWindowSize(windowHandle_, width, height);
	}
}

void SdlGfxDevice::setFullScreen(bool fullScreen)
{
	if (isFullScreen_ != fullScreen)
	{
		isFullScreen_ = fullScreen;

		const int flags = isFullScreen_ ? SDL_WINDOW_FULLSCREEN : 0;
		SDL_SetWindowFullscreen(windowHandle_, flags);
	}
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

const IGfxDevice::VideoMode &SdlGfxDevice::currentVideoMode() const
{
	SDL_DisplayMode mode;
	SDL_GetCurrentDisplayMode(0, &mode);
	convertVideoModeInfo(mode, currentVideoMode_);

	return currentVideoMode_;
}

bool SdlGfxDevice::setVideoMode(unsigned int index)
{
	ASSERT(index < numVideoModes_);

	int modeIndex = index;
	if (index >= numVideoModes_)
		modeIndex = 0;

	SDL_DisplayMode mode;
	SDL_GetDisplayMode(0, modeIndex, &mode);

	return SDL_SetWindowDisplayMode(windowHandle_, &mode);
}

void SdlGfxDevice::updateVideoModes()
{
	const int count = SDL_GetNumDisplayModes(0);
	numVideoModes_ = (count < MaxVideoModes) ? count : MaxVideoModes;

	SDL_DisplayMode mode;
	for (unsigned int i = 0; i < numVideoModes_; i++)
	{
		SDL_GetDisplayMode(0, i, &mode);
		convertVideoModeInfo(mode, videoModes_[i]);
	}
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
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, displayMode_.redBits());
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, displayMode_.greenBits());
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, displayMode_.blueBits());
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, displayMode_.alphaBits());
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, displayMode_.isDoubleBuffered());
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, displayMode_.depthBits());
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, displayMode_.stencilBits());
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glContextInfo_.majorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glContextInfo_.minorVersion);
#if defined(WITH_ANGLE)
	SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#elif defined(__EMSCRIPTEN__)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, glContextInfo_.coreProfile
	                                                     ? SDL_GL_CONTEXT_PROFILE_CORE
	                                                     : SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
#endif
	if (glContextInfo_.forwardCompatible == false)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	if (glContextInfo_.debugContext)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	Uint32 flags = SDL_WINDOW_OPENGL;
	if (isFullScreen_)
		flags |= SDL_WINDOW_FULLSCREEN;
	else if (width_ == 0 || height_ == 0)
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	// Creating a window with SDL2
	windowHandle_ = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_, height_, flags);
	FATAL_ASSERT_MSG_X(windowHandle_, "SDL_CreateWindow failed: %s", SDL_GetError());

	SDL_SetWindowResizable(windowHandle_, isResizable_ ? SDL_TRUE : SDL_FALSE);

	// resolution should be set to current screen size
	if (width_ == 0 || height_ == 0)
		SDL_GetWindowSize(windowHandle_, &width_, &height_);

	glContextHandle_ = SDL_GL_CreateContext(windowHandle_);
	FATAL_ASSERT_MSG_X(glContextHandle_, "SDL_GL_CreateContext failed: %s", SDL_GetError());

	const int interval = displayMode_.hasVSync() ? 1 : 0;
	SDL_GL_SetSwapInterval(interval);

#ifdef WITH_GLEW
	const GLenum err = glewInit();
	FATAL_ASSERT_MSG_X(err == GLEW_OK, "GLEW error: %s", glewGetErrorString(err));

	glContextInfo_.debugContext = glContextInfo_.debugContext && glewIsSupported("GL_ARB_debug_output");
#endif

	updateVideoModes();
}

void SdlGfxDevice::convertVideoModeInfo(const SDL_DisplayMode &sdlVideoMode, IGfxDevice::VideoMode &videoMode) const
{
	videoMode.width = static_cast<unsigned int>(sdlVideoMode.w);
	videoMode.height = static_cast<unsigned int>(sdlVideoMode.h);
	videoMode.refreshRate = static_cast<unsigned int>(sdlVideoMode.refresh_rate);

	switch (sdlVideoMode.format)
	{
		case SDL_PIXELFORMAT_RGB332:
			videoMode.redBits = 3;
			videoMode.greenBits = 3;
			videoMode.blueBits = 2;
			break;
		case SDL_PIXELFORMAT_RGB444:
		case SDL_PIXELFORMAT_ARGB4444:
		case SDL_PIXELFORMAT_RGBA4444:
		case SDL_PIXELFORMAT_ABGR4444:
		case SDL_PIXELFORMAT_BGRA4444:
			videoMode.redBits = 4;
			videoMode.greenBits = 4;
			videoMode.blueBits = 4;
			break;
		case SDL_PIXELFORMAT_RGB555:
		case SDL_PIXELFORMAT_BGR555:
		case SDL_PIXELFORMAT_ARGB1555:
		case SDL_PIXELFORMAT_RGBA5551:
		case SDL_PIXELFORMAT_ABGR1555:
		case SDL_PIXELFORMAT_BGRA5551:
			videoMode.redBits = 5;
			videoMode.greenBits = 5;
			videoMode.blueBits = 5;
			break;
		case SDL_PIXELFORMAT_RGB565:
		case SDL_PIXELFORMAT_BGR565:
			videoMode.redBits = 5;
			videoMode.greenBits = 6;
			videoMode.blueBits = 5;
			break;
		case SDL_PIXELFORMAT_RGB24:
		case SDL_PIXELFORMAT_BGR24:
		case SDL_PIXELFORMAT_RGB888:
		case SDL_PIXELFORMAT_RGBX8888:
		case SDL_PIXELFORMAT_BGR888:
		case SDL_PIXELFORMAT_BGRX8888:
		case SDL_PIXELFORMAT_ARGB8888:
		case SDL_PIXELFORMAT_RGBA8888:
		case SDL_PIXELFORMAT_ABGR8888:
		case SDL_PIXELFORMAT_BGRA8888:
		default:
			videoMode.redBits = 8;
			videoMode.greenBits = 8;
			videoMode.blueBits = 8;
			break;
		case SDL_PIXELFORMAT_ARGB2101010:
			videoMode.redBits = 10;
			videoMode.greenBits = 10;
			videoMode.blueBits = 10;
			break;
	}
}

}
