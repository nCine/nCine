#include "Viewport.h"
#include "RenderQueue.h"
#include "RenderResources.h"
#include "Application.h"
#include "SceneNode.h"
#include "Camera.h"
#include "GLFramebufferObject.h"
#include "Texture.h"
#include "GLClearColor.h"
#include "GLViewport.h"

namespace ncine {

Texture::Format colorFormatToTexFormat(Viewport::ColorFormat format)
{
	switch (format)
	{
		default:
		case Viewport::ColorFormat::RGB8:
			return Texture::Format::RGB8;
		case Viewport::ColorFormat::RGBA8:
			return Texture::Format::RGBA8;
	}
}

GLenum depthStencilFormatToGLFormat(Viewport::DepthStencilFormat format)
{
	switch (format)
	{
		case Viewport::DepthStencilFormat::DEPTH16:
			return GL_DEPTH_COMPONENT16;
		case Viewport::DepthStencilFormat::DEPTH24:
			return GL_DEPTH_COMPONENT24;
		case Viewport::DepthStencilFormat::DEPTH24_STENCIL8:
			return GL_DEPTH24_STENCIL8;
		default:
			return GL_DEPTH24_STENCIL8;
	}
}

GLenum depthStencilFormatToGLAttachment(Viewport::DepthStencilFormat format)
{
	switch (format)
	{
		case Viewport::DepthStencilFormat::DEPTH16:
			return GL_DEPTH_ATTACHMENT;
		case Viewport::DepthStencilFormat::DEPTH24:
			return GL_DEPTH_ATTACHMENT;
		case Viewport::DepthStencilFormat::DEPTH24_STENCIL8:
			return GL_DEPTH_STENCIL_ATTACHMENT;
		default:
			return GL_DEPTH_STENCIL_ATTACHMENT;
	}
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Viewport::Viewport(Type type)
    : type_(type), width_(0), height_(0), viewportRect_(0, 0, 0, 0),
      colorFormat_(ColorFormat::RGB8), depthStencilFormat_(DepthStencilFormat::NONE),
      clearMode_(ClearMode::EVERY_FRAME), clearColor_(Colorf::Black),
      renderQueue_(nctl::makeUnique<RenderQueue>()),
      fbo_(nullptr), texture_(nullptr),
      rootNode_(nullptr), camera_(nullptr), nextViewport_(nullptr)
{
}

Viewport::Viewport()
    : Viewport(Type::REGULAR)
{
}

Viewport::Viewport(int width, int height, ColorFormat colorFormat, DepthStencilFormat depthStencilFormat)
    : type_(Type::REGULAR), width_(0), height_(0), viewportRect_(0, 0, width, height),
      colorFormat_(colorFormat), depthStencilFormat_(depthStencilFormat),
      clearMode_(ClearMode::EVERY_FRAME), clearColor_(Colorf::Black),
      renderQueue_(nctl::makeUnique<RenderQueue>()),
      fbo_(nullptr), texture_(nctl::makeUnique<Texture>()),
      rootNode_(nullptr), camera_(nullptr), nextViewport_(nullptr)
{
	const bool isInitialized = initTexture(width, height, colorFormat, depthStencilFormat);
	ASSERT(isInitialized);
}

Viewport::Viewport(const Vector2i &size, ColorFormat colorFormat, DepthStencilFormat depthStencilFormat)
    : Viewport(size.x, size.y, colorFormat, depthStencilFormat)
{
}

Viewport::Viewport(int width, int height)
    : Viewport(width, height, ColorFormat::RGB8, DepthStencilFormat::DEPTH24_STENCIL8)
{
}

Viewport::Viewport(const Vector2i &size)
    : Viewport(size.x, size.y, ColorFormat::RGB8, DepthStencilFormat::DEPTH24_STENCIL8)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool Viewport::initTexture(int width, int height, ColorFormat colorFormat, DepthStencilFormat depthStencilFormat)
{
	if (type_ == Type::SCREEN)
		return false;

	if (texture_ == nullptr)
	{
		viewportRect_.set(0, 0, width, height);
		texture_ = nctl::makeUnique<Texture>();
	}
	texture_->init("Viewport", colorFormatToTexFormat(colorFormat), width, height);

	fbo_ = nctl::makeUnique<GLFramebufferObject>();
	fbo_->attachTexture(*texture_->glTexture_, GL_COLOR_ATTACHMENT0);
	if (depthStencilFormat != DepthStencilFormat::NONE)
		fbo_->attachRenderbuffer(depthStencilFormatToGLFormat(depthStencilFormat), width, height, depthStencilFormatToGLAttachment(depthStencilFormat));
	const bool isStatusComplete = fbo_->isStatusComplete();

	if (isStatusComplete)
	{
		width_ = width;
		height_ = height;
		colorFormat_ = colorFormat;
		depthStencilFormat_ = depthStencilFormat;
	}
	return isStatusComplete;
}

bool Viewport::initTexture(const Vector2i &size, ColorFormat colorFormat, DepthStencilFormat depthStencilFormat)
{
	return initTexture(size.x, size.y, colorFormat, depthStencilFormat);
}

bool Viewport::initTexture(int width, int height)
{
	return initTexture(width, height, colorFormat_, depthStencilFormat_);
}

bool Viewport::initTexture(const Vector2i &size)
{
	return initTexture(size.x, size.y, colorFormat_, depthStencilFormat_);
}

void Viewport::resize(int width, int height)
{
	if (width == width_ && height == height_)
		return;

	if (fbo_ && texture_)
		initTexture(width, height);

	viewportRect_.set(0, 0, width, height);

	if (camera_ != nullptr)
		camera_->setOrthoProjection(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
	else if (type_ == Type::SCREEN)
		RenderResources::defaultCamera_->setOrthoProjection(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

	width_ = width;
	height_ = height;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Viewport::update()
{
	if (nextViewport_)
		nextViewport_->update();

	if (rootNode_)
		rootNode_->update(theApplication().interval());
}

void Viewport::visit()
{
	if (nextViewport_)
		nextViewport_->visit();

	if (rootNode_)
		rootNode_->visit(*renderQueue_);
}

void Viewport::sortAndCommitQueue()
{
	if (nextViewport_)
		nextViewport_->sortAndCommitQueue();

	renderQueue_->sortAndCommit();
}

void Viewport::draw()
{
	if (nextViewport_)
		nextViewport_->draw();

	if (fbo_ && texture_)
		fbo_->bind(GL_DRAW_FRAMEBUFFER);

	if (type_ == Type::SCREEN || (fbo_ && texture_))
	{
		GLClearColor::State clearColorState = GLClearColor::state();
		GLClearColor::setColor(clearColor_);
		if (clearMode_ == ClearMode::EVERY_FRAME || clearMode_ == ClearMode::THIS_FRAME_ONLY)
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		GLClearColor::setState(clearColorState);
	}

	RenderResources::setCamera(camera_);
	GLViewport::State viewportState = GLViewport::state();
	GLViewport::setRect(viewportRect_);
	renderQueue_->draw();
	GLViewport::setState(viewportState);

	// Camera dirty flags are reset in the `RenderQueue` class

	if (fbo_ && depthStencilFormat_ != DepthStencilFormat::NONE)
	{
		const GLenum invalidAttachment = depthStencilFormatToGLAttachment(depthStencilFormat_);
		fbo_->invalidate(1, &invalidAttachment);
	}

	if (clearMode_ == ClearMode::THIS_FRAME_ONLY)
		clearMode_ = ClearMode::NEVER;
	else if (clearMode_ == ClearMode::NEXT_FRAME_ONLY)
		clearMode_ = ClearMode::THIS_FRAME_ONLY;

	if (fbo_ && texture_)
		fbo_->unbind(GL_DRAW_FRAMEBUFFER);
}

}
