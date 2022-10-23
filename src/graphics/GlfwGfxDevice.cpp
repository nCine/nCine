#include "common_macros.h"
#include "GlfwGfxDevice.h"
#include "GlfwInputManager.h"
#include "ITextureLoader.h"

#ifdef __EMSCRIPTEN__
	#include <emscripten/html5.h>
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLFWwindow *GlfwGfxDevice::windowHandle_ = nullptr;
GLFWmonitor *GlfwGfxDevice::monitorPointers_[MaxMonitors];
int GlfwGfxDevice::fsMonitorIndex_ = -1;
int GlfwGfxDevice::fsModeIndex_ = -1;
#ifdef _WIN32
int GlfwGfxDevice::swapInterval_ = -1;
#endif

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GlfwGfxDevice::GlfwGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode)
    : IGfxDevice(windowMode, glContextInfo, displayMode)
{
	initGraphics();
	initWindowScaling(windowMode);
	initDevice();
}

GlfwGfxDevice::~GlfwGfxDevice()
{
	glfwDestroyWindow(windowHandle_);
	windowHandle_ = nullptr;
	glfwTerminate();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GlfwGfxDevice::setSwapInterval(int interval)
{
	glfwSwapInterval(interval);
#ifdef _WIN32
	// The swap interval is reset after going full screen on Windows (https://github.com/glfw/glfw/issues/1072)
	swapInterval_ = interval;
#endif
}

void GlfwGfxDevice::setFullScreen(bool fullScreen)
{
	if (isFullScreen_ == fullScreen)
		return;

	if (fsMonitorIndex_ < 0 || fsMonitorIndex_ > numMonitors_)
		fsMonitorIndex_ = windowMonitorIndex();

	// The windows goes in full screen on the monitor it was on on the last call to `setVideoMode()`
	GLFWmonitor *monitor = fullScreen ? monitorPointers_[fsMonitorIndex_] : nullptr;

	const GLFWvidmode *currentMode = glfwGetVideoMode(monitorPointers_[fsMonitorIndex_]);
	int width = (monitor != nullptr) ? currentMode->width : width_;
	int height = (monitor != nullptr) ? currentMode->height : height_;
	int refreshRate = (monitor != nullptr) ? currentMode->refreshRate : GLFW_DONT_CARE;

	if (fsModeIndex_ >= 0 && fsModeIndex_ < monitors_[fsMonitorIndex_].numVideoModes && fullScreen)
	{
		const IGfxDevice::VideoMode &mode = monitors_[fsMonitorIndex_].videoModes[fsModeIndex_];
		width = mode.width;
		height = mode.height;
		refreshRate = mode.refreshRate;
	}

#ifndef __EMSCRIPTEN__
	glfwSetWindowMonitor(windowHandle_, monitor, 0, 0, width, height, refreshRate);
	#ifdef _WIN32
	// The swap interval is reset after going full screen on Windows (https://github.com/glfw/glfw/issues/1072)
	if (isFullScreen_)
		glfwSwapInterval(swapInterval_);
	#endif
#else
	EmscriptenFullscreenChangeEvent fsce;
	emscripten_get_fullscreen_status(&fsce);
	if (fullScreen)
	{
		// On Emscripten, requesting full screen on GLFW is done by changing the window size to the screen size
		glfwSetWindowSize(windowHandle_, fsce.screenWidth, fsce.screenHeight);
	}
	else
		emscripten_exit_fullscreen();
#endif
	isFullScreen_ = fullScreen;
	glfwGetWindowSize(windowHandle_, &width_, &height_);
	glfwGetFramebufferSize(windowHandle_, &drawableWidth_, &drawableHeight_);
}

int GlfwGfxDevice::windowPositionX() const
{
	int posX = 0;
	glfwGetWindowPos(windowHandle_, &posX, nullptr);
	return posX;
}

int GlfwGfxDevice::windowPositionY() const
{
	int posY = 0;
	glfwGetWindowPos(windowHandle_, nullptr, &posY);
	return posY;
}

const Vector2i GlfwGfxDevice::windowPosition() const
{
	Vector2i position(0, 0);
	glfwGetWindowPos(windowHandle_, &position.x, &position.y);
	return position;
}

void GlfwGfxDevice::setWindowPosition(int x, int y)
{
	int width = width_;
	int height = height_;
	glfwGetWindowSize(windowHandle_, &width_, &height_);

	glfwSetWindowSizeCallback(windowHandle_, nullptr);
	glfwSetFramebufferSizeCallback(windowHandle_, nullptr);

	glfwSetWindowPos(windowHandle_, x, y);
	glfwSetWindowSize(windowHandle_, width, height);

	glfwSetWindowSizeCallback(windowHandle_, GlfwInputManager::windowSizeCallback);
	glfwSetFramebufferSizeCallback(windowHandle_, GlfwInputManager::framebufferSizeCallback);
}

void GlfwGfxDevice::setWindowSize(int width, int height)
{
	// change resolution only in case it is valid and it really changes
	if (width == 0 || height == 0 || (width == width_ && height == height_))
		return;

	if (glfwGetWindowMonitor(windowHandle_) == nullptr)
	{
		glfwSetWindowSize(windowHandle_, width, height);
		glfwGetWindowSize(windowHandle_, &width_, &height_);
		glfwGetFramebufferSize(windowHandle_, &drawableWidth_, &drawableHeight_);
	}
}

void GlfwGfxDevice::setWindowIcon(const char *windowIconFilename)
{
	nctl::UniquePtr<ITextureLoader> image = ITextureLoader::createFromFile(windowIconFilename);
	GLFWimage glfwImage;
	glfwImage.width = image->width();
	glfwImage.height = image->height();
	glfwImage.pixels = const_cast<unsigned char *>(image->pixels());

	glfwSetWindowIcon(windowHandle_, 1, &glfwImage);
}

void GlfwGfxDevice::flashWindow() const
{
#if (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300)
	glfwRequestWindowAttention(windowHandle_);
#endif
}

unsigned int GlfwGfxDevice::primaryMonitorIndex() const
{
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();

	const int retrievedIndex = retrieveMonitorIndex(monitor);
	const unsigned int index = (retrievedIndex >= 0) ? static_cast<unsigned int>(retrievedIndex) : 0;
	return index;
}

unsigned int GlfwGfxDevice::windowMonitorIndex() const
{
	if (numMonitors_ == 1 || windowHandle_ == nullptr)
		return 0;

	GLFWmonitor *monitor = glfwGetWindowMonitor(windowHandle_);
	if (monitor == nullptr)
	{
		// Fallback value if a monitor containing the window cannot be found
		monitor = glfwGetPrimaryMonitor();

		Vector2i position(0, 0);
		glfwGetWindowPos(windowHandle_, &position.x, &position.y);
		Vector2i size(0, 0);
		glfwGetWindowSize(windowHandle_, &size.x, &size.y);
		const Vector2i center = position + size / 2;

		for (unsigned int i = 0; i < numMonitors_; i++)
		{
			const VideoMode &videoMode = currentVideoMode(i);
			const Recti surface(monitors_[i].position, Vector2i(videoMode.width, videoMode.height));
			if (surface.contains(center))
			{
				monitor = monitorPointers_[i];
				break;
			}
		}
	}

	const int retrievedIndex = retrieveMonitorIndex(monitor);
	const unsigned int index = (retrievedIndex >= 0) ? static_cast<unsigned int>(retrievedIndex) : 0;
	return index;
}

const IGfxDevice::VideoMode &GlfwGfxDevice::currentVideoMode(unsigned int monitorIndex) const
{
	// Fallback if the index is not valid
	GLFWmonitor *monitor = windowHandle_ ? glfwGetWindowMonitor(windowHandle_) : nullptr;
	if (monitor == nullptr)
		monitor = glfwGetPrimaryMonitor();

	if (monitorIndex < numMonitors_)
		monitor = monitorPointers_[monitorIndex];

	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	if (mode != nullptr)
		convertVideoModeInfo(*mode, currentVideoMode_);

	return currentVideoMode_;
}

bool GlfwGfxDevice::setVideoMode(unsigned int modeIndex)
{
	const int monitorIndex = windowMonitorIndex();
	ASSERT(monitorIndex >= 0);

	const unsigned int numVideoModes = monitors_[monitorIndex].numVideoModes;
	ASSERT(modeIndex < numVideoModes);

	if (modeIndex < numVideoModes)
	{
		if (isFullScreen_)
		{
			GLFWmonitor *monitor = monitorPointers_[monitorIndex];
			const IGfxDevice::VideoMode &mode = monitors_[monitorIndex].videoModes[modeIndex];
			glfwSetWindowMonitor(windowHandle_, monitor, 0, 0, mode.width, mode.height, static_cast<int>(mode.refreshRate));
		}
		fsMonitorIndex_ = monitorIndex;
		fsModeIndex_ = modeIndex;

		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GlfwGfxDevice::initGraphics()
{
#if (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300)
	glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
#endif
	glfwSetErrorCallback(errorCallback);
	FATAL_ASSERT_MSG(glfwInit() == GL_TRUE, "glfwInit() failed");
}

void GlfwGfxDevice::initDevice()
{
	// At this point `updateMonitors()` has already been called by `initWindowScaling()`

	const GLFWvidmode *vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	// asking for a video mode that does not change current screen resolution
	if (width_ == 0 || height_ == 0)
	{
		width_ = vidMode->width;
		height_ = vidMode->height;
		isFullScreen_ = true;
	}

	GLFWmonitor *monitor = nullptr;
	if (isFullScreen_)
	{
		monitor = glfwGetPrimaryMonitor();
		glfwWindowHint(GLFW_REFRESH_RATE, vidMode->refreshRate);
	}

	// setting window hints and creating a window with GLFW
	glfwWindowHint(GLFW_RESIZABLE, isResizable_ ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, static_cast<int>(glContextInfo_.majorVersion));
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, static_cast<int>(glContextInfo_.minorVersion));
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, glContextInfo_.debugContext ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_RED_BITS, static_cast<int>(displayMode_.redBits()));
	glfwWindowHint(GLFW_GREEN_BITS, static_cast<int>(displayMode_.greenBits()));
	glfwWindowHint(GLFW_BLUE_BITS, static_cast<int>(displayMode_.blueBits()));
	glfwWindowHint(GLFW_ALPHA_BITS, static_cast<int>(displayMode_.alphaBits()));
	glfwWindowHint(GLFW_DEPTH_BITS, static_cast<int>(displayMode_.depthBits()));
	glfwWindowHint(GLFW_STENCIL_BITS, static_cast<int>(displayMode_.stencilBits()));
#if defined(WITH_OPENGLES)
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__EMSCRIPTEN__)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_FOCUSED, 1);
#else
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, glContextInfo_.forwardCompatible ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, glContextInfo_.coreProfile ? GLFW_OPENGL_CORE_PROFILE : GLFW_OPENGL_COMPAT_PROFILE);
#endif

	windowHandle_ = glfwCreateWindow(width_, height_, "", monitor, nullptr);
	FATAL_ASSERT_MSG(windowHandle_, "glfwCreateWindow() failed");
	glfwGetFramebufferSize(windowHandle_, &drawableWidth_, &drawableHeight_);
	initGLViewport();

	glfwMakeContextCurrent(windowHandle_);

	const int interval = displayMode_.hasVSync() ? 1 : 0;
	glfwSwapInterval(interval);

