#include <android_native_app_glue.h> // for android_app
#include "common_macros.h"
#include "EglGfxDevice.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

EglGfxDevice::EglGfxDevice(struct android_app *state, const GLContextInfo &glContextInfo, const DisplayMode &displayMode)
    : IGfxDevice(WindowMode(0, 0, true, false), glContextInfo, displayMode)
{
	initDevice(state);
}

EglGfxDevice::~EglGfxDevice()
{
	if (display_ != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

		if (context_ != EGL_NO_CONTEXT)
			eglDestroyContext(display_, context_);

		if (surface_ != EGL_NO_SURFACE)
			eglDestroySurface(display_, surface_);

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
	if (state->window != nullptr)
	{
		surface_ = eglCreateWindowSurface(display_, config_, state->window, nullptr);
		FATAL_ASSERT_MSG(surface_ != EGL_NO_SURFACE, "eglCreateWindowSurface() returned EGL_NO_SURFACE");
	}
}

void EglGfxDevice::bindContext()
{
	const EGLBoolean ret = eglMakeCurrent(display_, surface_, surface_, context_);
	FATAL_ASSERT_MSG(ret != EGL_FALSE, "eglMakeCurrent() returned EGL_FALSE");
}

void EglGfxDevice::unbindContext()
{
	const EGLBoolean ret = eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	FATAL_ASSERT_MSG(ret != EGL_FALSE, "eglMakeCurrent() returned EGL_FALSE");
}

void EglGfxDevice::querySurfaceSize()
{
	eglQuerySurface(display_, surface_, EGL_WIDTH, &width_);
	eglQuerySurface(display_, surface_, EGL_HEIGHT, &height_);
}

bool EglGfxDevice::isModeSupported(struct android_app *state, const GLContextInfo &glContextInfo, const DisplayMode &mode)
{
	const EGLint renderableTypeBit = (glContextInfo.majorVersion == 3) ? EGL_OPENGL_ES3_BIT_KHR : EGL_OPENGL_ES2_BIT;

	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, renderableTypeBit,
		EGL_BLUE_SIZE, static_cast<int>(mode.blueBits()),
		EGL_GREEN_SIZE, static_cast<int>(mode.greenBits()),
		EGL_RED_SIZE, static_cast<int>(mode.redBits()),
		EGL_ALPHA_SIZE, static_cast<int>(mode.alphaBits()),
		EGL_DEPTH_SIZE, static_cast<int>(mode.depthBits()),
		EGL_STENCIL_SIZE, static_cast<int>(mode.stencilBits()),
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

	EGLSurface surface = eglCreateWindowSurface(display, config, state->window, nullptr);

	const bool modeIsSupported = (surface != EGL_NO_SURFACE);

	if (surface != EGL_NO_SURFACE)
		eglDestroySurface(display, surface);

	eglTerminate(display);

	return modeIsSupported;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void EglGfxDevice::initDevice(struct android_app *state)
{
	const EGLint renderableTypeBit = (glContextInfo_.majorVersion == 3) ? EGL_OPENGL_ES3_BIT_KHR : EGL_OPENGL_ES2_BIT;

	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, renderableTypeBit,
		EGL_BLUE_SIZE, static_cast<int>(displayMode_.blueBits()),
		EGL_GREEN_SIZE, static_cast<int>(displayMode_.greenBits()),
		EGL_RED_SIZE, static_cast<int>(displayMode_.redBits()),
		EGL_ALPHA_SIZE, static_cast<int>(displayMode_.alphaBits()),
		EGL_DEPTH_SIZE, static_cast<int>(displayMode_.depthBits()),
		EGL_STENCIL_SIZE, static_cast<int>(displayMode_.stencilBits()),
		EGL_NONE
	};

	//const EGLint glProfileMaskBit = glContextInfo_.coreProfile ? EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR :
	//	EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR; // disabled

	EGLint attribList[] = {
		EGL_CONTEXT_MAJOR_VERSION_KHR, static_cast<EGLint>(glContextInfo_.majorVersion),
		EGL_CONTEXT_MINOR_VERSION_KHR, static_cast<EGLint>(glContextInfo_.minorVersion),
		//EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, glProfileMaskBit, // disabled
		EGL_NONE, EGL_NONE,
		EGL_NONE
	};

#if !defined(__ANDROID__) || (GL_ES_VERSION_3_0 && __ANDROID_API__ >= 21)
	if (glContextInfo_.forwardCompatible || glContextInfo_.debugContext)
	{
		attribList[4] = EGL_CONTEXT_FLAGS_KHR;
		EGLint contextFlagsMask = 0;
		contextFlagsMask |= (glContextInfo_.forwardCompatible) ? EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR : 0;
		contextFlagsMask |= (glContextInfo_.debugContext) ? EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR : 0;
		attribList[5] = contextFlagsMask;
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
	context_ = eglCreateContext(display_, config_, nullptr, attribList);
	FATAL_ASSERT_MSG(context_ != EGL_NO_CONTEXT, "eglCreateContext() returned EGL_NO_CONTEXT");

	bindContext();
	querySurfaceSize();

#ifndef __ANDROID__
	const EGLint swapInterval = mode_.hasVSync() ? 1 : 0;
	eglSwapInterval(display_, swapInterval);
#endif

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
