#include <android_native_app_glue.h> // for android_app
#include <android/configuration.h>
#if __ANDROID_API__ >= 30
	#include <android/native_window.h>
#endif

#include "common_macros.h"
#include "EglGfxDevice.h"
#include "AndroidJniHelper.h"
#include "AndroidApplication.h"

#ifdef __ANDROID__
	#include <jni.h>

extern "C"
{
	namespace nc = ncine;

	/// Called by `jnicall_functions.cpp`
	DLL_PUBLIC void updateMonitors(JNIEnv *env, jclass clazz)
	{
		nc::AndroidApplication &androidApp = static_cast<nc::AndroidApplication &>(nc::theApplication());
		if (androidApp.isInitialized())
		{
			JNIEnv *oldEnv = nc::AndroidJniHelper::jniEnv;
			nc::AndroidJniHelper::jniEnv = env;

			nc::EglGfxDevice::updateMonitorsFromJni();

			nc::AndroidJniHelper::jniEnv = oldEnv;
		}
	}
}
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

char EglGfxDevice::monitorNames_[MaxMonitors][MaxMonitorNameLength];

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

EglGfxDevice::EglGfxDevice(struct android_app *state, const GLContextInfo &glContextInfo, const DisplayMode &displayMode)
    : IGfxDevice(WindowMode(0, 0, 0.0f, 0, 0, true, false, false), glContextInfo, displayMode), state_(state)
{
	updateMonitors();
	initDevice();
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

const IGfxDevice::VideoMode &EglGfxDevice::currentVideoMode(unsigned int monitorIndex) const
{
	if (monitorIndex >= numMonitors_)
		monitorIndex = 0;

	AndroidJniClass_Display display = AndroidJniWrap_DisplayManager::getDisplay(monitorIndex);
	AndroidJniClass_DisplayMode mode = display.getMode();
	convertVideoModeInfo(mode, currentVideoMode_);

	return currentVideoMode_;
}

bool EglGfxDevice::setVideoMode(unsigned int modeIndex)
{
	const int monitorIndex = windowMonitorIndex();
	ASSERT(monitorIndex >= 0);

	const unsigned int numVideoModes = monitors_[monitorIndex].numVideoModes;
	ASSERT(modeIndex < numVideoModes);

#if defined(__ANDROID__) && __ANDROID_API__ >= 30
	if (modeIndex < monitors_[monitorIndex].numVideoModes)
	{
		const float refreshRate = monitors_[monitorIndex].videoModes[modeIndex].refreshRate;
		const int8_t compatibility = ANATIVEWINDOW_FRAME_RATE_COMPATIBILITY_DEFAULT;
	#if __ANDROID_API__ >= 31
		const int8_t changeFrameRateStrategy = ANATIVEWINDOW_CHANGE_FRAME_RATE_ALWAYS;
		const int result = ANativeWindow_setFrameRateWithChangeStrategy(state_->window, refreshRate, compatibility, changeFrameRateStrategy);
	#else
		const int result = ANativeWindow_setFrameRate(state_->window, refreshRate, compatibility);
	#endif
		return (result == 0);
	}
#endif
	return false;
}

void EglGfxDevice::createSurface()
{
	if (state_->window != nullptr)
	{
		surface_ = eglCreateWindowSurface(display_, config_, state_->window, nullptr);
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
	drawableWidth_ = width_;
	drawableHeight_ = height_;
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

#ifdef __ANDROID__
void EglGfxDevice::updateMonitorsFromJni()
{
	EglGfxDevice &gfxDevice = static_cast<EglGfxDevice &>(theApplication().gfxDevice());
	gfxDevice.updateMonitors();
}
#endif

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void EglGfxDevice::initDevice()
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
	ANativeWindow_setBuffersGeometry(state_->window, 0, 0, format);
#endif

	createSurface();
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

void EglGfxDevice::updateMonitors()
{
	const int32_t densityEnum = AConfiguration_getDensity(state_->config);
	unsigned int density = ACONFIGURATION_DENSITY_LOW;
	if (densityEnum != ACONFIGURATION_DENSITY_ANY && densityEnum != ACONFIGURATION_DENSITY_NONE)
		density = static_cast<unsigned int>(densityEnum);

	const float densityScale = density / static_cast<float>(ACONFIGURATION_DENSITY_LOW);

	AndroidJniClass_Display displays[MaxMonitors];
	const int monitorCount = AndroidJniWrap_DisplayManager::getDisplays(displays, MaxMonitors);
	ASSERT(monitorCount >= 1);
	numMonitors_ = (monitorCount < MaxMonitors) ? monitorCount : MaxMonitors;

	for (unsigned int i = 0; i < numMonitors_; i++)
	{
		displays[i].getName(monitorNames_[i], MaxMonitorNameLength);
		monitors_[i].name = monitorNames_[i];

		monitors_[i].position.x = 0;
		monitors_[i].position.y = 0;
		monitors_[i].scale.x = densityScale;
		monitors_[i].scale.y = densityScale;
		monitors_[i].dpi.x = density;
		monitors_[i].dpi.y = density;

		AndroidJniClass_DisplayMode modes[MaxVideoModes];
		const int modeCount = displays[i].getSupportedModes(modes, MaxVideoModes);
		monitors_[i].numVideoModes = (modeCount < MaxVideoModes) ? modeCount : MaxVideoModes;

		for (unsigned int j = 0; j < monitors_[i].numVideoModes; j++)
			convertVideoModeInfo(modes[j], monitors_[i].videoModes[j]);
	}
}

void EglGfxDevice::convertVideoModeInfo(const AndroidJniClass_DisplayMode &javaDisplayMode, IGfxDevice::VideoMode &videoMode) const
{
	videoMode.width = static_cast<unsigned int>(javaDisplayMode.getPhysicalWidth());
	videoMode.height = static_cast<unsigned int>(javaDisplayMode.getPhysicalHeight());
	videoMode.refreshRate = javaDisplayMode.getRefreshRate();
	// `android.view.Display.getPixelFormat()` has been deprecated in API level 17.
	// It now always returns `android.graphics.PixelFormat.RGBA_8888`.
	videoMode.redBits = 8;
	videoMode.greenBits = 8;
	videoMode.blueBits = 8;
}

}
