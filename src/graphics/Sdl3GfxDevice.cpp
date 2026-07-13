#ifdef WITH_GLEW
	#define GLEW_NO_GLU
	#include <GL/glew.h>
#endif
#include <SDL3/SDL.h>

#include "common_macros.h"
#include "SdlGfxDevice.h"
#include "IImageLoader.h"

#ifdef __EMSCRIPTEN__
	#include <emscripten/html5.h>
#endif

namespace ncine {

namespace {

int getDisplayIndexForWindow(SDL_Window *windowHandle, const SDL_DisplayID *displayIDs, int displayIDCount)
{
	int displayIndex = -1;
	const SDL_DisplayID displayID = SDL_GetDisplayForWindow(windowHandle);
	if (displayID != 0)
	{
		for (int i = 0; i < displayIDCount; i++)
		{
			if (displayIDs[i] == displayID)
			{
				displayIndex = i;
				break;
			}
		}
	}

	return displayIndex;
}

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

SDL_Window *SdlGfxDevice::windowHandle_ = nullptr;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

SdlGfxDevice::SdlGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode)
    : IGfxDevice(windowMode, glContextInfo, displayMode), displayIDCount_(0), displayIDs_(nullptr)
{
#if defined(_WIN32)
	if (windowMode.hasWindowScaling)
	{
		// Disable automatic window scaling, SDL will take care of it
		backendScalesWindowSize_ = true;
	}
#endif

	initGraphics();
	initWindowScaling(windowMode);
	initDevice(windowMode);
}

SdlGfxDevice::~SdlGfxDevice()
{
	SDL_free(displayIDs_);
	displayIDs_ = nullptr;
	displayIDCount_ = 0;

	SDL_GL_DestroyContext(glContextHandle_);
	glContextHandle_ = nullptr;
	SDL_DestroyWindow(windowHandle_);
	windowHandle_ = nullptr;

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void SdlGfxDevice::setSwapInterval(int interval)
{
	SDL_GL_SetSwapInterval(interval);
}

void SdlGfxDevice::setFullscreen(bool fullscreen)
{
	if (isFullscreen_ == fullscreen)
		return;

	isFullscreen_ = fullscreen;
	SDL_SetWindowFullscreen(windowHandle_, isFullscreen_);

	SDL_GetWindowSize(windowHandle_, &width_, &height_);
	SDL_GetWindowSizeInPixels(windowHandle_, &drawableWidth_, &drawableHeight_);
}

void SdlGfxDevice::setResizable(bool resizable)
{
	SDL_SetWindowResizable(windowHandle_, resizable ? true : false);
	isResizable_ = resizable;
}

int SdlGfxDevice::windowPositionX() const
{
	int posX = 0;
	SDL_GetWindowPosition(windowHandle_, &posX, nullptr);
	return posX;
}

int SdlGfxDevice::windowPositionY() const
{
	int posY = 0;
	SDL_GetWindowPosition(windowHandle_, nullptr, &posY);
	return posY;
}

const Vector2i SdlGfxDevice::windowPosition() const
{
	Vector2i position(0, 0);
	SDL_GetWindowPosition(windowHandle_, &position.x, &position.y);
	return position;
}

inline void SdlGfxDevice::setWindowPosition(int x, int y)
{
	SDL_SetWindowPosition(windowHandle_, x, y);
}

void SdlGfxDevice::setWindowSize(int width, int height)
{
	// change resolution only in case it is valid and it really changes
	if (width <= 0 || height <= 0 || (width == width_ && height == height_))
		return;

	const SDL_WindowFlags flags = SDL_GetWindowFlags(windowHandle_);
	// Not in desktop fullscreen, either windowed or using exclusive fullscreen
	if ((flags & SDL_WINDOW_FULLSCREEN) == 0 || SDL_GetWindowFullscreenMode(windowHandle_) != nullptr)
	{
		SDL_SetWindowSize(windowHandle_, width, height);
		SDL_GetWindowSize(windowHandle_, &width_, &height_);
		SDL_GetWindowSizeInPixels(windowHandle_, &drawableWidth_, &drawableHeight_);
	}
}

void SdlGfxDevice::setWindowTitle(const char *windowTitle)
{
	SDL_SetWindowTitle(windowHandle_, windowTitle);
}

void SdlGfxDevice::setWindowIcon(const char *windowIconFilename)
{
	nctl::UniquePtr<IImageLoader> image = IImageLoader::createFromFile(windowIconFilename);
	const unsigned int bytesPerPixel = image->numChannels();
	const SDL_PixelFormat pixelFormat = (bytesPerPixel == 4) ? SDL_PIXELFORMAT_ABGR8888 : SDL_PIXELFORMAT_XBGR8888;

	SDL_Surface *surface = nullptr;
	const int pitch = image->width() * bytesPerPixel;
	void *pixels = reinterpret_cast<void *>(image->pixels());
	surface = SDL_CreateSurfaceFrom(image->width(), image->height(), pixelFormat, pixels, pitch);
	SDL_SetWindowIcon(windowHandle_, surface);
	SDL_DestroySurface(surface);
}

void SdlGfxDevice::flashWindow() const
{
#if !defined(__EMSCRIPTEN__)
	SDL_FlashWindow(windowHandle_, SDL_FLASH_UNTIL_FOCUSED);
#endif
}

unsigned int SdlGfxDevice::windowMonitorIndex() const
{
	const int retrievedIndex = windowHandle_ ? getDisplayIndexForWindow(windowHandle_, displayIDs_, displayIDCount_) : 0;
	const unsigned int index = (retrievedIndex >= 0) ? static_cast<unsigned int>(retrievedIndex) : 0;
	return index;
}

const IGfxDevice::VideoMode &SdlGfxDevice::currentVideoMode(unsigned int monitorIndex) const
{
	if (monitorIndex >= numMonitors_)
		monitorIndex = 0;

	const SDL_DisplayID displayID = displayIDs_[monitorIndex];
	const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayID);
	convertVideoModeInfo(*mode, currentVideoMode_);

	return currentVideoMode_;
}

bool SdlGfxDevice::setVideoMode(unsigned int modeIndex)
{
	int displayIndex = getDisplayIndexForWindow(windowHandle_, displayIDs_, displayIDCount_);
	if (displayIndex < 0 || displayIndex >= numMonitors_)
		displayIndex = 0;

	if (modeIndex < monitors_[displayIndex].numVideoModes)
	{
		const SDL_DisplayID displayID = displayIDs_[displayIndex];
		const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayID);
		return SDL_SetWindowFullscreenMode(windowHandle_, mode);
	}
	return false;
}

