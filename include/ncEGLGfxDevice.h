#ifndef CLASS_NCEGLGFXDEVICE
#define CLASS_NCEGLGFXDEVICE

#include <EGL/egl.h>

#include "ncIGfxDevice.h"
#include "ncPoint.h"
#include "ncDisplayMode.h"

struct android_app;

/// The EGL based graphics device
class ncEGLGfxDevice : public ncIGfxDevice
{
  public:
	// Constructor taking a DisplayMode
	ncEGLGfxDevice(struct android_app* state, ncDisplayMode mode);
	~ncEGLGfxDevice();

	void setResolution(int width, int height) { }
	void setResolution(ncPoint size) { }

	void toggleFullScreen() { }

	inline void update() { eglSwapBuffers(display_, surface_); }

	void setWindowTitle(const char *windowTitle) { }

	// Checks if the desired pixel format is supported
	static bool isModeSupported(struct android_app* state, ncDisplayMode mode);

  private:
	/// The EGL display connection
	EGLDisplay display_;
	/// The EGL drawing surface
	EGLSurface surface_;
	/// The EGL context
	EGLContext context_;

	// Initializes the OpenGL graphic context
	void initDevice(struct android_app* state);
};

#endif
