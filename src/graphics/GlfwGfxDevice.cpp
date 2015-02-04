#include <cstdlib> // for exit()
#include "GlfwGfxDevice.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLFWwindow* GlfwGfxDevice::windowHandle_ = NULL;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructor taking the resolution as two integer
GlfwGfxDevice::GlfwGfxDevice(int width, int height, bool isFullScreen)
{
	init(width, height, DisplayMode(), isFullScreen);
}

/// Constructor taking the resolution as a Point class
GlfwGfxDevice::GlfwGfxDevice(Point size, bool isFullScreen)
{
	init(size.x, size.y, DisplayMode(), isFullScreen);
}

/// Constructor taking the resolution as two integer and a DisplayMode
GlfwGfxDevice::GlfwGfxDevice(int width, int height, DisplayMode mode, bool isFullScreen)
{
	init(width, height, mode, isFullScreen);
}

/// Constructor taking the resolution as a Point class and a DisplayMode
GlfwGfxDevice::GlfwGfxDevice(Point size, DisplayMode mode, bool isFullScreen)
{
	init(size.x, size.y, mode, isFullScreen);
}

GlfwGfxDevice::~GlfwGfxDevice()
{
	glfwDestroyWindow(windowHandle_);
	windowHandle_ = NULL;
	glfwTerminate();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GlfwGfxDevice::setResolution(int width, int height)
{
	// change resolution only in the case it really changes
	if (width != width_ || height != height_)
	{
		width_ = width;
		height_ = height;

		glfwDestroyWindow(windowHandle_);
		windowHandle_ = NULL;
		initDevice();
	}
}

void GlfwGfxDevice::setResolution(Point size)
{
	setResolution(size.x, size.y);
}

void GlfwGfxDevice::toggleFullScreen()
{
	isFullScreen_ = !isFullScreen_;

	glfwDestroyWindow(windowHandle_);
	windowHandle_ = NULL;
	initDevice();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes the class
void GlfwGfxDevice::init(int width, int height, DisplayMode mode, bool isFullScreen)
{
	width_ = width;
	height_ = height;
	mode_ = mode;
	isFullScreen_ = isFullScreen;

	initGraphics();
	initDevice();
}

/// Initilizes the video subsystem (SDL)
void GlfwGfxDevice::initGraphics()
{
	glfwSetErrorCallback(errorCallback);

	if (glfwInit() != GL_TRUE)
	{
		LOGF("glfwInit() failed");
		exit(EXIT_FAILURE);
	}
}

/// Initilizes the OpenGL graphic context
void GlfwGfxDevice::initDevice()
{
	GLFWmonitor* monitor = NULL;
	if (isFullScreen_)
	{
		monitor = glfwGetPrimaryMonitor();
	}

	// setting window hints and creating a window with GLFW
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_RED_BITS, mode_.redBits());
	glfwWindowHint(GLFW_GREEN_BITS, mode_.greenBits());
	glfwWindowHint(GLFW_BLUE_BITS, mode_.blueBits());
	glfwWindowHint(GLFW_ALPHA_BITS, mode_.alphaBits());
	glfwWindowHint(GLFW_DEPTH_BITS, mode_.depthBits());
	glfwWindowHint(GLFW_STENCIL_BITS, mode_.stencilBits());

	windowHandle_ = glfwCreateWindow(width_, height_, "", monitor, NULL);
	if (windowHandle_ == NULL)
	{
		LOGF("glfwCreateWindow failed");
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(windowHandle_);

	if (mode_.hasVSync())
	{
		glfwSwapInterval(1);
	}
	else
	{
		glfwSwapInterval(0);
	}

#ifdef WITH_GLEW
	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		LOGF_X("GLEW error: %s", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	if (!GLEW_VERSION_2_0)
	{
		LOGF("OpenGL 2 is not supported");
		exit(EXIT_FAILURE);
	}
#endif
}

/// Callback for glfwSetErrorCallback()
void GlfwGfxDevice::errorCallback(int error, const char* description)
{
	LOGE_X("(%d) %s", error, description);
}

}