void SdlGfxDevice::swapBuffers()
{
	SDL_GL_SwapWindow(windowHandle_);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void SdlGfxDevice::initGraphics()
{
	const bool success = SDL_InitSubSystem(SDL_INIT_VIDEO);
	FATAL_ASSERT_MSG_X(success == true, "SDL_InitSubSystem(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
}

void SdlGfxDevice::initDevice(const WindowMode &windowMode)
{
	// At this point `updateMonitors()` has already been called by `initWindowScaling()`

	// Setting OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, displayMode_.redBits());
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, displayMode_.greenBits());
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, displayMode_.blueBits());
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, displayMode_.alphaBits());
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, displayMode_.isDoubleBuffered());
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, displayMode_.depthBits());
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, displayMode_.stencilBits());
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glContextInfo_.majorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glContextInfo_.minorVersion);
#if defined(WITH_OPENGLES)
	SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#elif defined(__EMSCRIPTEN__)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, glContextInfo_.coreProfile ? SDL_GL_CONTEXT_PROFILE_CORE : SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
#endif
	if (glContextInfo_.forwardCompatible == false)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	if (glContextInfo_.debugContext)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	Uint32 flags = SDL_WINDOW_OPENGL;
#ifndef __EMSCRIPTEN__
	if (windowMode.hasWindowScaling)
		flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
#endif