#ifdef WITH_GLEW
	const GLenum err = glewInit();
	FATAL_ASSERT_MSG_X(err == GLEW_OK, "GLEW error: %s", glewGetErrorString(err));

	glContextInfo_.debugContext = glContextInfo_.debugContext && glewIsSupported("GL_ARB_debug_output");
#endif
}

void GlfwGfxDevice::updateMonitors()
{
	int monitorCount = 0;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);
	ASSERT(monitorCount >= 1);
	numMonitors_ = (monitorCount < MaxMonitors) ? monitorCount : MaxMonitors;

	for (unsigned int i = 0; i < MaxMonitors; i++)
		monitorPointers_[i] = (i < numMonitors_) ? monitors[i] : nullptr;

	for (unsigned int i = 0; i < numMonitors_; i++)
	{
		GLFWmonitor *monitor = monitors[i];
		monitors_[i].name = glfwGetMonitorName(monitor);
		ASSERT(monitors_[i].name != nullptr);
		glfwGetMonitorPos(monitor, &monitors_[i].position.x, &monitors_[i].position.y);
#if (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300)
		glfwGetMonitorContentScale(monitor, &monitors_[i].scale.x, &monitors_[i].scale.y);
#elif defined(__EMSCRIPTEN__)
		monitors_[i].scale.x = emscripten_get_device_pixel_ratio();
		monitors_[i].scale.y = monitors_[i].scale.x;
#endif
		monitors_[i].dpi.x = DefaultDpi * monitors_[i].scale.x;
		monitors_[i].dpi.y = DefaultDpi * monitors_[i].scale.y;

		int modeCount = 0;
		const GLFWvidmode *modes = glfwGetVideoModes(monitor, &modeCount);
		monitors_[i].numVideoModes = (modeCount < MaxVideoModes) ? modeCount : MaxVideoModes;

		for (unsigned int j = 0; j < monitors_[i].numVideoModes; j++)
		{
			// Reverse GLFW video mode array to be consistent with SDL
			const int srcIndex = modeCount - 1 - j;
			convertVideoModeInfo(modes[srcIndex], monitors_[i].videoModes[j]);
		}

#if defined(__EMSCRIPTEN__)
		if (monitors_[0].numVideoModes == 0)
		{
			monitors_[0].numVideoModes = 1;
			monitors_[0].videoModes[0] = currentVideoMode_;
		}
#endif
	}

	fsMonitorIndex_ = -1;
	fsModeIndex_ = -1;
}

