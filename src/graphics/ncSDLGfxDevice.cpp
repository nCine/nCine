#include "ncSDLGfxDevice.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructor taking the resolution as two integer
ncSDLGfxDevice::ncSDLGfxDevice(int iWidth, int iHeight)
{
	Init(iWidth, iHeight, ncDisplayMode(), true);
}

/// Constructor taking the resolution as a ncPoint class
ncSDLGfxDevice::ncSDLGfxDevice(ncPoint size)
{
	Init(size.x, size.y, ncDisplayMode(), true);
}

/// Constructor taking the resolution as two integer and a DisplayMode
ncSDLGfxDevice::ncSDLGfxDevice(int iWidth, int iHeight, ncDisplayMode mode)
{
	Init(iWidth, iHeight, mode, true);
}

/// Constructor taking the resolution as a ncPoint class and a DisplayMode
ncSDLGfxDevice::ncSDLGfxDevice(ncPoint size, ncDisplayMode mode)
{
	Init(size.x, size.y, mode, true);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Sets screen resolution with two integers
void ncSDLGfxDevice::SetResolution(int iWidth, int iHeight)
{
	// change resolution only in the case it really changes
	if(iWidth != m_iWidth || iHeight != m_iHeight)
	{
		m_iWidth = iWidth;
		m_iHeight = iHeight;

		InitDevice();
	}
}

/// Sets screen resolution with the ncPoint class
void ncSDLGfxDevice::SetResolution(ncPoint size)
{
	// change resolution only in the case it really changes
	if(size.x != m_iWidth || size.y != m_iHeight)
	{
		m_iWidth = size.x;
		m_iHeight = size.y;

		InitDevice();
	}
}

/// Toggles between fullscreen and windowed mode
void ncSDLGfxDevice::ToggleFullScreen()
{
	m_bIsWindowed = !m_bIsWindowed;
	InitDevice();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes the class
void ncSDLGfxDevice::Init(int iWidth, int iHeight, ncDisplayMode mode, bool bIsWindowed)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_mode = mode;
	m_bIsWindowed = bIsWindowed;

	InitGraphics();
	InitDevice();
	InitGL();

	SDL_WM_SetCaption("nCine", NULL);
}

/// Initilizes the video subsystem (SDL)
void ncSDLGfxDevice::InitGraphics()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (const char *)"ncSDLGfxDevice::InitGraphics - SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
		exit(-1);
	}
}

/// Initilizes the OpenGL graphic context
void ncSDLGfxDevice::InitDevice()
{
	// setting OpenGL attributes
	if (m_mode.RedBits() > 0)
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, m_mode.RedBits());
	if (m_mode.GreenBits() > 0)
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, m_mode.GreenBits());
	if (m_mode.BlueBits() > 0)
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, m_mode.BlueBits());
	if (m_mode.AlphaBits() > 0)
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, m_mode.AlphaBits());
	if (m_mode.BufferBits() > 0)
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, m_mode.BufferBits());

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
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (const char *)"ncSDLGfxDevice::InitDevice - SDL_SetVideoMode failed: %s", SDL_GetError());
		exit(-1);
	}

#ifndef NO_GLEW
	GLenum err = glewInit();

	if (GLEW_OK != err) {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (const char *)"ncSDLGfxDevice::InitDevice - GLEW error: %s", glewGetErrorString(err));
		exit(-1);
	}

	if (!GLEW_VERSION_2_0) {
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (const char *)"ncSDLGfxDevice::InitDevice - OpenGL 2 is not supported");
		exit(-1);
	}
#endif
}


/// Initilizes starting OpenGL state
void ncSDLGfxDevice::InitGL()
{
	glDisable(GL_DITHER);
//	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glOrtho(0, m_iWidth, 0, m_iHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
