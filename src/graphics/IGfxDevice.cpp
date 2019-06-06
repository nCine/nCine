#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "common_macros.h"
#include "IGfxDevice.h"
#include "Colorf.h"
#include "RenderResources.h"
#include "GLDepthTest.h"
#include "GLBlending.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
///////////////////////////////////////////////////////////

IGfxDevice::IGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode)
    : width_(windowMode.width), height_(windowMode.height),
      isFullScreen_(windowMode.isFullScreen), isResizable_(windowMode.isResizable),
      glContextInfo_(glContextInfo), displayMode_(displayMode), numVideoModes_(0)
{
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
	RenderResources::projectionMatrix_ = Matrix4x4f::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

void IGfxDevice::setViewport(const Vector2i &size)
{
	glViewport(0, 0, size.x, size.y);
	RenderResources::projectionMatrix_ = Matrix4x4f::ortho(0.0f, size.x, 0.0f, size.y, -1.0f, 1.0f);
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
