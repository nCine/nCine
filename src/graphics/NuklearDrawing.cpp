#include <cstring> // for `memset()`
#include <cstddef> // for `offsetof()`
#include "NuklearDrawing.h"

#include "GLShaderProgram.h"
#include "GLScissorTest.h"
#include "GLBlending.h"
#include "GLDepthTest.h"
#include "GLCullFace.h"
#include "GLViewport.h"
#include "RenderQueue.h"
#include "RenderCommandPool.h"
#include "RenderResources.h"
#include "Application.h"

#if defined(WITH_GLFW)
	#include "NuklearGlfwInput.h"
#elif defined(WITH_SDL)
	#include "NuklearSdlInput.h"
#elif defined(WITH_QT5)
	#include "NuklearQt5Input.h"
#elif defined(__ANDROID__)
	#include "NuklearAndroidInput.h"
#endif

#ifdef WITH_EMBEDDED_SHADERS
	#include "shader_strings.h"
#else
	#include "FileSystem.h" // for dataPath()
#endif

namespace ncine {

namespace {

	struct nk_vertex
	{
		float pos[2];
		float uv[2];
		nk_byte col[4];
	};

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

NuklearDrawing::NuklearDrawing(bool withSceneGraph)
    : withSceneGraph_(withSceneGraph),
      lastFrameWidth_(0), lastFrameHeight_(0), lastLayerValue_(0)
{
	const AppConfiguration &appCfg = theApplication().appConfiguration();
	const GLShaderProgram::QueryPhase queryPhase = appCfg.deferShaderQueries ? GLShaderProgram::QueryPhase::DEFERRED : GLShaderProgram::QueryPhase::IMMEDIATE;

	// Nuklear shaders are the same as ImGui
	nuklearShaderProgram_ = nctl::makeUnique<GLShaderProgram>(queryPhase);
#ifndef WITH_EMBEDDED_SHADERS
	nuklearShaderProgram_->attachShaderFromFile(GL_VERTEX_SHADER, (fs::dataPath() + "shaders/imgui_vs.glsl").data());
	nuklearShaderProgram_->attachShaderFromFile(GL_FRAGMENT_SHADER, (fs::dataPath() + "shaders/imgui_fs.glsl").data());
#else
	// Skipping the initial new line character of the raw string literal
	nuklearShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::imgui_vs + 1);
	nuklearShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::imgui_fs + 1);
#endif
	nuklearShaderProgram_->link(GLShaderProgram::Introspection::ENABLED);

	if (withSceneGraph == false)
		setupBuffersAndShader();

