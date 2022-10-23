#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "common_macros.h"
#include "IGfxDevice.h"
#include "GLDepthTest.h"
#include "GLBlending.h"
#include "GLViewport.h"

#ifdef __EMSCRIPTEN__
	#include <emscripten/html5.h>
	#include "Application.h"
	#include "IAppEventHandler.h"
#endif

namespace ncine {

#ifdef __EMSCRIPTEN__
EM_BOOL IGfxDevice::resize_callback(int eventType, const EmscriptenUiEvent *event, void *userData)
{
	if (event->windowInnerWidth > 0 && event->windowInnerHeight > 0)
	{
		const double pixelRatio = emscripten_get_device_pixel_ratio();
		const int width = static_cast<int>(event->windowInnerWidth * pixelRatio);
		const int height = static_cast<int>(event->windowInnerHeight * pixelRatio);

		IGfxDevice *gfxDevice = reinterpret_cast<IGfxDevice *>(userData);
	#ifdef WITH_GLFW
		EmscriptenFullscreenChangeEvent fsce;
		emscripten_get_fullscreen_status(&fsce);
		// Setting the window size equal to screen size on GLFW will only
		// request a full screen without changing the canvas size
		if (width == fsce.screenWidth && height == fsce.screenHeight)
			gfxDevice->setWindowSize(width + 1, height + 1);
	#endif
		gfxDevice->setWindowSize(width, height);
		theApplication().resizeScreenViewport(width, height);
	}

	return 1;
}

EM_BOOL IGfxDevice::fullscreenchange_callback(int eventType, const EmscriptenFullscreenChangeEvent *event, void *userData)
{
	IGfxDevice *gfxDevice = reinterpret_cast<IGfxDevice *>(userData);
	gfxDevice->setFullScreen(event->isFullscreen);
	return 1;
}

EM_BOOL IGfxDevice::focus_callback(int eventType, const EmscriptenFocusEvent *event, void *userData)
{
	if (eventType == EMSCRIPTEN_EVENT_FOCUS)
		theApplication().setFocus(true);
	else if (eventType == EMSCRIPTEN_EVENT_BLUR)
		theApplication().setFocus(false);
	return 1;
}

#endif

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const float IGfxDevice::DefaultDpi = 96.0f;

///////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
///////////////////////////////////////////////////////////

IGfxDevice::IGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode)
    : width_(windowMode.width), height_(windowMode.height),
      drawableWidth_(windowMode.width), drawableHeight_(windowMode.height),
      isFullScreen_(windowMode.isFullScreen), isResizable_(windowMode.isResizable),
      glContextInfo_(glContextInfo), displayMode_(displayMode), numMonitors_(0),
      previousScalingFactor_(1.0f)
{
#ifdef __EMSCRIPTEN__
	double cssWidth = 0.0;
	double cssHeight = 0.0;
	// Referring to the first element of type <canvas> in the DOM
	emscripten_get_element_css_size("#canvas", &cssWidth, &cssHeight);
	const double pixelRatio = emscripten_get_device_pixel_ratio();

	EmscriptenFullscreenChangeEvent fsce;
	emscripten_get_fullscreen_status(&fsce);

	width_ = static_cast<int>(cssWidth * pixelRatio);
	height_ = static_cast<int>(cssHeight * pixelRatio);
	drawableWidth_ = width_;
	drawableHeight_ = height_;
	isFullScreen_ = fsce.isFullscreen;
	isResizable_ = true;

	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true, IGfxDevice::resize_callback);
	emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true, IGfxDevice::fullscreenchange_callback);

	// GLFW does not seem to correctly handle Emscripten focus and blur events
	#ifdef WITH_GLFW
	emscripten_set_blur_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, IGfxDevice::focus_callback);
	emscripten_set_focus_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, IGfxDevice::focus_callback);
	#endif
#endif

	currentVideoMode_.width = width_;
	currentVideoMode_.height = height_;
	currentVideoMode_.refreshRate = 0.0f;
	currentVideoMode_.redBits = displayMode.redBits();
	currentVideoMode_.greenBits = displayMode.greenBits();
	currentVideoMode_.blueBits = displayMode.blueBits();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/*! \internal Having this method inlined does not seem to work correctly with Qt5 on Linux */
unsigned int IGfxDevice::numMonitors() const
{
	return numMonitors_;
}

const IGfxDevice::Monitor &IGfxDevice::monitor(unsigned int index) const
{
	ASSERT(index < numMonitors_);
	if (index >= numMonitors_)
		index = 0;
	return monitors_[index];
}

float IGfxDevice::windowScalingFactor() const
{
#ifdef __APPLE__
	const float factor = drawableWidth() / static_cast<float>(width());
#else
	const Vector2f &scale = monitor().scale;
	const float factor = (scale.x + scale.y) * 0.5f;
#endif

	return factor;
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

void IGfxDevice::initWindowScaling(const WindowMode &windowMode)
{
	updateMonitors();
	const float factor = windowScalingFactor();

#ifndef __EMSCRIPTEN__
	if (windowMode.hasWindowScaling)
	{
	#ifdef WITH_QT5
		setWindowSize(width_ * factor, height_ * factor);
	#else
		width_ *= factor;
		height_ *= factor;
	#endif
	}
#endif

	previousScalingFactor_ = factor;
}

void IGfxDevice::initGLViewport()
{
	GLViewport::initRect(0, 0, drawableWidth_, drawableHeight_);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void IGfxDevice::setupGL()
{
	glDisable(GL_DITHER);
	GLBlending::setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLDepthTest::enable();
}

bool IGfxDevice::scaleWindowSize(bool windowScaling)
{
	const Monitor &windowMonitor = monitor();
	const float currentScalingFactor = (windowMonitor.scale.x + windowMonitor.scale.y) * 0.5f;
	const bool scalingFactorChanged = (currentScalingFactor != previousScalingFactor_);

#ifndef __APPLE__
	if (isFullScreen_ == false && scalingFactorChanged && windowScaling)
	{
		const float ratio = currentScalingFactor / previousScalingFactor_;
		setWindowSize(width_ * ratio, height_ * ratio);
	}
#endif

	previousScalingFactor_ = currentScalingFactor;
	return scalingFactorChanged;
}

}
