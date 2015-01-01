#include <cstdlib> // for exit()
#include <android_native_app_glue.h> // for android_app
#include "EglGfxDevice.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructor taking a DisplayMode
EglGfxDevice::EglGfxDevice(struct android_app* state, DisplayMode mode)
{
	mode_ = mode;
	initDevice(state);
	initGL();
}

EglGfxDevice::~EglGfxDevice()
{
	if (display_ != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (context_ != EGL_NO_CONTEXT)
		{
			eglDestroyContext(display_, context_);
		}
		if (surface_ != EGL_NO_SURFACE)
		{
			eglDestroySurface(display_, surface_);
		}
		eglTerminate(display_);
	}

	display_ = EGL_NO_DISPLAY;
	context_ = EGL_NO_CONTEXT;
	surface_ = EGL_NO_SURFACE;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Recreates a surface from a native window
void EglGfxDevice::createSurface(struct android_app* state)
{
	if (state->window != NULL)
	{
		surface_ = eglCreateWindowSurface(display_, config_, state->window, NULL);
		if (surface_ == EGL_NO_SURFACE)
		{
			LOGF("eglCreateWindowSurface() returned EGL_NO_SURFACE");
			exit(-1);
		}
	}
}

/// Binds the current context
void EglGfxDevice::bindContext()
{
	if (eglMakeCurrent(display_, surface_, surface_, context_) == EGL_FALSE)
	{
		LOGF("eglMakeCurrent() returned EGL_FALSE");
		exit(-1);
	}
}

/// Unbinds the current context
void EglGfxDevice::unbindContext()
{
	if (eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) == EGL_FALSE)
	{
		LOGF("eglMakeCurrent() returned EGL_FALSE");
		exit(-1);
	}
}

/// Queries the size of the current surface
void EglGfxDevice::querySurfaceSize()
{
	eglQuerySurface(display_, surface_, EGL_WIDTH, &width_);
	eglQuerySurface(display_, surface_, EGL_HEIGHT, &height_);
}

/// Checks if the desired pixel format is supported
bool EglGfxDevice::isModeSupported(struct android_app *state, DisplayMode mode)
{
	const EGLint attribs[] =
	{
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_BLUE_SIZE,			static_cast<int>(mode.blueBits()),
		EGL_GREEN_SIZE,			static_cast<int>(mode.greenBits()),
		EGL_RED_SIZE,			static_cast<int>(mode.redBits()),
		EGL_ALPHA_SIZE,			static_cast<int>(mode.alphaBits()),
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

	bool modeIsSupported = (surface != EGL_NO_SURFACE);

	if (surface != EGL_NO_SURFACE)
	{
		eglDestroySurface(display, surface);
	}
	eglTerminate(display);

	return modeIsSupported;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes the OpenGL graphic context
void EglGfxDevice::initDevice(struct android_app* state)
{
	const EGLint attribs[] =
	{
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_BLUE_SIZE,			static_cast<int>(mode_.blueBits()),
		EGL_GREEN_SIZE,			static_cast<int>(mode_.greenBits()),
		EGL_RED_SIZE,			static_cast<int>(mode_.redBits()),
		EGL_ALPHA_SIZE,			static_cast<int>(mode_.alphaBits()),
		EGL_NONE
	};

	EGLint format, numConfigs;

	display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display_, 0, 0);
	eglChooseConfig(display_, attribs, &config_, 1, &numConfigs);
	eglGetConfigAttrib(display_, config_, EGL_NATIVE_VISUAL_ID, &format);

#ifdef __ANDROID__
	ANativeWindow_setBuffersGeometry(state->window, 0, 0, format);
#endif

	createSurface(state);
	context_ = eglCreateContext(display_, config_, NULL, NULL);
	if (context_ == EGL_NO_CONTEXT)
	{
		LOGF("eglCreateContext() returned EGL_NO_CONTEXT");
		exit(-1);
	}

	bindContext();
	querySurfaceSize();

	EGLint red, blue, green, alpha, depth, stencil, samples;
	eglGetConfigAttrib(display_, config_, EGL_RED_SIZE, &red);
	eglGetConfigAttrib(display_, config_, EGL_GREEN_SIZE, &green);
	eglGetConfigAttrib(display_, config_, EGL_BLUE_SIZE, &blue);
	eglGetConfigAttrib(display_, config_, EGL_ALPHA_SIZE, &alpha);
	eglGetConfigAttrib(display_, config_, EGL_DEPTH_SIZE, &depth);
	eglGetConfigAttrib(display_, config_, EGL_STENCIL_SIZE, &stencil);
	eglGetConfigAttrib(display_, config_, EGL_SAMPLES, &samples);

	LOGI_X("Surface configuration is RGBA:%d%d%d%d, depth:%d, stencil:%d, samples:%d", red, green, blue, alpha, depth, stencil, samples);
}

}
