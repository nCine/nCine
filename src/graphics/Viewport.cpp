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
#include "GLScissorTest.h"
#include "tracy.h"

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

Viewport::Viewport()
    : type_(Type::NO_TEXTURE), width_(0), height_(0),
      viewportRect_(0, 0, 0, 0), hasDirtyCullingRect_(true),
      colorFormat_(ColorFormat::RGB8), depthStencilFormat_(DepthStencilFormat::NONE),
      clearMode_(ClearMode::EVERY_FRAME), clearColor_(Colorf::Black),
      renderQueue_(nctl::makeUnique<RenderQueue>(*this)),
      fbo_(nullptr), texture_(nullptr),
      rootNode_(nullptr), camera_(nullptr), nextViewport_(nullptr)
{
}

Viewport::Viewport(int width, int height, ColorFormat colorFormat, DepthStencilFormat depthStencilFormat)
    : type_(Type::REGULAR), width_(0), height_(0),
      viewportRect_(0, 0, width, height), hasDirtyCullingRect_(true),
      colorFormat_(colorFormat), depthStencilFormat_(depthStencilFormat),
      clearMode_(ClearMode::EVERY_FRAME), clearColor_(Colorf::Black),
      renderQueue_(nctl::makeUnique<RenderQueue>(*this)),
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
	ASSERT(width > 0 && height > 0);

	if (type_ == Type::SCREEN)
		return false;

	if (type_ == Type::NO_TEXTURE)
	{
		viewportRect_.set(0, 0, width, height);
		texture_ = nctl::makeUnique<Texture>();
		type_ = Type::REGULAR;
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

void Viewport::setNextViewport(Viewport *nextViewport)
{
	if (nextViewport != nullptr)
	{
		ASSERT(nextViewport->type_ != Type::SCREEN);
		if (nextViewport->type_ == Type::SCREEN)
			return;

		if (nextViewport->type_ == Type::NO_TEXTURE)
		{
			nextViewport->width_ = width_;
			nextViewport->height_ = height_;
			nextViewport->viewportRect_ = viewportRect_;
			nextViewport->colorFormat_ = colorFormat_;
			nextViewport->depthStencilFormat_ = depthStencilFormat_;
			nextViewport->clearMode_ = ClearMode::NEVER;
		}
	}

	nextViewport_ = nextViewport;
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

void Viewport::calculateCullingRect()
{
	ZoneScoped;

	const Camera *vieportCamera = camera_ ? camera_ : RenderResources::currentCamera();

	const Camera::ProjectionValues projValues = vieportCamera->projectionValues();
	cullingRect_.set(projValues.left, projValues.top, projValues.right - projValues.left, projValues.bottom - projValues.top);

	if (scissorRect_.w > 0 && scissorRect_.h > 0)
	{
		Rectf scissorRectFloat(scissorRect_.x, scissorRect_.y, scissorRect_.w, scissorRect_.h);

		if (viewportRect_.w > 0 && viewportRect_.h > 0)
		{
			scissorRectFloat.x -= viewportRect_.x;
			scissorRectFloat.y -= viewportRect_.y;

			const float viewportWidthRatio = width_ / static_cast<float>(viewportRect_.w);
			const float viewportHeightRatio = height_ / static_cast<float>(viewportRect_.h);
			scissorRectFloat.x *= viewportWidthRatio;
			scissorRectFloat.y *= viewportHeightRatio;
			scissorRectFloat.w *= viewportWidthRatio;
			scissorRectFloat.h *= viewportHeightRatio;
		}

		cullingRect_.intersect(scissorRectFloat);
	}

	const Camera::ViewValues viewValues = vieportCamera->viewValues();
	if (viewValues.scale != 0.0f && viewValues.scale != 1.0f)
	{
		const float invScale = 1.0f / viewValues.scale;
		cullingRect_.x += invScale * -viewValues.position.x;
		cullingRect_.y += invScale * -viewValues.position.y;
		cullingRect_.w *= invScale;
		cullingRect_.h *= invScale;
	}
	else
	{
		cullingRect_.x += -viewValues.position.x;
		cullingRect_.y += -viewValues.position.y;
	}

	if (viewValues.rotation > SceneNode::MinRotation || viewValues.rotation < -SceneNode::MinRotation)
	{
		const float sinRot = sinf(viewValues.rotation * fDegToRad);
		const float cosRot = cosf(viewValues.rotation * fDegToRad);
		const float rotatedWidth = fabsf(cullingRect_.w * cosRot) + fabsf(cullingRect_.h * sinRot);
		const float rotatedHeight = fabsf(cullingRect_.w * sinRot) + fabsf(cullingRect_.h * cosRot);

		const Vector2f center = cullingRect_.center();
		// Using the inverse rotation angle
		const float rotatedX = cosRot * (center.x) + sinRot * (center.y);
		const float rotatedY = -sinRot * (center.x) + cosRot * (center.y);

		cullingRect_ = Rectf::fromCenterSize(rotatedX, rotatedY, rotatedWidth, rotatedHeight);
	}

	hasDirtyCullingRect_ = (cullingRect_.x != prevCullingRect_.x || cullingRect_.y != prevCullingRect_.y ||
	                        cullingRect_.w != prevCullingRect_.w || cullingRect_.h != prevCullingRect_.h);
	prevCullingRect_ = cullingRect_;
}

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

	calculateCullingRect();

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
	if (nextViewport_ && nextViewport_->type_ == Type::REGULAR)
		nextViewport_->draw();

	if (type_ == Type::REGULAR)
		fbo_->bind(GL_DRAW_FRAMEBUFFER);

	if (type_ == Type::SCREEN || type_ == Type::REGULAR)
	{
		GLClearColor::State clearColorState = GLClearColor::state();
		GLClearColor::setColor(clearColor_);
		if (clearMode_ == ClearMode::EVERY_FRAME || clearMode_ == ClearMode::THIS_FRAME_ONLY)
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		GLClearColor::setState(clearColorState);
	}

	// This allows for sub-viewports that only change the camera and the OpenGL viewport
	if (nextViewport_ && nextViewport_->type_ == Type::NO_TEXTURE)
	{
		nextViewport_->clearMode_ = ClearMode::NEVER;
		nextViewport_->draw();
	}

	RenderResources::setCamera(camera_);

	const bool viewportRectNonZeroArea = (viewportRect_.w > 0 && viewportRect_.h > 0);
	GLViewport::State viewportState = GLViewport::state();
	if (viewportRectNonZeroArea)
		GLViewport::setRect(viewportRect_.x, viewportRect_.y, viewportRect_.w, viewportRect_.h);

	const bool scissorRectNonZeroArea = (scissorRect_.w > 0 && scissorRect_.h > 0);
	GLScissorTest::State scissorTestState = GLScissorTest::state();
	if (scissorRectNonZeroArea)
		GLScissorTest::enable(scissorRect_.x, scissorRect_.y, scissorRect_.w, scissorRect_.h);

	renderQueue_->draw();

	if (scissorRectNonZeroArea)
		GLScissorTest::setState(scissorTestState);
	if (viewportRectNonZeroArea)
		GLViewport::setState(viewportState);

	if (type_ == Type::REGULAR && depthStencilFormat_ != DepthStencilFormat::NONE &&
	    theApplication().appConfiguration().withGlDebugContext == false)
	{
		const GLenum invalidAttachment = depthStencilFormatToGLAttachment(depthStencilFormat_);
		fbo_->invalidate(1, &invalidAttachment);
	}

	if (clearMode_ == ClearMode::THIS_FRAME_ONLY)
		clearMode_ = ClearMode::NEVER;
	else if (clearMode_ == ClearMode::NEXT_FRAME_ONLY)
		clearMode_ = ClearMode::THIS_FRAME_ONLY;

	if (type_ == Type::REGULAR)
		fbo_->unbind(GL_DRAW_FRAMEBUFFER);
}

}
