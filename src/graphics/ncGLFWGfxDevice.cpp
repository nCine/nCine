#include <cstdlib> // for exit()
#include "ncGLFWGfxDevice.h"
#include "ncServiceLocator.h"

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

		glfwCloseWindow();
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

		glfwCloseWindow();
		InitDevice();
	}
}

void ncGLFWGfxDevice::ToggleFullScreen()
{
	m_bIsWindowed = !m_bIsWindowed;
	glfwCloseWindow();
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
	if (glfwInit() != GL_TRUE) {
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncGLFWGfxDevice::InitGraphics - glfwInit() failed");
		exit(-1);
	}
}

/// Initilizes the OpenGL graphic context
void ncGLFWGfxDevice::InitDevice()
{
	int iWinMode = GLFW_WINDOW;
	if (m_bIsWindowed == false)
		iWinMode = GLFW_FULLSCREEN;

	// setting screen mode, get a screen from SDL
	if (glfwOpenWindow(m_iWidth, m_iHeight, m_mode.RedBits(), m_mode.GreenBits(), m_mode.BlueBits(), m_mode.AlphaBits(),
					   m_mode.DepthBits(), m_mode.StencilBits(), iWinMode) != GL_TRUE)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncGLFWGfxDevice::InitDevice - glfwOpenWindow failed");
		exit(-1);
	}

	if (m_mode.isVSynced())
		glfwSwapInterval(1);

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
