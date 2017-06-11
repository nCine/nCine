#include <cstdlib> // for exit()
#include <android_native_app_glue.h> // for android_app
#include "EglGfxDevice.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

EglGfxDevice::EglGfxDevice(struct android_app *state, const GLContextInfo &contextInfo, const DisplayMode &mode)
	: IGfxDevice(-1, -1, mode, true)
{
	initDevice(state, contextInfo);
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

void EglGfxDevice::createSurface(struct android_app *state)
{
	if (state->window != NULL)
	{
		surface_ = eglCreateWindowSurface(display_, config_, state->window, NULL);
		if (surface_ == EGL_NO_SURFACE)
		{
			LOGF("eglCreateWindowSurface() returned EGL_NO_SURFACE");
			exit(EXIT_FAILURE);
		}
	}
}

void EglGfxDevice::bindContext()
{
	if (eglMakeCurrent(display_, surface_, surface_, context_) == EGL_FALSE)
	{
		LOGF("eglMakeCurrent() returned EGL_FALSE");
		exit(EXIT_FAILURE);
	}
}

void EglGfxDevice::unbindContext()
{
	if (eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) == EGL_FALSE)
	{
		LOGF("eglMakeCurrent() returned EGL_FALSE");
		exit(EXIT_FAILURE);
	}
}

void EglGfxDevice::querySurfaceSize()
{
	eglQuerySurface(display_, surface_, EGL_WIDTH, &width_);
	eglQuerySurface(display_, surface_, EGL_HEIGHT, &height_);
}

bool EglGfxDevice::isModeSupported(struct android_app *state, const GLContextInfo &contextInfo, const DisplayMode &mode)
{
	EGLint renderableTypeBit = (contextInfo.majorVersion == 3) ? EGL_OPENGL_ES3_BIT_KHR : EGL_OPENGL_ES2_BIT;

	const EGLint attribs[] =
	{
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE,	renderableTypeBit,
		EGL_BLUE_SIZE,			static_cast<int>(mode.blueBits()),
		EGL_GREEN_SIZE,			static_cast<int>(mode.greenBits()),
		EGL_RED_SIZE,			static_cast<int>(mode.redBits()),
		EGL_ALPHA_SIZE,			static_cast<int>(mode.alphaBits()),
		EGL_DEPTH_SIZE,			static_cast<int>(mode.depthBits()),
		EGL_STENCIL_SIZE,		static_cast<int>(mode.stencilBits()),
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

void EglGfxDevice::initDevice(struct android_app *state, const GLContextInfo &contextInfo)
{
	EGLint renderableTypeBit = (contextInfo.majorVersion == 3) ? EGL_OPENGL_ES3_BIT_KHR : EGL_OPENGL_ES2_BIT;

	const EGLint attribs[] =
	{
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE,	renderableTypeBit,
		EGL_BLUE_SIZE,			static_cast<int>(mode_.blueBits()),
		EGL_GREEN_SIZE,			static_cast<int>(mode_.greenBits()),
		EGL_RED_SIZE,			static_cast<int>(mode_.redBits()),
		EGL_ALPHA_SIZE,			static_cast<int>(mode_.alphaBits()),
		EGL_DEPTH_SIZE,			static_cast<int>(mode_.depthBits()),
		EGL_STENCIL_SIZE,		static_cast<int>(mode_.stencilBits()),
		EGL_NONE
	};

	EGLint attribList[] =
	{
		EGL_CONTEXT_MAJOR_VERSION_KHR, static_cast<EGLint>(contextInfo.majorVersion),
		EGL_CONTEXT_MINOR_VERSION_KHR, static_cast<EGLint>(contextInfo.minorVersion),
		EGL_NONE, EGL_NONE,
		EGL_NONE
	};

#if !defined(__ANDROID__) || (GL_ES_VERSION_3_0 && __ANDROID_API__ >= 21)
	if (contextInfo.debugContext)
	{
		attribList[4] = EGL_CONTEXT_FLAGS_KHR;
		attribList[5] = EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR;
	}
#endif

	EGLint format, numConfigs;

	display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display_, 0, 0);
	eglChooseConfig(display_, attribs, &config_, 1, &numConfigs);
	eglGetConfigAttrib(display_, config_, EGL_NATIVE_VISUAL_ID, &format);

#ifdef __ANDROID__
	ANativeWindow_setBuffersGeometry(state->window, 0, 0, format);
#endif

	createSurface(state);
	context_ = eglCreateContext(display_, config_, NULL, attribList);
	if (context_ == EGL_NO_CONTEXT)
	{
		LOGF("eglCreateContext() returned EGL_NO_CONTEXT");
		exit(EXIT_FAILURE);
	}

	bindContext();
	querySurfaceSize();

#ifndef __ANDROID__
	EGLint swapInterval = mode_.hasVSync() ? 1 : 0;
	eglSwapInterval(display_, swapInterval);
#endif

	if (contextInfo.debugContext)
	{
		enableGlDebugOutput();
	}

	EGLint red, blue, green, alpha, depth, stencil, samples;
	eglGetConfigAttrib(display_, config_, EGL_RED_SIZE, &red);
	eglGetConfigAttrib(display_, config_, EGL_GREEN_SIZE, &green);
	eglGetConfigAttrib(display_, config_, EGL_BLUE_SIZE, &blue);
	eglGetConfigAttrib(display_, config_, EGL_ALPHA_SIZE, &alpha);
	eglGetConfigAttrib(display_, config_, EGL_DEPTH_SIZE, &depth);
	eglGetConfigAttrib(display_, config_, EGL_STENCIL_SIZE, &stencil);
	eglGetConfigAttrib(display_, config_, EGL_SAMPLES, &samples);

	LOGI_X("Surface configuration is size:%dx%d, RGBA:%d%d%d%d, depth:%d, stencil:%d, samples:%d", width_, height_, red, green, blue, alpha, depth, stencil, samples);
}

}
