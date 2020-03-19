#include "common_macros.h"
#include "GlfwGfxDevice.h"
#include "ITextureLoader.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLFWwindow *GlfwGfxDevice::windowHandle_ = nullptr;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GlfwGfxDevice::GlfwGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode)
    : IGfxDevice(windowMode, glContextInfo, displayMode)
{
	initGraphics();
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
}

void GlfwGfxDevice::setResolution(int width, int height)
{
	// change resolution only in the case it really changes
	if (width == width_ && height == height_)
		return;

	// asking for fullscreen mode that does not change current screen resolution
	if (width == 0 || height == 0)
	{
		GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);
		width_ = mode->width;
		height_ = mode->height;
		isFullScreen_ = true;
		glfwSetWindowMonitor(windowHandle_, primaryMonitor, 0, 0, width_, height_, GLFW_DONT_CARE);
	}
	else
	{
		width_ = width;
		height_ = height;
		glfwSetWindowSize(windowHandle_, width, height);
	}
}

void GlfwGfxDevice::setFullScreen(bool fullScreen)
{
	if (isFullScreen_ != fullScreen)
	{
		isFullScreen_ = fullScreen;

		GLFWmonitor *monitor = isFullScreen_ ? glfwGetPrimaryMonitor() : nullptr;
		glfwSetWindowMonitor(windowHandle_, monitor, 0, 0, width_, height_, GLFW_DONT_CARE);
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

const IGfxDevice::VideoMode &GlfwGfxDevice::currentVideoMode() const
{
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	convertVideoModeInfo(*mode, currentVideoMode_);

	return currentVideoMode_;
}

bool GlfwGfxDevice::setVideoMode(unsigned int index)
{
	ASSERT(index < numVideoModes_);

	int modeIndex = index;
	if (index >= numVideoModes_)
		modeIndex = 0;

	glfwSetWindowMonitor(windowHandle_, glfwGetPrimaryMonitor(), 0, 0,
	                     videoModes_[modeIndex].width, videoModes_[modeIndex].height, videoModes_[modeIndex].refreshRate);

	return true;
}

void GlfwGfxDevice::updateVideoModes()
{
	int count;
	const GLFWvidmode *modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &count);
	numVideoModes_ = (count < MaxVideoModes) ? count : MaxVideoModes;

	for (unsigned int i = 0; i < numVideoModes_; i++)
	{
		// Reverse GLFW video mode array to be consistent with SDL
		const int srcIndex = count - 1 - i;
		convertVideoModeInfo(modes[srcIndex], videoModes_[i]);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GlfwGfxDevice::initGraphics()
{
#if GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 3
	glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
#endif
	glfwSetErrorCallback(errorCallback);
	FATAL_ASSERT_MSG(glfwInit() == GL_TRUE, "glfwInit() failed");
}

void GlfwGfxDevice::initDevice()
{
	// asking for a video mode that does not change current screen resolution
	if (width_ == 0 || height_ == 0)
	{
		const GLFWvidmode *vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		width_ = vidMode->width;
		height_ = vidMode->height;
	}

	GLFWmonitor *monitor = nullptr;
	if (isFullScreen_)
		monitor = glfwGetPrimaryMonitor();

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
#if defined(WITH_ANGLE)
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

	glfwMakeContextCurrent(windowHandle_);

	const int interval = displayMode_.hasVSync() ? 1 : 0;
	glfwSwapInterval(interval);

#ifdef WITH_GLEW
	const GLenum err = glewInit();
	FATAL_ASSERT_MSG_X(err == GLEW_OK, "GLEW error: %s", glewGetErrorString(err));

	glContextInfo_.debugContext = glContextInfo_.debugContext && glewIsSupported("GL_ARB_debug_output");
#endif

	updateVideoModes();
}

void GlfwGfxDevice::convertVideoModeInfo(const GLFWvidmode &glfwVideoMode, IGfxDevice::VideoMode &videoMode) const
{
	videoMode.width = static_cast<unsigned int>(glfwVideoMode.width);
	videoMode.height = static_cast<unsigned int>(glfwVideoMode.height);
	videoMode.redBits = static_cast<unsigned int>(glfwVideoMode.redBits);
	videoMode.greenBits = static_cast<unsigned int>(glfwVideoMode.greenBits);
	videoMode.blueBits = static_cast<unsigned int>(glfwVideoMode.blueBits);
	videoMode.refreshRate = static_cast<unsigned int>(glfwVideoMode.refreshRate);
}

void GlfwGfxDevice::errorCallback(int error, const char *description)
{
	LOGE_X("GLFW error %d: \"%s\"", error, description);
}

}
