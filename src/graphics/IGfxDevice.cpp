#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "common_macros.h"
#include "IGfxDevice.h"
#include "Colorf.h"
#include "RenderResources.h"
#include "GLDepthTest.h"
#include "GLBlending.h"

#ifdef __EMSCRIPTEN__
	#include <emscripten/html5.h>
	#include "Application.h"
#endif

namespace ncine {

#ifdef __EMSCRIPTEN__
EM_BOOL IGfxDevice::resize_callback(int eventType, const EmscriptenUiEvent *event, void *userData)
{
	// Change resolution only if the canvas fills all the available space
	if (event->documentBodyClientWidth == 0 || event->documentBodyClientHeight == 0)
	{
		IGfxDevice *gfxDevice = reinterpret_cast<IGfxDevice *>(userData);
		gfxDevice->setResolution(event->windowInnerWidth, event->windowInnerHeight);
	}

	return 1;
}

EM_BOOL IGfxDevice::fullscreenchange_callback(int eventType, const EmscriptenFullscreenChangeEvent *event, void *userData)
{
	IGfxDevice *gfxDevice = reinterpret_cast<IGfxDevice *>(userData);
	gfxDevice->setResolution(event->elementWidth, event->elementHeight);
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
// CONSTRUCTORS AND DESTRUCTOR
///////////////////////////////////////////////////////////

IGfxDevice::IGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode)
    : width_(windowMode.width), height_(windowMode.height),
      isFullScreen_(windowMode.isFullScreen), isResizable_(windowMode.isResizable),
      glContextInfo_(glContextInfo), displayMode_(displayMode), numVideoModes_(0)
{
#ifdef __EMSCRIPTEN__
	double cssWidth = 0.0;
	double cssHeight = 0.0;
	// Referring to the first element of type <canvas> in the DOM
	emscripten_get_element_css_size("canvas", &cssWidth, &cssHeight);

	EmscriptenFullscreenChangeEvent fsce;
	emscripten_get_fullscreen_status(&fsce);

	width_ = static_cast<int>(cssWidth);
	height_ = static_cast<int>(cssHeight);
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

	currentVideoMode_.width = windowMode.width;
	currentVideoMode_.height = windowMode.height;
	currentVideoMode_.redBits = displayMode.redBits();
	currentVideoMode_.greenBits = displayMode.greenBits();
	currentVideoMode_.blueBits = displayMode.blueBits();
	currentVideoMode_.refreshRate = 60;
	videoModes_[0] = currentVideoMode_;
	numVideoModes_ = 1;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void IGfxDevice::setClearColor(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
}

void IGfxDevice::setClearColor(const Colorf &color)
{
	glClearColor(color.r(), color.g(), color.b(), color.a());
}

void IGfxDevice::setViewport(int width, int height)
{
	glViewport(0, 0, width, height);
	RenderResources::setProjectionMatrix(Matrix4x4f::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f));
}

const IGfxDevice::VideoMode &IGfxDevice::videoMode(unsigned int index) const
{
	ASSERT(index < numVideoModes_);
	if (index >= numVideoModes_)
		return videoModes_[0];
	return videoModes_[index];
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void IGfxDevice::setupGL()
{
	glDisable(GL_DITHER);
	GLBlending::blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLDepthTest::enable();
}

void IGfxDevice::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

}
