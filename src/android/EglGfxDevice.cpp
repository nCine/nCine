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

/// Checks if the desired pixel format is supported
bool EglGfxDevice::isModeSupported(struct android_app *state, DisplayMode mode)
{
	const EGLint attribs[] =
	{
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_BLUE_SIZE,			mode.blueBits(),
		EGL_GREEN_SIZE,			mode.greenBits(),
		EGL_RED_SIZE,			mode.redBits(),
		EGL_ALPHA_SIZE,			mode.alphaBits(),
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
		EGL_BLUE_SIZE,			mode_.blueBits(),
		EGL_GREEN_SIZE,			mode_.greenBits(),
		EGL_RED_SIZE,			mode_.redBits(),
		EGL_ALPHA_SIZE,			mode_.alphaBits(),
		EGL_NONE
	};

	EGLint format, numConfigs;
	EGLConfig config;

	display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display_, 0, 0);
	eglChooseConfig(display_, attribs, &config, 1, &numConfigs);
	eglGetConfigAttrib(display_, config, EGL_NATIVE_VISUAL_ID, &format);

#ifdef __ANDROID__
	ANativeWindow_setBuffersGeometry(state->window, 0, 0, format);
#endif

	surface_ = eglCreateWindowSurface(display_, config, state->window, NULL);
	context_ = eglCreateContext(display_, config, NULL, NULL);

	if (eglMakeCurrent(display_, surface_, surface_, context_) == EGL_FALSE)
	{
		LOGF("Unable to eglMakeCurrent");
		exit(-1);
	}

	eglQuerySurface(display_, surface_, EGL_WIDTH, &width_);
	eglQuerySurface(display_, surface_, EGL_HEIGHT, &height_);


	EGLint red, blue, green, alpha, depth, stencil, samples;
	eglGetConfigAttrib(display_, config, EGL_RED_SIZE, &red);
	eglGetConfigAttrib(display_, config, EGL_GREEN_SIZE, &green);
	eglGetConfigAttrib(display_, config, EGL_BLUE_SIZE, &blue);
	eglGetConfigAttrib(display_, config, EGL_ALPHA_SIZE, &alpha);
	eglGetConfigAttrib(display_, config, EGL_DEPTH_SIZE, &depth);
	eglGetConfigAttrib(display_, config, EGL_STENCIL_SIZE, &stencil);
	eglGetConfigAttrib(display_, config, EGL_SAMPLES, &samples);

	LOGI_X("Surface configuration is RGBA:%d%d%d%d, depth:%d, stencil:%d, samples:%d", red, green, blue, alpha, depth, stencil, samples);
}

}