	nk_font_atlas_begin(&NuklearContext::atlas_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool NuklearDrawing::bakeFonts()
{
	const void *image;
	int w, h;
	image = nk_font_atlas_bake(&NuklearContext::atlas_, &w, &h, NK_FONT_ATLAS_RGBA32);
	fontTex_ = nctl::makeUnique<GLTexture>(GL_TEXTURE_2D);
	fontTex_->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	fontTex_->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	fontTex_->texImage2D(0, GL_RGBA8, w, h, GL_RGBA, GL_UNSIGNED_BYTE, image);

	nk_font_atlas_end(&NuklearContext::atlas_, nk_handle_ptr(fontTex_.get()), &null_);
	if (NuklearContext::atlas_.default_font)
		nk_style_set_font(&NuklearContext::ctx_, &NuklearContext::atlas_.default_font->handle);

	// Setting the convert configuration after the atlas is baked
	static const struct nk_draw_vertex_layout_element vertex_layout[] = {
		{ NK_VERTEX_POSITION, NK_FORMAT_FLOAT, offsetof(nk_vertex, pos) },
		{ NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, offsetof(nk_vertex, uv) },
		{ NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, offsetof(nk_vertex, col) },
		{ NK_VERTEX_LAYOUT_END }
	};
	memset(&config_, 0, sizeof(config_));
	config_.vertex_layout = vertex_layout;
	config_.vertex_size = sizeof(nk_vertex);
	config_.vertex_alignment = alignof(nk_vertex);
	config_.tex_null = null_;
	config_.circle_segment_count = 22;
	config_.curve_segment_count = 22;
	config_.arc_segment_count = 22;
	config_.global_alpha = 1.0f;
	config_.shape_AA = NK_ANTI_ALIASING_ON;
	config_.line_AA = NK_ANTI_ALIASING_ON;

	return (image != nullptr);
}

void NuklearDrawing::newFrame()
{
#if defined(WITH_GLFW)
	NuklearGlfwInput::newFrame();
#elif defined(WITH_SDL)
	NuklearSdlInput::newFrame();
#elif defined(WITH_QT5)
	NuklearQt5Input::newFrame();
#elif defined(__ANDROID__)
	NuklearAndroidInput::newFrame();
#endif

	if (lastFrameWidth_ != NuklearContext::width_ || lastFrameHeight_ != NuklearContext::height_)
	{
		projectionMatrix_ = Matrix4x4f::ortho(0.0f, NuklearContext::width_, NuklearContext::height_, 0.0f, -1.0f, 1.0f);

		if (withSceneGraph_ == false)
		{
			nuklearShaderUniforms_->uniform(Material::GuiProjectionMatrixUniformName)->setFloatVector(projectionMatrix_.data());
			nuklearShaderUniforms_->uniform(Material::DepthUniformName)->setFloatValue(0.0f);
			nuklearShaderUniforms_->commitUniforms();
		}
	}
}

void NuklearDrawing::endFrame(RenderQueue &renderQueue)
{
	draw(renderQueue);
}

void NuklearDrawing::endFrame()
{
	draw();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

RenderCommand *NuklearDrawing::retrieveCommandFromPool()
{
	bool commandAdded = false;
	RenderCommand *retrievedCommand = RenderResources::renderCommandPool().retrieveOrAdd(nuklearShaderProgram_.get(), commandAdded);
	if (commandAdded)
		setupRenderCmd(*retrievedCommand);

	return retrievedCommand;
}

void NuklearDrawing::setupRenderCmd(RenderCommand &cmd)
{
	cmd.setProfilingType(RenderCommand::CommandTypes::NUKLEAR);

	Material &material = cmd.material();
	material.setShaderProgram(nuklearShaderProgram_.get());
	material.reserveUniformsDataMemory();
	material.uniform(Material::TextureUniformName)->setIntValue(0); // GL_TEXTURE0
	nuklearShaderProgram_->attribute(Material::PositionAttributeName)->setVboParameters(sizeof(nk_vertex), reinterpret_cast<void *>(offsetof(nk_vertex, pos)));
	nuklearShaderProgram_->attribute(Material::TexCoordsAttributeName)->setVboParameters(sizeof(nk_vertex), reinterpret_cast<void *>(offsetof(nk_vertex, uv)));
	nuklearShaderProgram_->attribute(Material::ColorAttributeName)->setVboParameters(sizeof(nk_vertex), reinterpret_cast<void *>(offsetof(nk_vertex, col)));
	nuklearShaderProgram_->attribute(Material::ColorAttributeName)->setType(GL_UNSIGNED_BYTE);
	nuklearShaderProgram_->attribute(Material::ColorAttributeName)->setNormalized(true);
	material.setBlendingEnabled(true);
	material.setBlendingFactors(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	cmd.geometry().setNumElementsPerVertex(sizeof(nk_vertex) / sizeof(GLfloat));
	cmd.geometry().setDrawParameters(GL_TRIANGLES, 0, 0);
}

void NuklearDrawing::draw(RenderQueue &renderQueue)
{
	const unsigned int numElements = sizeof(nk_vertex) / sizeof(GLfloat);

	RenderCommand &firstCmd = *retrieveCommandFromPool();
	if (lastFrameWidth_ != NuklearContext::width_ || lastFrameHeight_ != NuklearContext::height_)
	{
		firstCmd.material().uniform(Material::GuiProjectionMatrixUniformName)->setFloatVector(projectionMatrix_.data());
		lastFrameWidth_ = NuklearContext::width_;
		lastFrameHeight_ = NuklearContext::height_;
	}

	/* convert from command queue into draw list and draw to screen */
	const struct nk_draw_command *cmd;
	struct nk_buffer vbuf, ibuf;

	// setup buffers to load vertices and indices
	nk_buffer_init_default(&vbuf);
	nk_buffer_init_default(&ibuf);
	nk_convert(&NuklearContext::ctx_, &NuklearContext::cmds_, &vbuf, &ibuf, &config_);

	// copy draw vertices & indices into vertex + indices buffer
	firstCmd.geometry().shareVbo(nullptr);
	GLfloat *vertices = firstCmd.geometry().acquireVertexPointer(vbuf.size / sizeof(GLfloat), numElements);
	memcpy(vertices, vbuf.memory.ptr, vbuf.size);
	firstCmd.geometry().releaseVertexPointer();

	firstCmd.geometry().shareIbo(nullptr);
	GLushort *indices = firstCmd.geometry().acquireIndexPointer(ibuf.size / sizeof(GLushort));
	memcpy(indices, ibuf.memory.ptr, ibuf.size);
	firstCmd.geometry().releaseIndexPointer();

	if (lastLayerValue_ != theApplication().guiSettings().nuklearLayer)
	{
		// It is enough to set the uniform value once as every Nuklear command share the same shader
		const float depth = RenderCommand::calculateDepth(theApplication().guiSettings().nuklearLayer, -1.0f, 1.0f);
		firstCmd.material().uniform(Material::DepthUniformName)->setFloatValue(depth);
		lastLayerValue_ = theApplication().guiSettings().nuklearLayer;
	}

	nk_buffer_free(&vbuf);
	nk_buffer_free(&ibuf);

	// iterate over and execute each draw command
	GLushort firstIndex = 0;
	int cmdIdx = 0;
	nk_draw_foreach(cmd, &NuklearContext::ctx_, &NuklearContext::cmds_)
	{
		RenderCommand &currCmd = (cmdIdx == 0) ? firstCmd : *retrieveCommandFromPool();

		if (!cmd->elem_count)
			continue;

		currCmd.setScissor(static_cast<GLint>(cmd->clip_rect.x * NuklearContext::fbScale_.x),
		                   static_cast<GLint>((NuklearContext::height_ - (cmd->clip_rect.y + cmd->clip_rect.h)) * NuklearContext::fbScale_.y),
		                   static_cast<GLsizei>(cmd->clip_rect.w * NuklearContext::fbScale_.x),
		                   static_cast<GLsizei>(cmd->clip_rect.h * NuklearContext::fbScale_.y));

		if (cmdIdx > 0)
		{
			currCmd.geometry().shareVbo(&firstCmd.geometry());
			currCmd.geometry().shareIbo(&firstCmd.geometry());
		}

		currCmd.geometry().setNumIndices(cmd->elem_count);
		currCmd.geometry().setFirstIndex(firstIndex);
		currCmd.setLayer(theApplication().guiSettings().nuklearLayer);
		currCmd.setVisitOrder(cmdIdx);
		currCmd.material().setTexture(reinterpret_cast<GLTexture *>(cmd->texture.ptr));

		renderQueue.addCommand(&currCmd);

		firstIndex += cmd->elem_count;
		cmdIdx++;
	}
	nk_clear(&NuklearContext::ctx_);
	nk_buffer_clear(&NuklearContext::cmds_);
}

void NuklearDrawing::setupBuffersAndShader()
{
	vbo_ = nctl::makeUnique<GLBufferObject>(GL_ARRAY_BUFFER);
	ibo_ = nctl::makeUnique<GLBufferObject>(GL_ELEMENT_ARRAY_BUFFER);

	nuklearShaderUniforms_ = nctl::makeUnique<GLShaderUniforms>(nuklearShaderProgram_.get());
	nuklearShaderUniforms_->setUniformsDataPointer(uniformsBuffer_);
	nuklearShaderUniforms_->uniform(Material::TextureUniformName)->setIntValue(0); // GL_TEXTURE0

	nuklearShaderProgram_->attribute(Material::PositionAttributeName)->setVboParameters(sizeof(nk_vertex), reinterpret_cast<void *>(offsetof(nk_vertex, pos)));
	nuklearShaderProgram_->attribute(Material::TexCoordsAttributeName)->setVboParameters(sizeof(nk_vertex), reinterpret_cast<void *>(offsetof(nk_vertex, uv)));
	nuklearShaderProgram_->attribute(Material::ColorAttributeName)->setVboParameters(sizeof(nk_vertex), reinterpret_cast<void *>(offsetof(nk_vertex, col)));
	nuklearShaderProgram_->attribute(Material::ColorAttributeName)->setType(GL_UNSIGNED_BYTE);
	nuklearShaderProgram_->attribute(Material::ColorAttributeName)->setNormalized(true);
}

void NuklearDrawing::draw()
{
	/* convert from command queue into draw list and draw to screen */
	const struct nk_draw_command *cmd;
	struct nk_buffer vbuf, ibuf;

	// setup buffers to load vertices and indices
	nk_buffer_init_default(&vbuf);
	nk_buffer_init_default(&ibuf);
	nk_convert(&NuklearContext::ctx_, &NuklearContext::cmds_, &vbuf, &ibuf, &config_);

	// Always define vertex format (and bind VAO) before uploading data to buffers
	nuklearShaderProgram_->defineVertexFormat(vbo_.get(), ibo_.get());
	/* copy to vertex and element buffer */
	vbo_->bufferData(vbuf.size, vbuf.memory.ptr, GL_STREAM_DRAW);
	ibo_->bufferData(ibuf.size, ibuf.memory.ptr, GL_STREAM_DRAW);

	nk_buffer_free(&vbuf);
	nk_buffer_free(&ibuf);

	// setup global state
	GLBlending::State blendingState = GLBlending::state();
	GLBlending::enable();
	GLBlending::setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLCullFace::State cullFaceState = GLCullFace::state();
	GLCullFace::disable();
	GLDepthTest::State depthTestState = GLDepthTest::state();
	GLDepthTest::disable();
	GLViewport::State viewportState = GLViewport::state();

	// setup program
	nuklearShaderProgram_->use();
	GLViewport::setRect(0, 0, NuklearContext::displayWidth_, NuklearContext::displayHeight_);

	// iterate over and execute each draw command
	const nk_draw_index *offset = nullptr;
	nk_draw_foreach(cmd, &NuklearContext::ctx_, &NuklearContext::cmds_)
	{
		if (!cmd->elem_count)
			continue;

		GLScissorTest::enable(static_cast<GLint>(cmd->clip_rect.x * NuklearContext::fbScale_.x),
		                      static_cast<GLint>((NuklearContext::height_ - (cmd->clip_rect.y + cmd->clip_rect.h)) * NuklearContext::fbScale_.y),
		                      static_cast<GLsizei>(cmd->clip_rect.w * NuklearContext::fbScale_.x),
		                      static_cast<GLsizei>(cmd->clip_rect.h * NuklearContext::fbScale_.y));

		GLTexture::bindHandle(GL_TEXTURE_2D, reinterpret_cast<GLTexture *>(cmd->texture.ptr)->glHandle());
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cmd->elem_count), GL_UNSIGNED_SHORT, offset);
		offset += cmd->elem_count;
	}
	nk_clear(&NuklearContext::ctx_);
	nk_buffer_clear(&NuklearContext::cmds_);

	GLScissorTest::disable();
	GLViewport::setState(viewportState);
	GLDepthTest::setState(depthTestState);
	GLCullFace::setState(cullFaceState);
	GLBlending::setState(blendingState);
}

}
