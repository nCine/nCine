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
private:
	/// The EGL display connection
	EGLDisplay m_display;
	/// The EGL drawing surface
	EGLSurface m_surface;
	/// The EGL context
	EGLContext m_context;

	// Initializes the OpenGL graphic context
	void InitDevice(struct android_app* state);

public:
	// Constructor taking a DisplayMode
	ncEGLGfxDevice(struct android_app* state, ncDisplayMode mode);
	~ncEGLGfxDevice();

	void SetResolution(int iWidth, int iHeight) { }
	void SetResolution(ncPoint size) { }

	void ToggleFullScreen() { }

	inline void Update() { eglSwapBuffers(m_display, m_surface); }

	void SetWindowTitle(const char *pWindowTitle) { }
};

#endif