void GlfwGfxDevice::updateMonitorScaling(unsigned int monitorIndex)
{
	int monitorCount = 0;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if (monitorIndex < monitorCount)
	{
		IGfxDevice::Monitor &monitor = monitors_[monitorIndex];

#if (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300)
		glfwGetMonitorContentScale(monitors[monitorIndex], &monitor.scale.x, &monitor.scale.y);
#elif defined(__EMSCRIPTEN__)
		monitor.scale.x = emscripten_get_device_pixel_ratio();
		monitor.scale.y = monitor.scale.x;
#endif
		monitor.dpi.x = DefaultDpi * monitor.scale.x;
		monitor.dpi.y = DefaultDpi * monitor.scale.y;
	}
}

int GlfwGfxDevice::retrieveMonitorIndex(GLFWmonitor *monitor) const
{
	if (monitor == nullptr)
		return -1;

	int index = -1;
	for (unsigned int i = 0; i < numMonitors_; i++)
	{
		if (monitorPointers_[i] == monitor)
		{
			index = i;
			break;
		}
	}
	return index;
}

void GlfwGfxDevice::convertVideoModeInfo(const GLFWvidmode &glfwVideoMode, IGfxDevice::VideoMode &videoMode) const
{
	videoMode.width = static_cast<unsigned int>(glfwVideoMode.width);
	videoMode.height = static_cast<unsigned int>(glfwVideoMode.height);
	videoMode.refreshRate = static_cast<float>(glfwVideoMode.refreshRate);
	videoMode.redBits = static_cast<unsigned char>(glfwVideoMode.redBits);
	videoMode.greenBits = static_cast<unsigned char>(glfwVideoMode.greenBits);
	videoMode.blueBits = static_cast<unsigned char>(glfwVideoMode.blueBits);
}

void GlfwGfxDevice::errorCallback(int error, const char *description)
{
	LOGE_X("GLFW error %d: \"%s\"", error, description);
}

}
