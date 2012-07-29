#include <cstdlib> // for exit()
#include <android_native_app_glue.h> // for android_app
#include "ncEGLGfxDevice.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructor taking a DisplayMode
ncEGLGfxDevice::ncEGLGfxDevice(struct android_app* state, ncDisplayMode mode)
{
	m_mode = mode;
	InitDevice(state);
	InitGL();
}

ncEGLGfxDevice::~ncEGLGfxDevice()
{
	if (m_display != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (m_context != EGL_NO_CONTEXT)
			eglDestroyContext(m_display, m_context);
		if (m_surface != EGL_NO_SURFACE)
			eglDestroySurface(m_display, m_surface);
		eglTerminate(m_display);
	}

	m_display = EGL_NO_DISPLAY;
	m_context = EGL_NO_CONTEXT;
	m_surface = EGL_NO_SURFACE;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Checks if the desired pixel format is supported
bool ncEGLGfxDevice::isModeSupported(struct android_app* state, ncDisplayMode mode)
{
	const EGLint attribs[] = {
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_BLUE_SIZE,			mode.BlueBits(),
		EGL_GREEN_SIZE,			mode.GreenBits(),
		EGL_RED_SIZE,			mode.RedBits(),
		EGL_ALPHA_SIZE,			mode.AlphaBits(),
		EGL_NONE
	};

	EGLint format, numConfigs;
	EGLConfig config;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

#ifdef __ANDROID__
	ANativeWindow_setBuffersGeometry(state->window, 0, 0, format);
#endif

	EGLSurface surface = eglCreateWindowSurface(display, config, state->window, NULL);

	bool bSupportedMode = (surface != EGL_NO_SURFACE);

	if (surface != EGL_NO_SURFACE)
		eglDestroySurface(display, surface);
	eglTerminate(display);

	return bSupportedMode;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes the OpenGL graphic context
void ncEGLGfxDevice::InitDevice(struct android_app* state)
{
	const EGLint attribs[] = {
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_BLUE_SIZE,			m_mode.BlueBits(),
		EGL_GREEN_SIZE,			m_mode.GreenBits(),
		EGL_RED_SIZE,			m_mode.RedBits(),
		EGL_ALPHA_SIZE,			m_mode.AlphaBits(),
		EGL_NONE
	};

	EGLint format, numConfigs;
	EGLConfig config;

	m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(m_display, 0, 0);
	eglChooseConfig(m_display, attribs, &config, 1, &numConfigs);
	eglGetConfigAttrib(m_display, config, EGL_NATIVE_VISUAL_ID, &format);

#ifdef __ANDROID__
	ANativeWindow_setBuffersGeometry(state->window, 0, 0, format);
#endif

	m_surface = eglCreateWindowSurface(m_display, config, state->window, NULL);
	m_context = eglCreateContext(m_display, config, NULL, NULL);

	if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_FALSE)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (char *)"ncEGLGfxDevice::Init - Unable to eglMakeCurrent");
		exit(-1);
	}

	eglQuerySurface(m_display, m_surface, EGL_WIDTH, &m_iWidth);
	eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &m_iHeight);


	EGLint red, blue, green, alpha, depth, stencil, samples;
	eglGetConfigAttrib(m_display, config, EGL_RED_SIZE, &red);
	eglGetConfigAttrib(m_display, config, EGL_GREEN_SIZE, &green);
	eglGetConfigAttrib(m_display, config, EGL_BLUE_SIZE, &blue);
	eglGetConfigAttrib(m_display, config, EGL_ALPHA_SIZE, &alpha);
	eglGetConfigAttrib(m_display, config, EGL_DEPTH_SIZE, &depth);
	eglGetConfigAttrib(m_display, config, EGL_STENCIL_SIZE, &stencil);
	eglGetConfigAttrib(m_display, config, EGL_SAMPLES, &samples);

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (char *)"ncEGLGfxDevice::Init - Surface configuration is RGBA:%d%d%d%d, depth:%d, stencil:%d, samples:%d",
									 red, green, blue, alpha, depth, stencil, samples);
}
