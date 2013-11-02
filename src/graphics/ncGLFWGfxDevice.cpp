#include <cstdlib> // for exit()
#include "ncGLFWGfxDevice.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLFWwindow* ncGLFWGfxDevice::s_pWindowHandle = NULL;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructor taking the resolution as two integer
ncGLFWGfxDevice::ncGLFWGfxDevice(int iWidth, int iHeight)
{
	Init(iWidth, iHeight, ncDisplayMode(), true);
}

/// Constructor taking the resolution as a ncPoint class
ncGLFWGfxDevice::ncGLFWGfxDevice(ncPoint size)
{
	Init(size.x, size.y, ncDisplayMode(), true);
}

/// Constructor taking the resolution as two integer and a DisplayMode
ncGLFWGfxDevice::ncGLFWGfxDevice(int iWidth, int iHeight, ncDisplayMode mode)
{
	Init(iWidth, iHeight, mode, true);
}

/// Constructor taking the resolution as a ncPoint class and a DisplayMode
ncGLFWGfxDevice::ncGLFWGfxDevice(ncPoint size, ncDisplayMode mode)
{
	Init(size.x, size.y, mode, true);
}

ncGLFWGfxDevice::~ncGLFWGfxDevice()
{
	glfwDestroyWindow(s_pWindowHandle);
	s_pWindowHandle = NULL;
	glfwTerminate();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncGLFWGfxDevice::SetResolution(int iWidth, int iHeight)
{
	// change resolution only in the case it really changes
	if(iWidth != m_iWidth || iHeight != m_iHeight)
	{
		m_iWidth = iWidth;
		m_iHeight = iHeight;

		glfwDestroyWindow(s_pWindowHandle);
		s_pWindowHandle = NULL;
		InitDevice();
	}
}

void ncGLFWGfxDevice::SetResolution(ncPoint size)
{
	// change resolution only in the case it really changes
	if(size.x != m_iWidth || size.y != m_iHeight)
	{
		m_iWidth = size.x;
		m_iHeight = size.y;

		glfwDestroyWindow(s_pWindowHandle);
		s_pWindowHandle = NULL;
		InitDevice();
	}
}

void ncGLFWGfxDevice::ToggleFullScreen()
{
	m_bIsWindowed = !m_bIsWindowed;

	glfwDestroyWindow(s_pWindowHandle);
	s_pWindowHandle = NULL;
	InitDevice();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes the class
void ncGLFWGfxDevice::Init(int iWidth, int iHeight, ncDisplayMode mode, bool bIsWindowed)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_mode = mode;
	m_bIsWindowed = bIsWindowed;

	InitGraphics();
	InitDevice();
	InitGL();
}

/// Initilizes the video subsystem (SDL)
void ncGLFWGfxDevice::InitGraphics()
{
	glfwSetErrorCallback(ErrorCallback);

	if (glfwInit() != GL_TRUE) {
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncGLFWGfxDevice::InitGraphics - glfwInit() failed");
		exit(-1);
	}
}

/// Initilizes the OpenGL graphic context
void ncGLFWGfxDevice::InitDevice()
{
	GLFWmonitor* monitor = NULL;
	if (m_bIsWindowed == false)
		monitor = glfwGetPrimaryMonitor();

	// setting window hints and creating a window with GLFW
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_RED_BITS, m_mode.RedBits());
	glfwWindowHint(GLFW_GREEN_BITS, m_mode.GreenBits());
	glfwWindowHint(GLFW_BLUE_BITS, m_mode.BlueBits());
	glfwWindowHint(GLFW_ALPHA_BITS, m_mode.AlphaBits());
	glfwWindowHint(GLFW_DEPTH_BITS, m_mode.DepthBits());
	glfwWindowHint(GLFW_STENCIL_BITS, m_mode.StencilBits());

	s_pWindowHandle = glfwCreateWindow(m_iWidth, m_iHeight, "", monitor, NULL);
	if (s_pWindowHandle == NULL)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncGLFWGfxDevice::InitDevice - glfwCreateWindow failed");
		exit(-1);
	}

	glfwMakeContextCurrent(s_pWindowHandle);

	if (m_mode.isVSynced())
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

#ifdef WITH_GLEW
	GLenum err = glewInit();

	if (GLEW_OK != err) {
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncGLFWGfxDevice::InitDevice - GLEW error: %s", glewGetErrorString(err));
		exit(-1);
	}

	if (!GLEW_VERSION_2_0) {
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncGLFWGfxDevice::InitDevice - OpenGL 2 is not supported");
		exit(-1);
	}
#endif
}

/// Callback for glfwSetErrorCallback()
void ncGLFWGfxDevice::ErrorCallback(int error, const char* description)
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncGLFWGfxDevice::ErrorCallback - (%d) %s", error, description);
}
