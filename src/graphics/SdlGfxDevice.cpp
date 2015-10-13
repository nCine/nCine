#ifdef WITH_GLEW
	#include <GL/glew.h>
#endif

#include "SdlGfxDevice.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructor taking the resolution as two integer
SdlGfxDevice::SdlGfxDevice(int width, int height, bool isFullScreen)
{
	init(width, height, DisplayMode(), isFullScreen);
}

/// Constructor taking the resolution as a Vector2i class
SdlGfxDevice::SdlGfxDevice(Vector2i size, bool isFullScreen_)
{
	init(size.x, size.y, DisplayMode(), isFullScreen_);
}

/// Constructor taking the resolution as two integer and a DisplayMode
SdlGfxDevice::SdlGfxDevice(int width, int height, DisplayMode mode, bool isFullScreen)
{
	init(width, height, mode, isFullScreen);
}

/// Constructor taking the resolution as a Vector2i class and a DisplayMode
SdlGfxDevice::SdlGfxDevice(Vector2i size, DisplayMode mode, bool isFullScreen)
{
	init(size.x, size.y, mode, isFullScreen);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void SdlGfxDevice::setResolution(int width, int height)
{
	// change resolution only in the case it really changes
	if (width != width_ || height != height_)
	{
		width_ = width;
		height_ = height;

		initDevice();
	}
}

void SdlGfxDevice::setResolution(Vector2i size)
{
	// change resolution only in the case it really changes
	if (size.x != width_ || size.y != height_)
	{
		width_ = size.x;
		height_ = size.y;

		initDevice();
	}
}

void SdlGfxDevice::toggleFullScreen()
{
	isFullScreen_ = !isFullScreen_;
	initDevice();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes the class
void SdlGfxDevice::init(int width, int height, DisplayMode mode, bool isFullScreen)
{
	width_ = width;
	height_ = height;
	mode_ = mode;
	isFullScreen_ = isFullScreen;

	initGraphics();
	initDevice();
}

/// Initilizes the video subsystem (SDL)
void SdlGfxDevice::initGraphics()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOGF_X("SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
		exit(-1);
	}
}

/// Initilizes the OpenGL graphic context
void SdlGfxDevice::initDevice()
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
	if (isFullScreen_)
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
		LOGF_X("GLEW error: %s", glewGetErrorString(err));
		exit(-1);
	}

	if (!GLEW_VERSION_2_1)
	{
		LOGF("OpenGL 2.1 is not supported");
		exit(-1);
	}
#endif
}

}
