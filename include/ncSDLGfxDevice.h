#ifndef CLASS_NCSDLGFXDEVICE
#define CLASS_NCSDLGFXDEVICE

#include <SDL/SDL.h>

#include "ncIGfxDevice.h"
#include "ncPoint.h"
#include "ncDisplayMode.h"

/// The SDL based graphics device
class ncSDLGfxDevice : public ncIGfxDevice
{
  public:
	// Constructor taking the resolution as two integer
	ncSDLGfxDevice(int width, int height);
	// Constructor taking the resolution as a size class
	ncSDLGfxDevice(ncPoint size);
	// Constructor taking the resolution as two integer and a DisplayMode
	ncSDLGfxDevice(int width, int height, ncDisplayMode mode);
	// Constructor taking the resolution as a Size class and a DisplayMode
	ncSDLGfxDevice(ncPoint size, ncDisplayMode mode);
	~ncSDLGfxDevice() { SDL_Quit(); }

	void setResolution(int width, int height);
	void setResolution(ncPoint size);

	void toggleFullScreen();

	inline void update() { SDL_GL_SwapBuffers(); }

	inline void setWindowTitle(const char *windowTitle) { SDL_WM_SetCaption(windowTitle, NULL); }

  private:
	// Initilizes the class
	void init(int width, int height, ncDisplayMode mode, bool isWindowed);
	// Initilizes the video subsystem (SDL)
	void initGraphics();
	// Initilizes the OpenGL graphic context
	void initDevice();
};

#endif
