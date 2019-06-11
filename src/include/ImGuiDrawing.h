#ifndef CLASS_NCINE_IMGUIDRAWING
#define CLASS_NCINE_IMGUIDRAWING

#include <nctl/UniquePtr.h>
#include <nctl/StaticArray.h>
#include "RenderCommand.h"
#include "Matrix4x4.h"

namespace ncine {

class GLTexture;
class GLShaderProgram;
class GLShaderUniforms;
class GLShaderAttributes;
class GLBufferObject;
class RenderQueue;

/// The class the handles ImGui drawing
class ImGuiDrawing
{
  public:
	ImGuiDrawing(bool withSceneGraph);
	~ImGuiDrawing();

	void newFrame();
	/// Renders ImGui with render commands
	void endFrame(RenderQueue &renderQueue);
	/// Renders ImGui directly with OpenGL
	void endFrame();

  private:
	bool withSceneGraph_;
	nctl::UniquePtr<GLTexture> texture_;
	nctl::UniquePtr<GLShaderProgram> imguiShaderProgram_;

	nctl::Array<nctl::UniquePtr<RenderCommand>> freeCommandsPool_;
	nctl::Array<nctl::UniquePtr<RenderCommand>> usedCommandsPool_;

	nctl::UniquePtr<GLBufferObject> vbo_;
	nctl::UniquePtr<GLBufferObject> ibo_;

	static const int UniformsBufferSize = 65;
	unsigned char uniformsBuffer_[UniformsBufferSize];
	nctl::UniquePtr<GLShaderUniforms> imguiShaderUniforms_;
	nctl::UniquePtr<GLShaderAttributes> imguiShaderAttributes_;

	int lastFrameWidth_;
	int lastFrameHeight_;
	Matrix4x4f projectionMatrix_;

	RenderCommand *retrieveCommandFromPool();
	void resetCommandPool();
	void setupRenderCmd(RenderCommand &cmd);
	void draw(RenderQueue &renderQueue);

	void setupBuffersAndShader();
	void draw();
};

}

#endif
