#ifndef NO_GLEW
	#include <GL/glew.h>
#endif

#include "GfxDevice.h"
#include "../ServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructor taking the resolution as two integer
GfxDevice::GfxDevice(int iWidth, int iHeight)
	: m_iWidth(iWidth), m_iHeight(iHeight), m_bIsWindowed(true), m_mode()
{
	InitGraphics();
	InitDevice();
	InitGL();
}

/// Constructor taking the resolution as a Size class
GfxDevice::GfxDevice(Size size)
	: m_iWidth(size.Width()), m_iHeight(size.Height()), m_bIsWindowed(true), m_mode()
{
	InitGraphics();
	InitDevice();
	InitGL();
}

/// Constructor taking the resolution as two integer and a DisplayMode
GfxDevice::GfxDevice(int iWidth, int iHeight, DisplayMode mode)
	: m_iWidth(iWidth), m_iHeight(iHeight), m_bIsWindowed(true), m_mode(mode)
{
	InitGraphics();
	InitDevice();
	InitGL();
}

/// Constructor taking the resolution as a Size class and a DisplayMode
GfxDevice::GfxDevice(Size size, DisplayMode mode)
	: m_iWidth(size.Width()), m_iHeight(size.Height()), m_bIsWindowed(true), m_mode(mode)
{
	InitGraphics();
	InitDevice();
	InitGL();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Set screen resolution with two integers
void GfxDevice::SetResolution(int iWidth, int iHeight)
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
void GfxDevice::SetResolution(Size size)
{
	// change resolution only in the case it really changes
	if(size.Width() != m_iWidth || size.Height() != m_iHeight)
	{
		m_iWidth = size.Width();
		m_iHeight = size.Height();

		InitDevice();
	}
}

/// Toggle between fullscreen and windowed mode
void GfxDevice::ToggleFullScreen()
{
	m_bIsWindowed = !m_bIsWindowed;
	InitDevice();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Init the video subsystem (SDL)
void GfxDevice::InitGraphics()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		ServiceLocator::GetLogger().Write(1, (char *)"GfxDevice::InitGraphics - SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
		exit(-1);
	}

	atexit(SDL_Quit);
}

/// Init the OpenGL graphic context
void GfxDevice::InitDevice()
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
		ServiceLocator::GetLogger().Write(1, (char *)"GfxDevice::InitDevice - SDL_SetVideoMode failed: %s", SDL_GetError());
		exit(-1);
	}

#ifndef NO_GLEW
	GLenum err = glewInit();

	if (GLEW_OK != err) {
		ServiceLocator::GetLogger().Write(1, (char *)"GfxDevice::InitDevice - GLEW error: %s", glewGetErrorString(err));
		exit(-1);
	}

	if (!GLEW_VERSION_2_0) {
		ServiceLocator::GetLogger().Write(1, (char *)"GfxDevice::InitDevice - OpenGL 2 is not supported");
		exit(-1);
	}
#endif
}


/// Init starting OpenGL state
void GfxDevice::InitGL()
{
//	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}