#ifndef __EMSCRIPTEN__
	// If the window size is zero or negative, then the application will go fullscreen
	if (width_ <= 0 || height_ <= 0)
		isFullscreen_ = true;

	const bool ignoreAnyWindowPosition = (windowMode.windowPositionX == AppConfiguration::Window::IgnorePosition ||
	                                      windowMode.windowPositionY == AppConfiguration::Window::IgnorePosition);
	const Vector2i windowCenter(windowMode.windowPositionX + windowMode.width / 2, windowMode.windowPositionY + windowMode.height / 2);
	const unsigned int monitorIndex = ignoreAnyWindowPosition ? 0 : containingMonitorIndex(windowCenter);
	// If fullscreen is requested, current video mode will not be changed
	const bool desktopFullscreen = isFullscreen_ && (windowMode.refreshRate <= 0.0f) && (windowMode.width <= 0 || windowMode.height <= 0);

	bool closestModeFound = false;
	SDL_DisplayMode closestMode;

	const SDL_DisplayID displayID = displayIDs_[monitorIndex];
	if (isFullscreen_ && desktopFullscreen == false)
	{
		const SDL_DisplayMode *currentMode = SDL_GetCurrentDisplayMode(displayID);

		const int targetWidth = (windowMode.width > 0) ? windowMode.width : currentMode->w;
		const int targetHeight = (windowMode.height > 0) ? windowMode.height : currentMode->h;
		const float targetRefreshRate = (windowMode.refreshRate > 0.0f) ? windowMode.refreshRate : currentMode->refresh_rate;
		const bool includeHighDensityModes = false;
		closestModeFound = SDL_GetClosestFullscreenDisplayMode(displayID, targetWidth, targetHeight, targetRefreshRate, includeHighDensityModes, &closestMode);
		ASSERT_MSG_X(closestModeFound, "Unable to find a suitable fullscreen display mode");
	}
#endif

	const bool windowPositionIsValid = (containingMonitorIndex(windowMode) != -1);

	const int windowPosX = (windowMode.windowPositionX != AppConfiguration::Window::IgnorePosition && windowPositionIsValid)
	        ? windowMode.windowPositionX : SDL_WINDOWPOS_CENTERED;
	const int windowPosY = (windowMode.windowPositionY != AppConfiguration::Window::IgnorePosition && windowPositionIsValid)
	        ? windowMode.windowPositionY : SDL_WINDOWPOS_CENTERED;

	windowHandle_ = SDL_CreateWindow("", width_, height_, flags);
	FATAL_ASSERT_MSG_X(windowHandle_, "SDL_CreateWindow failed: %s", SDL_GetError());

	SDL_SetWindowPosition(windowHandle_, windowPosX, windowPosY);

#ifndef __EMSCRIPTEN__
	if (isFullscreen_)
	{
		bool success = false;

		if (desktopFullscreen)
			success = SDL_SetWindowFullscreenMode(windowHandle_, nullptr);
		else if (closestModeFound == true)
			success = SDL_SetWindowFullscreenMode(windowHandle_, &closestMode);

		ASSERT_MSG_X(success, "SDL_SetWindowFullscreenMode failed: %s", SDL_GetError());

		success = SDL_SetWindowFullscreen(windowHandle_, true);
		ASSERT_MSG_X(success, "SDL_SetWindowFullscreen failed: %s", SDL_GetError());
	}
	else
	{
		// Default mode used by a later call to setFullscreen(true)
		const SDL_DisplayMode *currentMode = SDL_GetCurrentDisplayMode(displayID);
		const bool success = SDL_SetWindowFullscreenMode(windowHandle_, currentMode);
		ASSERT_MSG_X(success, "SDL_SetWindowFullscreenMode failed: %s", SDL_GetError());
	}
#endif

	SDL_GetWindowSize(windowHandle_, &width_, &height_);
	SDL_GetWindowSizeInPixels(windowHandle_, &drawableWidth_, &drawableHeight_);
	initGLViewport();

	SDL_SetWindowResizable(windowHandle_, isResizable_);

	glContextHandle_ = SDL_GL_CreateContext(windowHandle_);
	FATAL_ASSERT_MSG_X(glContextHandle_, "SDL_GL_CreateContext failed: %s", SDL_GetError());

	const int interval = displayMode_.hasVSync() ? 1 : 0;
	SDL_GL_SetSwapInterval(interval);

