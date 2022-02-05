#ifndef CLASS_NCINE_VIEWPORT
#define CLASS_NCINE_VIEWPORT

#include "common_defines.h"
#include <nctl/UniquePtr.h>
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
	/// The clear mode for a viewport with a texture or for the screen
	enum class ClearMode
	{
		/// The default behavior of clearing the viewport at every frame
		EVERY_FRAME,
		/// The viewport is cleared only once, at this frame
		THIS_FRAME_ONLY,
		/// The viewport is cleared only once, at next frame
		NEXT_FRAME_ONLY,
		/// The viewport is never cleared
		NEVER
	};

	/// The color format for a viewport with a texture or for the screen
	enum class ColorFormat
	{
		RGB8,
		RGBA8
	};

	/// The depth and stencil format for a viewport with a texture or for the screen
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
	explicit Viewport(const Vector2i &size);

	~Viewport();

	/// Initializes the render target of the viewport with the specified dimensions and format
	bool initTexture(int width, int height, ColorFormat colorFormat, DepthStencilFormat depthStencilFormat);
	/// Initializes the render target of the viewport with the specified dimensions as a vector and format
	bool initTexture(const Vector2i &size, ColorFormat colorFormat, DepthStencilFormat depthStencilFormat);

	/// Initializes the render target of the viewport with the specified dimensions and the current format
	bool initTexture(int width, int height);
	/// Initializes the render target of the viewport with the specified dimensions as a vector and the current format
	bool initTexture(const Vector2i &size);

	/// Changes the size, viewport rectangle and projection matrix of a viewport
	void resize(int width, int height);

	/// Returns viewport size as a `Vector2i` object
	inline Vector2i size() const { return Vector2i(width_, height_); }
	/// Returns viewport width
	inline int width() const { return width_; }
	/// Returns viewport height
	inline int height() const { return height_; }

	/// Returns the OpenGL viewport rectangle
	inline Recti viewportRect() const { return viewportRect_; }
	/// Sets the OpenGL viewport rectangle through a `Recti` object
	inline void setViewportRect(Recti viewportRect) { viewportRect_ = viewportRect; }
	/// Sets the OpenGL viewport rectangle with four integers
	inline void setViewportRect(int x, int y, int width, int height) { viewportRect_.set(x, y, width, height); }
	/// Resizes the OpenGL viewport rectangle through a `Vector2i` object
	inline void setViewportRect(const Vector2i &size) { viewportRect_.set(0, 0, size.x, size.y); }
	/// Resizes the OpenGL viewport rectangle with two integers
	inline void setViewportRect(int width, int height) { viewportRect_.set(0, 0, width, height); }

	/// Returns the OpenGL scissor test rectangle
	inline Recti scissorRect() const { return scissorRect_; }
	/// Sets the OpenGL scissor test rectangle through a `Recti` object
	inline void setScissorRect(Recti scissorRect) { scissorRect_ = scissorRect; }
	/// Sets the OpenGL scissor test rectangle with four integers
	inline void setScissorRect(int x, int y, int width, int height) { scissorRect_.set(x, y, width, height); }
	/// Resizes the OpenGL scissor test rectangle through a `Vector2i` object
	inline void setScissorRect(const Vector2i &size) { scissorRect_.set(0, 0, size.x, size.y); }
	/// Resizes the OpenGL scissor test rectangle with two integers
	inline void setScissorRect(int width, int height) { scissorRect_.set(0, 0, width, height); }

	/// Returns the rectangle for screen culling
	inline Rectf cullingRect() const { return cullingRect_; }

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

	/// Returns the root node as a constant
	inline const SceneNode *rootNode() const { return rootNode_; }
	/// Returns the root node
	inline SceneNode *rootNode() { return rootNode_; }
	/// Sets the root node
	inline void setRootNode(SceneNode *rootNode) { rootNode_ = rootNode; }

	/// Returns the next viewport in the rendering chain as a constant
	inline const Viewport *nextViewport() const { return nextViewport_; }
	/// Returns the next viewport in the rendering chain
	inline Viewport *nextViewport() { return nextViewport_; }
	/// Sets the next viewport in the rendering chain
	void setNextViewport(Viewport *nextViewport);

	/// Returns the camera used for rendering as a constant
	inline const Camera *camera() const { return camera_; }
	/// Returns the camera used for rendering
	inline Camera *camera() { return camera_; }
	/// Sets the camera to be used for rendering
	inline void setCamera(Camera *camera) { camera_ = camera; }

  protected:
	/// An enumeration to differentiate between a regular viewport, a textureless one and the screen (root viewport)
	enum class Type
	{
		REGULAR,
		NO_TEXTURE,
		SCREEN
	};

	Type type_;

	int width_;
	int height_;
	Recti viewportRect_;
	Recti scissorRect_;
	Rectf cullingRect_;

	ColorFormat colorFormat_;
	DepthStencilFormat depthStencilFormat_;

	ClearMode clearMode_;
	Colorf clearColor_;

	/// The render queue of commands for this viewport/RT
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

	/// Deleted copy constructor
	Viewport(const Viewport &) = delete;
	/// Deleted assignment operator
	Viewport &operator=(const Viewport &) = delete;

	void calculateCullingRect();

	void update();
	void visit();
	void sortAndCommitQueue();
	void draw();

  private:
	void updateCulling(SceneNode *node);

	friend class Application;
};

}

#endif
