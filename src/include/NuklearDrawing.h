#ifndef CLASS_NCINE_NUKLEARDRAWING
#define CLASS_NCINE_NUKLEARDRAWING

#include "NuklearContext.h"
#include "nuklear.h"

#include <nctl/UniquePtr.h>
#include "RenderCommand.h"
#include "GLTexture.h"
#include "Matrix4x4.h"

namespace ncine {

class GLShaderProgram;
class GLShaderUniforms;
class GLShaderAttributes;
class GLBufferObject;
class RenderQueue;

/// The class the handles Nuklear drawing
class NuklearDrawing
{
  public:
	explicit NuklearDrawing(bool withSceneGraph);

	/// Bakes the Nuklear font atlas and uploads it to a texture
	bool bakeFonts();

	void newFrame();
	/// Renders Nuklear with render commands
	void endFrame(RenderQueue &renderQueue);
	/// Renders Nuklear directly with OpenGL
	void endFrame();

  private:
	bool withSceneGraph_;
	nctl::UniquePtr<GLTexture> texture_;
	nctl::UniquePtr<GLShaderProgram> nuklearShaderProgram_;

	nctl::Array<nctl::UniquePtr<RenderCommand>> freeCommandsPool_;
	nctl::Array<nctl::UniquePtr<RenderCommand>> usedCommandsPool_;

	nctl::UniquePtr<GLBufferObject> vbo_;
	nctl::UniquePtr<GLBufferObject> ibo_;

	static const int UniformsBufferSize = 65;
	unsigned char uniformsBuffer_[UniformsBufferSize];
	nctl::UniquePtr<GLShaderUniforms> nuklearShaderUniforms_;
	nctl::UniquePtr<GLShaderAttributes> nuklearShaderAttributes_;

	int lastFrameWidth_;
	int lastFrameHeight_;
	Matrix4x4f projectionMatrix_;

	struct nk_convert_config config_;
	struct nk_draw_null_texture null_;
	nctl::UniquePtr<GLTexture> fontTex_;

	RenderCommand *retrieveCommandFromPool();
	void resetCommandPool();
	void setupRenderCmd(RenderCommand &cmd);
	void draw(RenderQueue &renderQueue);

	void setupBuffersAndShader();
	void draw();
};

}

#endif
