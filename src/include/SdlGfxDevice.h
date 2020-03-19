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
	SdlGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode);
	~SdlGfxDevice() override;

	void setSwapInterval(int interval) override;

	void setResolution(int width, int height) override;
	inline void setResolution(Vector2i size) override { setResolution(size.x, size.y); }

	void setFullScreen(bool fullScreen) override;

	inline void update() override { SDL_GL_SwapWindow(windowHandle_); }

	inline void setWindowTitle(const char *windowTitle) override { SDL_SetWindowTitle(windowHandle_, windowTitle); }
	void setWindowIcon(const char *windowIconFilename) override;

	const VideoMode &currentVideoMode() const override;
	bool setVideoMode(unsigned int index) override;
	void updateVideoModes() override;

	static inline SDL_Window *windowHandle() { return windowHandle_; }

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

	void convertVideoModeInfo(const SDL_DisplayMode &sdlVideoMode, IGfxDevice::VideoMode &videoMode) const;

	friend class SdlInputManager;
};

}

#endif
