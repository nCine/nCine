#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "common_macros.h"
#include "IGfxDevice.h"
#include "Colorf.h"
#include "RenderResources.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
///////////////////////////////////////////////////////////

IGfxDevice::IGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode)
    : width_(windowMode.width), height_(windowMode.height),
      isFullScreen_(windowMode.isFullScreen), isResizable_(windowMode.isResizable),
      glContextInfo_(glContextInfo), displayMode_(displayMode)
{
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

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void IGfxDevice::setupGL()
{
	glDisable(GL_DITHER);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
}

void IGfxDevice::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

}
