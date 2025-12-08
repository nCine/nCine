#ifndef CLASS_NCINE_IMGUIDRAWING
#define CLASS_NCINE_IMGUIDRAWING

#include <cstdint>
#include <nctl/UniquePtr.h>
#include <nctl/HashMap.h>
#include "Matrix4x4.h"

#if defined(WITH_OPENGLES) || defined(__EMSCRIPTEN__)
	#include <nctl/Array.h>
#endif

struct ImTextureData;

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

	void newFrame();
#ifdef WITH_SCENEGRAPH
	/// Renders ImGui with render commands
	void endFrame(RenderQueue &renderQueue);
#endif
	/// Renders ImGui directly with OpenGL
	void endFrame();

  private:
	bool withSceneGraph_;
	nctl::HashMap<GLTexture *, nctl::UniquePtr<GLTexture>> textures_;
#if defined(WITH_OPENGLES) || defined(__EMSCRIPTEN__)
	nctl::Array<char> tempTexBuffer_;
#endif
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

	void destroyTexture(ImTextureData *tex);
	void updateTexture(ImTextureData *tex);

#ifdef WITH_SCENEGRAPH
	RenderCommand *retrieveCommandFromPool();
	void setupRenderCmd(RenderCommand &cmd);
	void draw(RenderQueue &renderQueue);
#endif

	void setupBuffersAndShader();
	void draw();
};

}

#endif
