#ifndef CLASS_NCINE_SDLGFXDEVICE
#define CLASS_NCINE_SDLGFXDEVICE

#ifdef WITH_SDL2
	#include <SDL2/SDL_video.h>
#else
	#include <SDL3/SDL_video.h>
#endif

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

	void setFullscreen(bool fullscreen) override;
	void setResizable(bool resizable) override;

	int windowPositionX() const override;
	int windowPositionY() const override;
	const Vector2i windowPosition() const override;
	void setWindowPosition(int x, int y) override;

	void setWindowSize(int width, int height) override;

	void setWindowTitle(const char *windowTitle) override;
	void setWindowIcon(const char *windowIconFilename) override;
	void flashWindow() const override;

	unsigned int windowMonitorIndex() const override;

	const VideoMode &currentVideoMode(unsigned int monitorIndex) const override;
	bool setVideoMode(unsigned int modeIndex) override;

	void swapBuffers() override;

  private:
	/// SDL2 window handle
	static SDL_Window *windowHandle_;
	/// SDL2 OpenGL context handle
	SDL_GLContext glContextHandle_;

#ifdef WITH_SDL3
	int displayIDCount_;
	SDL_DisplayID *displayIDs_;
#endif

	/// Deleted copy constructor
	SdlGfxDevice(const SdlGfxDevice &) = delete;
	/// Deleted assignment operator
	SdlGfxDevice &operator=(const SdlGfxDevice &) = delete;

	/// Initilizes the video subsystem (SDL)
	void initGraphics();
	/// Initilizes the OpenGL graphic context
	void initDevice(const WindowMode &windowMode);

	void updateMonitors() override;

	void convertVideoModeInfo(const SDL_DisplayMode &sdlVideoMode, IGfxDevice::VideoMode &videoMode) const;

	static inline SDL_Window *windowHandle() { return windowHandle_; }

	friend class SdlInputManager;
};

}

#endif
