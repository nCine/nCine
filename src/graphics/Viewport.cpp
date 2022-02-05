#include <nctl/StaticString.h>
#include "Viewport.h"
#include "RenderQueue.h"
#include "RenderResources.h"
#include "Application.h"
#include "DrawableNode.h"
#include "Camera.h"
#include "GLFramebufferObject.h"
#include "Texture.h"
#include "GLClearColor.h"
#include "GLViewport.h"
#include "GLScissorTest.h"
#include "GLDebug.h"
#include "tracy.h"

#ifdef WITH_QT5
	#include "Qt5GfxDevice.h"
#endif

namespace ncine {

namespace {
	/// The string used to output OpenGL debug group information
	static nctl::StaticString<64> debugString;
}

Texture::Format colorFormatToTexFormat(Viewport::ColorFormat format)
{
	switch (format)
	{
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
      viewportRect_(0, 0, 0, 0), scissorRect_(0, 0, 0, 0),
      colorFormat_(ColorFormat::RGB8), depthStencilFormat_(DepthStencilFormat::NONE),
      clearMode_(ClearMode::EVERY_FRAME), clearColor_(Colorf::Black),
      renderQueue_(nctl::makeUnique<RenderQueue>()),
      fbo_(nullptr), texture_(nullptr),
      rootNode_(nullptr), camera_(nullptr), nextViewport_(nullptr)
{
}

Viewport::Viewport(int width, int height, ColorFormat colorFormat, DepthStencilFormat depthStencilFormat)
    : type_(Type::REGULAR), width_(0), height_(0),
      viewportRect_(0, 0, width, height), scissorRect_(0, 0, 0, 0),
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

Viewport::~Viewport() = default;

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

	if (type_ == Type::REGULAR)
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
	const float projWidth = projValues.right - projValues.left;
	const float projHeight = projValues.bottom - projValues.top;
	cullingRect_.set(projValues.left, projValues.top, projWidth, projHeight);

	const bool scissorRectNonZeroArea = (scissorRect_.w > 0 && scissorRect_.h > 0);
	if (scissorRectNonZeroArea)
	{
		Rectf scissorRectFloat(scissorRect_.x, scissorRect_.y, scissorRect_.w, scissorRect_.h);

		const bool viewportRectNonZeroArea = (viewportRect_.w > 0 && viewportRect_.h > 0);
		if (viewportRectNonZeroArea)
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

		scissorRectFloat.x = (scissorRectFloat.x * projWidth / static_cast<float>(width_)) + projValues.left;
		scissorRectFloat.y = (scissorRectFloat.y * projHeight / static_cast<float>(height_)) + projValues.top;
		scissorRectFloat.w *= projWidth / static_cast<float>(width_);
		scissorRectFloat.h *= projHeight / static_cast<float>(height_);

		cullingRect_.intersect(scissorRectFloat);
	}

	const Camera::ViewValues viewValues = vieportCamera->viewValues();
	if (viewValues.scale != 0.0f && viewValues.scale != 1.0f)
	{
		const float invScale = 1.0f / viewValues.scale;
		cullingRect_.x = (cullingRect_.x + viewValues.position.x) * invScale;
		cullingRect_.y = (cullingRect_.y + viewValues.position.y) * invScale;
		cullingRect_.w *= invScale;
		cullingRect_.h *= invScale;
	}
	else
	{
		cullingRect_.x += viewValues.position.x;
		cullingRect_.y += viewValues.position.y;
	}

	if (viewValues.rotation > SceneNode::MinRotation || viewValues.rotation < -SceneNode::MinRotation)
	{
		const float sinRot = sinf(-viewValues.rotation * fDegToRad);
		const float cosRot = cosf(-viewValues.rotation * fDegToRad);
		const float rotatedWidth = fabsf(cullingRect_.w * cosRot) + fabsf(cullingRect_.h * sinRot);
		const float rotatedHeight = fabsf(cullingRect_.w * sinRot) + fabsf(cullingRect_.h * cosRot);

		const Vector2f center = cullingRect_.center();
		// Using the inverse rotation angle
		const float rotatedX = cosRot * (center.x) + sinRot * (center.y);
		const float rotatedY = -sinRot * (center.x) + cosRot * (center.y);

		cullingRect_ = Rectf::fromCenterSize(rotatedX, rotatedY, rotatedWidth, rotatedHeight);
	}
}

void Viewport::update()
{
	if (nextViewport_)
		nextViewport_->update();

	RenderResources::setCurrentViewport(this);
	RenderResources::setCurrentCamera(camera_);

	calculateCullingRect();
	if (rootNode_)
	{
		ZoneScoped;
		if (rootNode_->lastFrameUpdated() < theApplication().numFrames())
			rootNode_->update(theApplication().interval());
		// AABBs should update after nodes have been transformed
		updateCulling(rootNode_);
	}
}

void Viewport::visit()
{
	if (nextViewport_)
		nextViewport_->visit();

	RenderResources::setCurrentViewport(this);

	if (rootNode_)
	{
		ZoneScoped;
		unsigned int visitOrderIndex = 0;
		rootNode_->visit(*renderQueue_, visitOrderIndex);
	}
}

void Viewport::sortAndCommitQueue()
{
	if (nextViewport_)
		nextViewport_->sortAndCommitQueue();

	RenderResources::setCurrentViewport(this);

	if (renderQueue_->isEmpty() == false)
	{
		ZoneScoped;
		renderQueue_->sortAndCommit();
	}
}

void Viewport::draw()
{
	if (nextViewport_ && nextViewport_->type_ == Type::REGULAR)
		nextViewport_->draw();

	ZoneScoped;
	if (type_ == Type::SCREEN)
		debugString.format("Draw root viewport (0x%lx)", uintptr_t(this));
	else if (type_ == Type::REGULAR && texture_->name() != nullptr)
		debugString.format("Draw viewport \"%s\" (0x%lx)", texture_->name(), uintptr_t(this));
	else
		debugString.format("Draw viewport (0x%lx)", uintptr_t(this));
	GLDebug::ScopedGroup(debugString.data());

	RenderResources::setCurrentViewport(this);

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

	RenderResources::setCurrentCamera(camera_);
	RenderResources::updateCameraUniforms();

	if (renderQueue_->isEmpty() == false)
	{
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
	}

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
	{
#ifdef WITH_QT5
		Qt5GfxDevice &gfxDevice = static_cast<Qt5GfxDevice &>(theApplication().gfxDevice());
		gfxDevice.bindDefaultDrawFramebufferObject();
#else
		fbo_->unbind(GL_DRAW_FRAMEBUFFER);
#endif
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Viewport::updateCulling(SceneNode *node)
{
	for (SceneNode *child : node->children())
		updateCulling(child);

	if (node->type() != Object::ObjectType::SCENENODE &&
	    node->type() != Object::ObjectType::PARTICLE_SYSTEM)
	{
		DrawableNode *drawable = static_cast<DrawableNode *>(node);
		drawable->updateCulling();
	}
}

}
