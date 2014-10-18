#ifdef WITH_GLEW
	#include <GL/glew.h>
#endif

#include "ncSDLGfxDevice.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructor taking the resolution as two integer
ncSDLGfxDevice::ncSDLGfxDevice(int width, int height)
{
	init(width, height, ncDisplayMode(), true);
}

/// Constructor taking the resolution as a ncPoint class
ncSDLGfxDevice::ncSDLGfxDevice(ncPoint size)
{
	init(size.x, size.y, ncDisplayMode(), true);
}

/// Constructor taking the resolution as two integer and a DisplayMode
ncSDLGfxDevice::ncSDLGfxDevice(int width, int height, ncDisplayMode mode)
{
	init(width, height, mode, true);
}

/// Constructor taking the resolution as a ncPoint class and a DisplayMode
ncSDLGfxDevice::ncSDLGfxDevice(ncPoint size, ncDisplayMode mode)
{
	init(size.x, size.y, mode, true);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncSDLGfxDevice::setResolution(int width, int height)
{
	// change resolution only in the case it really changes
	if (width != width_ || height != height_)
	{
		width_ = width;
		height_ = height;

		initDevice();
	}
}

void ncSDLGfxDevice::setResolution(ncPoint size)
{
	// change resolution only in the case it really changes
	if (size.x != width_ || size.y != height_)
	{
		width_ = size.x;
		height_ = size.y;

		initDevice();
	}
}

void ncSDLGfxDevice::toggleFullScreen()
{
	isWindowed_ = !isWindowed_;
	initDevice();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes the class
void ncSDLGfxDevice::init(int width, int height, ncDisplayMode mode, bool isWindowed)
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
void ncSDLGfxDevice::initGraphics()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOGF_X("SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
		exit(-1);
	}
}

/// Initilizes the OpenGL graphic context
void ncSDLGfxDevice::initDevice()
{
	// setting OpenGL attributes
	if (mode_.redBits() > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, mode_.redBits());
	}
	if (mode_.greenBits() > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, mode_.greenBits());
	}
	if (mode_.blueBits() > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, mode_.blueBits());
	}
	if (mode_.alphaBits() > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, mode_.alphaBits());
	}
	if (mode_.bufferBits() > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, mode_.bufferBits());
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, mode_.isDoubleBuffered());
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, mode_.hasVSync());
	if (mode_.depthBits() > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, mode_.depthBits());
	}
	if (mode_.stencilBits() > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, mode_.stencilBits());
	}

	Uint32 flags = SDL_OPENGL;
	if (isWindowed_ == false)
	{
		flags |= SDL_FULLSCREEN;
	}

	// setting screen mode, get a screen from SDL
	if (!SDL_SetVideoMode(width_, height_, 0, flags))
	{
		LOGF_X("SDL_SetVideoMode failed: %s", SDL_GetError());
		exit(-1);
	}

#ifdef WITH_GLEW
	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		ncServiceLocator::Logger().write(ncILogger::LOG_FATAL, (const char *)"ncSDLGfxDevice::initDevice - GLEW error: %s", glewGetErrorString(err));
		exit(-1);
	}

	if (!GLEW_VERSION_2_0)
	{
		ncServiceLocator::Logger().write(ncILogger::LOG_FATAL, (const char *)"ncSDLGfxDevice::initDevice - OpenGL 2 is not supported");
		exit(-1);
	}
#endif
}