#ifdef WITH_GLEW
	const GLenum err = glewInit();
	FATAL_ASSERT_MSG_X(err == GLEW_OK, "GLEW error: %s", glewGetErrorString(err));

	glContextInfo_.debugContext = glContextInfo_.debugContext && glewIsSupported("GL_ARB_debug_output");
#endif
}

void SdlGfxDevice::updateMonitors()
{
	SDL_free(displayIDs_);

	displayIDs_ = SDL_GetDisplays(&displayIDCount_);
	FATAL_ASSERT_MSG_X(displayIDs_ != nullptr, "SDL_GetDisplays failed: %s", SDL_GetError());
	ASSERT(displayIDCount_ >= 1);
	numMonitors_ = (displayIDCount_ < MaxMonitors) ? displayIDCount_ : MaxMonitors;

	for (unsigned int i = 0; i < numMonitors_; i++)
	{
		SDL_DisplayID displayID = displayIDs_[i];
		monitors_[i].name = SDL_GetDisplayName(displayID);
		ASSERT(monitors_[i].name != nullptr);

		SDL_Rect bounds;
		SDL_GetDisplayBounds(displayID, &bounds);
		monitors_[i].position.x = bounds.x;
		monitors_[i].position.y = bounds.y;

		const float displayScale = SDL_GetDisplayContentScale(displayID);
		monitors_[i].dpi.x = displayScale * DefaultDpi;
		monitors_[i].dpi.y = displayScale * DefaultDpi;
		monitors_[i].scale.x = displayScale;
		monitors_[i].scale.y = displayScale;

		int modeCount = 0;
		SDL_DisplayMode **displayModes = SDL_GetFullscreenDisplayModes(displayID, &modeCount);
		monitors_[i].numVideoModes = (modeCount < MaxVideoModes) ? modeCount : MaxVideoModes;

		for (unsigned int j = 0; j < monitors_[i].numVideoModes; j++)
		{
			const SDL_DisplayMode &mode = *displayModes[j];
			convertVideoModeInfo(mode, monitors_[i].videoModes[j]);
		}
		SDL_free(displayModes);
	}
}

void SdlGfxDevice::convertVideoModeInfo(const SDL_DisplayMode &sdlVideoMode, IGfxDevice::VideoMode &videoMode) const
{
#ifndef __EMSCRIPTEN__
	videoMode.width = static_cast<unsigned int>(sdlVideoMode.w);
	videoMode.height = static_cast<unsigned int>(sdlVideoMode.h);
#else
	double cssWidth = 0.0;
	double cssHeight = 0.0;

	// Referring to the first element of type <canvas> in the DOM
	emscripten_get_element_css_size("#canvas", &cssWidth, &cssHeight);
	videoMode.width = static_cast<unsigned int>(cssWidth);
	videoMode.height = static_cast<unsigned int>(cssHeight);
#endif
	videoMode.refreshRate = static_cast<float>(sdlVideoMode.refresh_rate);

	switch (sdlVideoMode.format)
	{
		case SDL_PIXELFORMAT_RGB332:
			videoMode.redBits = 3;
			videoMode.greenBits = 3;
			videoMode.blueBits = 2;
			break;
		case SDL_PIXELFORMAT_XRGB4444:
		case SDL_PIXELFORMAT_ARGB4444:
		case SDL_PIXELFORMAT_RGBA4444:
		case SDL_PIXELFORMAT_ABGR4444:
		case SDL_PIXELFORMAT_BGRA4444:
			videoMode.redBits = 4;
			videoMode.greenBits = 4;
			videoMode.blueBits = 4;
			break;
		case SDL_PIXELFORMAT_XRGB1555:
		case SDL_PIXELFORMAT_XBGR1555:
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
		case SDL_PIXELFORMAT_XRGB8888:
		case SDL_PIXELFORMAT_RGBX8888:
		case SDL_PIXELFORMAT_XBGR8888:
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
