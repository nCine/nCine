#ifndef CLASS_NCINE_EGLGFXDEVICE
#define CLASS_NCINE_EGLGFXDEVICE

#include <EGL/egl.h>

#include "IGfxDevice.h"
#include "Point.h"
#include "DisplayMode.h"

struct android_app;

namespace ncine {

/// The EGL based graphics device
class EglGfxDevice : public IGfxDevice
{
  public:
	// Constructor taking a DisplayMode
	EglGfxDevice(struct android_app* state, DisplayMode mode);
	~EglGfxDevice();

	void setResolution(int width, int height) { }
	void setResolution(Point size) { }

	void toggleFullScreen() { }

	inline void update() { eglSwapBuffers(display_, surface_); }

	void setWindowTitle(const char *windowTitle) { }

	// Checks if the desired pixel format is supported
	static bool isModeSupported(struct android_app* state, DisplayMode mode);

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

}

#endif
