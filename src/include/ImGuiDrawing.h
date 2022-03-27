#ifndef CLASS_NCINE_IMGUIDRAWING
#define CLASS_NCINE_IMGUIDRAWING

#include <cstdint>
#include <nctl/UniquePtr.h>
#include "Matrix4x4.h"

namespace ncine {

class GLTexture;
class GLShaderProgram;
class GLShaderUniforms;
class GLBufferObject;
class RenderCommand;
class RenderQueue;

/// The class the handles ImGui drawing
class ImGuiDrawing
{
  public:
	explicit ImGuiDrawing(bool withSceneGraph);
	~ImGuiDrawing();

	/// Builds the ImGui font atlas and uploads it to a texture
	bool buildFonts();

	void newFrame();
	/// Renders ImGui with render commands
	void endFrame(RenderQueue &renderQueue);
	/// Renders ImGui directly with OpenGL
	void endFrame();

  private:
	bool withSceneGraph_;
	nctl::UniquePtr<GLTexture> texture_;
	nctl::UniquePtr<GLShaderProgram> imguiShaderProgram_;

	nctl::UniquePtr<GLBufferObject> vbo_;
	nctl::UniquePtr<GLBufferObject> ibo_;

	static const int UniformsBufferSize = 65;
	unsigned char uniformsBuffer_[UniformsBufferSize];
	nctl::UniquePtr<GLShaderUniforms> imguiShaderUniforms_;

	int lastFrameWidth_;
	int lastFrameHeight_;
	Matrix4x4f projectionMatrix_;
	uint16_t lastLayerValue_;

	RenderCommand *retrieveCommandFromPool();
	void setupRenderCmd(RenderCommand &cmd);
	void draw(RenderQueue &renderQueue);

	void setupBuffersAndShader();
	void draw();
};

}

#endif
