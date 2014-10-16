#include <cstdlib> // for exit()
#include "ncGLFWGfxDevice.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLFWwindow* ncGLFWGfxDevice::windowHandle_ = NULL;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructor taking the resolution as two integer
ncGLFWGfxDevice::ncGLFWGfxDevice(int width, int height)
{
	init(width, height, ncDisplayMode(), true);
}

/// Constructor taking the resolution as a ncPoint class
ncGLFWGfxDevice::ncGLFWGfxDevice(ncPoint size)
{
	init(size.x, size.y, ncDisplayMode(), true);
}

/// Constructor taking the resolution as two integer and a DisplayMode
ncGLFWGfxDevice::ncGLFWGfxDevice(int width, int height, ncDisplayMode mode)
{
	init(width, height, mode, true);
}

/// Constructor taking the resolution as a ncPoint class and a DisplayMode
ncGLFWGfxDevice::ncGLFWGfxDevice(ncPoint size, ncDisplayMode mode)
{
	init(size.x, size.y, mode, true);
}

ncGLFWGfxDevice::~ncGLFWGfxDevice()
{
	glfwDestroyWindow(windowHandle_);
	windowHandle_ = NULL;
	glfwTerminate();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncGLFWGfxDevice::setResolution(int width, int height)
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

void ncGLFWGfxDevice::setResolution(ncPoint size)
{
	// change resolution only in the case it really changes
	if (size.x != width_ || size.y != height_)
	{
		width_ = size.x;
		height_ = size.y;

		glfwDestroyWindow(windowHandle_);
		windowHandle_ = NULL;
		initDevice();
	}
}

void ncGLFWGfxDevice::toggleFullScreen()
{
	isWindowed_ = !isWindowed_;

	glfwDestroyWindow(windowHandle_);
	windowHandle_ = NULL;
	initDevice();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes the class
void ncGLFWGfxDevice::init(int width, int height, ncDisplayMode mode, bool isWindowed)
{
	width_ = width;
	height_ = height;
	mode_ = mode;
	isWindowed_ = isWindowed;

	initGraphics();
	initDevice();
	initGL();
}

/// Initilizes the video subsystem (SDL)
void ncGLFWGfxDevice::initGraphics()
{
	glfwSetErrorCallback(errorCallback);

	if (glfwInit() != GL_TRUE)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncGLFWGfxDevice::initGraphics - glfwInit() failed");
		exit(EXIT_FAILURE);
	}
}

/// Initilizes the OpenGL graphic context
void ncGLFWGfxDevice::initDevice()
{
	GLFWmonitor* monitor = NULL;
	if (isWindowed_ == false)
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
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncGLFWGfxDevice::initDevice - glfwCreateWindow failed");
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
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncGLFWGfxDevice::initDevice - GLEW error: %s", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	if (!GLEW_VERSION_2_0)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncGLFWGfxDevice::initDevice - OpenGL 2 is not supported");
		exit(EXIT_FAILURE);
	}
#endif
}

/// Callback for glfwSetErrorCallback()
void ncGLFWGfxDevice::errorCallback(int error, const char* description)
{
	ncServiceLocator::logger().write(ncILogger::LOG_ERROR, (const char *)"ncGLFWGfxDevice::errorCallback - (%d) %s", error, description);
}
