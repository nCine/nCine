#include <cstdlib> // for exit()
#include "GlfwGfxDevice.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLFWwindow *GlfwGfxDevice::windowHandle_ = NULL;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GlfwGfxDevice::GlfwGfxDevice(int width, int height, const GLContextInfo &contextInfo, const DisplayMode &mode, bool isFullScreen)
	: IGfxDevice(width, height, mode, isFullScreen), contextInfo_(contextInfo)
{
	initGraphics();
	initDevice();
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

void GlfwGfxDevice::setResolution(Vector2i size)
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

void GlfwGfxDevice::initGraphics()
{
	glfwSetErrorCallback(errorCallback);

	if (glfwInit() != GL_TRUE)
	{
		LOGF("glfwInit() failed");
		exit(EXIT_FAILURE);
	}
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

	GLFWmonitor *monitor = NULL;
	if (isFullScreen_)
	{
		monitor = glfwGetPrimaryMonitor();
	}

	// setting window hints and creating a window with GLFW
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, contextInfo_.majorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, contextInfo_.minorVersion);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, contextInfo_.debugContext ? GLFW_TRUE : GLFW_FALSE);
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

	contextInfo_.debugContext = contextInfo_.debugContext && glewIsSupported("GL_ARB_debug_output");
#endif

	if (contextInfo_.debugContext)
	{
		enableGlDebugOutput();
	}
}

void GlfwGfxDevice::errorCallback(int error, const char *description)
{
	LOGE_X("GLFW error %d: \"%s\"", error, description);
}

}
