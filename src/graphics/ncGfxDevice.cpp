#ifndef NO_GLEW
	#include <GL/glew.h>
#endif

#include "ncGfxDevice.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructor taking the resolution as two integer
ncGfxDevice::ncGfxDevice(int iWidth, int iHeight)
	: m_iWidth(iWidth), m_iHeight(iHeight), m_bIsWindowed(true), m_mode()
{
	InitGraphics();
	InitDevice();
	InitGL();
}

/// Constructor taking the resolution as a Size class
ncGfxDevice::ncGfxDevice(ncPoint size)
	: m_iWidth(size.x), m_iHeight(size.y), m_bIsWindowed(true), m_mode()
{
	InitGraphics();
	InitDevice();
	InitGL();
}

/// Constructor taking the resolution as two integer and a DisplayMode
ncGfxDevice::ncGfxDevice(int iWidth, int iHeight, ncDisplayMode mode)
	: m_iWidth(iWidth), m_iHeight(iHeight), m_bIsWindowed(true), m_mode(mode)
{
	InitGraphics();
	InitDevice();
	InitGL();
}

/// Constructor taking the resolution as a Size class and a DisplayMode
ncGfxDevice::ncGfxDevice(ncPoint size, ncDisplayMode mode)
	: m_iWidth(size.x), m_iHeight(size.y), m_bIsWindowed(true), m_mode(mode)
{
	InitGraphics();
	InitDevice();
	InitGL();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Set screen resolution with two integers
void ncGfxDevice::SetResolution(int iWidth, int iHeight)
{
	// change resolution only in the case it really changes
	if(iWidth != m_iWidth || iHeight != m_iHeight)
	{
		m_iWidth = iWidth;
		m_iHeight = iHeight;

		InitDevice();
	}
}

/// Set screen resolution with the Size class
void ncGfxDevice::SetResolution(ncPoint size)
{
	// change resolution only in the case it really changes
	if(size.x != m_iWidth || size.y != m_iHeight)
	{
		m_iWidth = size.x;
		m_iHeight = size.y;

		InitDevice();
	}
}

/// Toggle between fullscreen and windowed mode
void ncGfxDevice::ToggleFullScreen()
{
	m_bIsWindowed = !m_bIsWindowed;
	InitDevice();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Init the video subsystem (SDL)
void ncGfxDevice::InitGraphics()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncGfxDevice::InitGraphics - SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
		exit(-1);
	}

	atexit(SDL_Quit);
}

/// Init the OpenGL graphic context
void ncGfxDevice::InitDevice()
{
	// setting OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, m_mode.isDoubleBuffered());
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, m_mode.isVSynced());
	if (m_mode.DepthBits() > 0)
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, m_mode.DepthBits());
	if (m_mode.StencilBits() > 0)
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, m_mode.StencilBits());

	Uint32 lFlags = SDL_OPENGL;
	if (m_bIsWindowed == false)
		lFlags |= SDL_FULLSCREEN;

	// setting screen mode, get a screen from SDL
	if (!SDL_SetVideoMode(m_iWidth, m_iHeight, 0, lFlags)) {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncGfxDevice::InitDevice - SDL_SetVideoMode failed: %s", SDL_GetError());
		exit(-1);
	}

#ifndef NO_GLEW
	GLenum err = glewInit();

	if (GLEW_OK != err) {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncGfxDevice::InitDevice - GLEW error: %s", glewGetErrorString(err));
		exit(-1);
	}

	if (!GLEW_VERSION_2_0) {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncGfxDevice::InitDevice - OpenGL 2 is not supported");
		exit(-1);
	}
#endif
}


/// Init starting OpenGL state
void ncGfxDevice::InitGL()
{
//	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glOrtho(0, m_iWidth, 0, m_iHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
