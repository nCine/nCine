#ifndef CLASS_NCINE_SDLGFXDEVICE
#define CLASS_NCINE_SDLGFXDEVICE

#include <SDL/SDL.h>

#include "IGfxDevice.h"
#include "Point.h"
#include "DisplayMode.h"

namespace ncine {

/// The SDL based graphics device
class SdlGfxDevice : public IGfxDevice
{
  public:
	// Constructor taking the resolution as two integer
	SdlGfxDevice(int width, int height, bool isFullScreen);
	// Constructor taking the resolution as a Point class
	SdlGfxDevice(Point size, bool isFullScreen);
	// Constructor taking the resolution as two integer and a DisplayMode
	SdlGfxDevice(int width, int height, DisplayMode mode, bool isFullScreen);
	// Constructor taking the resolution as a Point class and a DisplayMode
	SdlGfxDevice(Point size, DisplayMode mode, bool isFullScreen);
	~SdlGfxDevice() { SDL_Quit(); }

	void setResolution(int width, int height);
	void setResolution(Point size);

	void toggleFullScreen();

	inline void update() { SDL_GL_SwapBuffers(); }

	inline void setWindowTitle(const char *windowTitle) { SDL_WM_SetCaption(windowTitle, NULL); }

  private:
	/// Private copy constructor
	SdlGfxDevice(const SdlGfxDevice&);
	/// Private assignment operator
	SdlGfxDevice& operator=(const SdlGfxDevice&);

	// Initilizes the class
	void init(int width, int height, DisplayMode mode, bool isFullScreen_);
	// Initilizes the video subsystem (SDL)
	void initGraphics();
	// Initilizes the OpenGL graphic context
	void initDevice();
};

}

#endif
