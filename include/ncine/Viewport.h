#ifndef CLASS_NCINE_VIEWPORT
#define CLASS_NCINE_VIEWPORT

#include "common_defines.h"
#include <nctl/UniquePtr.h>
#include <nctl/Array.h>
#include "Colorf.h"
#include "Vector2.h"
#include "Rect.h"

namespace ncine {

class SceneNode;
class Camera;
class RenderQueue;
class GLFramebufferObject;
class Texture;

/// The class handling a viewport and its corresponding render target texture
class DLL_PUBLIC Viewport
{
  public:
	enum class ClearMode
	{
		EVERY_FRAME,
		THIS_FRAME_ONLY,
		NEXT_FRAME_ONLY,
		NEVER
	};

	enum class ColorFormat
	{
		RGB8,
		RGBA8
	};

	enum class DepthStencilFormat
	{
		NONE,
		DEPTH16,
		DEPTH24,
		DEPTH24_STENCIL8
	};

	/// Creates a new viewport with no associated texture
	Viewport();

	/// Creates a new viewport with the specified dimensions and format
	Viewport(int width, int height, ColorFormat colorFormat, DepthStencilFormat depthStencilFormat);
	/// Creates a new viewport with the specified dimensions as a vector and format
	Viewport(const Vector2i &size, ColorFormat colorFormat, DepthStencilFormat depthStencilFormat);

	/// Creates a new viewport with the specified dimensions and a default format
	Viewport(int width, int height);
	/// Creates a new viewport with the specified dimensions as a vector and a default format
	Viewport(const Vector2i &size);

	~Viewport() {}

	/// Initializes the render target of the viewport with the specified dimensions and format
	bool initTexture(int width, int height, ColorFormat colorFormat, DepthStencilFormat depthStencilFormat);
	/// Initializes the render target of the viewport with the specified dimensions as a vector and format
	bool initTexture(const Vector2i &size, ColorFormat colorFormat, DepthStencilFormat depthStencilFormat);

	/// Initializes the render target of the viewport with the specified dimensions and the current format
	bool initTexture(int width, int height);
	/// Initializes the render target of the viewport with the specified dimensions as a vector and the current format
	bool initTexture(const Vector2i &size);

	/// Returns viewport size as a `Vector2i` object
	inline Vector2i size() const { return Vector2i(width_, height_); }
	/// Returns device width
	inline int width() const { return width_; }
	/// Returns device height
	inline int height() const { return height_; }

	/// Returns the OpenGL viewport rectangle
	inline Recti viewportRect() const { return viewportRect_; }
	/// Sets the OpenGL viewport rectangle
	inline void setViewportRect(Recti viewportRect) { viewportRect_ = viewportRect; }

	/// Returns the color format of the offscreen render target texture
	inline ColorFormat colorFormat() const { return colorFormat_; }
	/// Returns the depth and stencil format of the offscreen render target texture
	inline DepthStencilFormat depthStencilFormat() const { return depthStencilFormat_; }

	/// Returns the viewport clear mode
	inline ClearMode clearMode() const { return clearMode_; }
	/// Sets the viewport clear mode
	inline void setClearMode(ClearMode clearMode) { clearMode_ = clearMode; }

	/// Returns the viewport clear color as a `Colorf` object
	inline Colorf clearColor() const { return clearColor_; }
	/// Sets the viewport clear color through four floats
	inline void setClearColor(float red, float green, float blue, float alpha) { clearColor_.set(red, green, blue, alpha); }
	/// Sets the viewport clear color through a `Colorf` object
	inline void setClearColor(const Colorf &color) { clearColor_ = color; }

	/// Returns the offscreen render target texture
	inline Texture *texture() { return texture_.get(); }

	inline const SceneNode *rootNode() const { return rootNode_; }
	inline SceneNode *rootNode() { return rootNode_; }
	inline void setRootNode(SceneNode *rootNode) { rootNode_ = rootNode; }

	inline const Viewport *nextViewport() const { return nextViewport_; }
	inline void setNextViewport(Viewport *nextViewport) { nextViewport_ = nextViewport; }

	inline const Camera *camera() const { return camera_; }
	inline Camera *camera() { return camera_; }
	inline void setCamera(Camera *camera) { camera_ = camera; }

  protected:
	enum class Type
	{
		REGULAR,
		SCREEN
	};

	const Type type_;

	int width_;
	int height_;
	Recti viewportRect_;

	ColorFormat colorFormat_;
	DepthStencilFormat depthStencilFormat_;

	ClearMode clearMode_;
	Colorf clearColor_;

	/// The render queue of commands for the this viewport/RT
	nctl::UniquePtr<RenderQueue> renderQueue_;

	nctl::UniquePtr<GLFramebufferObject> fbo_;
	nctl::UniquePtr<Texture> texture_;

	/// The root scene node for this viewport/RT
	SceneNode *rootNode_;

	/// The camera used by this viewport
	/*! \note If set to `nullptr` it will use the default camera */
	Camera *camera_;

	/// Next viewport to render after this one
	Viewport *nextViewport_;

	Viewport(Type type);

	/// Deleted copy constructor
	Viewport(const Viewport &) = delete;
	/// Deleted assignment operator
	Viewport &operator=(const Viewport &) = delete;

	void update();
	void visit();
	void sortAndCommitQueue();
	void draw();

	friend class Application;
	friend class ScreenViewport;
};

}

#endif
