#ifndef CLASS_NCINE_SDLGFXDEVICE
#define CLASS_NCINE_SDLGFXDEVICE

#include <SDL2/SDL.h>

#include "IGfxDevice.h"
#include "Vector2.h"
#include "DisplayMode.h"

namespace ncine {

/// The SDL based graphics device
class SdlGfxDevice : public IGfxDevice
{
  public:
	SdlGfxDevice(int width, int height, const GLContextInfo &contextInfo, const DisplayMode &mode, bool isFullScreen);
	~SdlGfxDevice() override;

	void setResolution(int width, int height) override;
	inline void setResolution(Vector2i size) override { setResolution(size.x, size.y); }

	void toggleFullScreen() override;

	inline void update() override { SDL_GL_SwapWindow(windowHandle_); }

	inline void setWindowTitle(const char *windowTitle) override { SDL_SetWindowTitle(windowHandle_, windowTitle); }
	void setWindowIcon(const char *windowIconFilename) override;

  private:
	/// SDL2 window handle
	static SDL_Window *windowHandle_;
	/// SDL2 OpenGL context handle
	SDL_GLContext glContextHandle_;

	/// Deleted copy constructor
	SdlGfxDevice(const SdlGfxDevice &) = delete;
	/// Deleted assignment operator
	SdlGfxDevice &operator=(const SdlGfxDevice &) = delete;

	/// Initilizes the video subsystem (SDL)
	void initGraphics();
	/// Initilizes the OpenGL graphic context
	void initDevice();
};

}

#endif
