#include "imgui.h"

#include "ImGuiDrawing.h"
#include "GLTexture.h"
#include "GLShaderProgram.h"
#include "GLScissorTest.h"
#include "GLBlending.h"
#include "GLDepthTest.h"
#include "GLCullFace.h"
#include "RenderQueue.h"
#include "RenderCommandPool.h"
#include "RenderResources.h"
#include "Application.h"

#if defined(WITH_GLFW)
	#include "ImGuiGlfwInput.h"
#elif defined(WITH_SDL)
	#include "ImGuiSdlInput.h"
#elif defined(WITH_QT5)
	#include "ImGuiQt5Input.h"
#elif defined(__ANDROID__)
	#include <nctl/CString.h>
	#include "FileSystem.h" // for savePath()
	#include "ImGuiAndroidInput.h"
#endif

#ifdef WITH_EMBEDDED_SHADERS
	#include "shader_strings.h"
#else
	#include "FileSystem.h" // for dataPath()
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ImGuiDrawing::ImGuiDrawing(bool withSceneGraph)
    : withSceneGraph_(withSceneGraph),
      lastFrameWidth_(0), lastFrameHeight_(0), lastLayerValue_(0)
{
	ImGuiIO &io = ImGui::GetIO();
#if defined(WITH_OPENGLES) || defined(__EMSCRIPTEN)
	io.BackendRendererName = "nCine_OpenGL_ES";
#else
	io.BackendRendererName = "nCine_OpenGL";
#endif

#ifdef __ANDROID__
	static char iniFilename[512];
	nctl::strncpy(iniFilename, fs::joinPath(fs::savePath(), "imgui.ini").data(), 512);
	io.IniFilename = iniFilename;
#endif

#if !(defined(WITH_OPENGLES) && !GL_ES_VERSION_3_2) && !defined(__EMSCRIPTEN__)
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
#endif

	const AppConfiguration &appCfg = theApplication().appConfiguration();
	const GLShaderProgram::QueryPhase queryPhase = appCfg.deferShaderQueries ? GLShaderProgram::QueryPhase::DEFERRED : GLShaderProgram::QueryPhase::IMMEDIATE;

	imguiShaderProgram_ = nctl::makeUnique<GLShaderProgram>(queryPhase);

	RenderResources::ShaderProgramCompileInfo shaderProgramInfo(imguiShaderProgram_, GLShaderProgram::Introspection::ENABLED, "ImGui");
#ifndef WITH_EMBEDDED_SHADERS
	const nctl::String vertexShaderPath = fs::dataPath() + RenderResources::ShadersDir + "/imgui_vs.glsl";
	const nctl::String fragmentShaderPath = fs::dataPath() + RenderResources::ShadersDir + "/imgui_fs.glsl";
	shaderProgramInfo.vertexInfo.shaderFile = vertexShaderPath.data();
	shaderProgramInfo.fragmentInfo.shaderFile = fragmentShaderPath.data();
#else
	// Skipping the initial new line character of the raw string literal
	shaderProgramInfo.vertexInfo.shaderString = ShaderStrings::imgui_vs + 1;
	shaderProgramInfo.fragmentInfo.shaderString = ShaderStrings::imgui_fs + 1;
	shaderProgramInfo.vertexInfo.hashString = ShaderHashes::imgui_vs;
	shaderProgramInfo.fragmentInfo.hashString = ShaderHashes::imgui_fs;
#endif
	RenderResources::compileShader(shaderProgramInfo);

	if (withSceneGraph == false)
		setupBuffersAndShader();

	unsigned char *pixels = nullptr;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
}

ImGuiDrawing::~ImGuiDrawing()
{
	ImGuiIO &io = ImGui::GetIO();
	io.Fonts->SetTexID(static_cast<ImTextureID>(reinterpret_cast<intptr_t>(nullptr)));
	io.BackendRendererName = nullptr;
	io.BackendFlags &= ~ImGuiBackendFlags_RendererHasVtxOffset;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool ImGuiDrawing::buildFonts()
{
	ImGuiIO &io = ImGui::GetIO();

	unsigned char *pixels = nullptr;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	texture_ = nctl::makeUnique<GLTexture>(GL_TEXTURE_2D);
	texture_->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture_->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	texture_->texImage2D(0, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	io.Fonts->TexID = reinterpret_cast<ImTextureID>(texture_.get());

	return (pixels != nullptr);
}

void ImGuiDrawing::newFrame()
{
#if defined(WITH_GLFW)
	ImGuiGlfwInput::newFrame();
#elif defined(WITH_SDL)
	ImGuiSdlInput::newFrame();
#elif defined(WITH_QT5)
	ImGuiQt5Input::newFrame();
#elif defined(__ANDROID__)
	ImGuiAndroidInput::newFrame();
#endif

	ImGuiIO &io = ImGui::GetIO();

	if (lastFrameWidth_ != io.DisplaySize.x || lastFrameHeight_ != io.DisplaySize.y)
	{
		projectionMatrix_ = Matrix4x4f::ortho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, 1.0f);

		if (withSceneGraph_ == false)
		{
			imguiShaderUniforms_->uniform(Material::GuiProjectionMatrixUniformName)->setFloatVector(projectionMatrix_.data());
			imguiShaderUniforms_->uniform(Material::DepthUniformName)->setFloatValue(0.0f);
			imguiShaderUniforms_->commitUniforms();
		}
	}

	ImGui::NewFrame();
}

void ImGuiDrawing::endFrame(RenderQueue &renderQueue)
{
	ImGui::EndFrame();
	ImGui::Render();

	draw(renderQueue);
}

void ImGuiDrawing::endFrame()
{
	ImGui::EndFrame();
	ImGui::Render();

	draw();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

RenderCommand *ImGuiDrawing::retrieveCommandFromPool()
{
	bool commandAdded = false;
	RenderCommand *retrievedCommand = RenderResources::renderCommandPool().retrieveOrAdd(imguiShaderProgram_.get(), commandAdded);
	if (commandAdded)
		setupRenderCmd(*retrievedCommand);

	return retrievedCommand;
}

void ImGuiDrawing::setupRenderCmd(RenderCommand &cmd)
{
	cmd.setType(RenderCommand::CommandTypes::IMGUI);

	Material &material = cmd.material();
	material.setShaderProgram(imguiShaderProgram_.get());
	material.reserveUniformsDataMemory();
	material.uniform(Material::TextureUniformName)->setIntValue(0); // GL_TEXTURE0
	imguiShaderProgram_->attribute(Material::PositionAttributeName)->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, pos)));
	imguiShaderProgram_->attribute(Material::TexCoordsAttributeName)->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, uv)));
	imguiShaderProgram_->attribute(Material::ColorAttributeName)->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, col)));
	imguiShaderProgram_->attribute(Material::ColorAttributeName)->setType(GL_UNSIGNED_BYTE);
	imguiShaderProgram_->attribute(Material::ColorAttributeName)->setNormalized(true);
	material.setBlendingEnabled(true);
	material.setBlendingFactors(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	cmd.geometry().setNumElementsPerVertex(sizeof(ImDrawVert) / sizeof(GLfloat));
	cmd.geometry().setDrawParameters(GL_TRIANGLES, 0, 0);
}

void ImGuiDrawing::draw(RenderQueue &renderQueue)
{
	ImDrawData *drawData = ImGui::GetDrawData();

	const unsigned int numElements = sizeof(ImDrawVert) / sizeof(GLfloat);

	ImGuiIO &io = ImGui::GetIO();
	const int fbWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
	const int fbHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
	if (fbWidth <= 0 || fbHeight <= 0)
		return;
	const ImVec2 clipOff = drawData->DisplayPos;
	const ImVec2 clipScale = drawData->FramebufferScale;

	unsigned int numCmd = 0;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList *imDrawList = drawData->CmdLists[n];

		RenderCommand &firstCmd = *retrieveCommandFromPool();
		if (lastFrameWidth_ != static_cast<int>(io.DisplaySize.x) ||
		    lastFrameHeight_ != static_cast<int>(io.DisplaySize.y))
		{
			firstCmd.material().uniform(Material::GuiProjectionMatrixUniformName)->setFloatVector(projectionMatrix_.data());
			lastFrameWidth_ = static_cast<int>(io.DisplaySize.x);
			lastFrameHeight_ = static_cast<int>(io.DisplaySize.y);
		}

		firstCmd.geometry().shareVbo(nullptr);
		GLfloat *vertices = firstCmd.geometry().acquireVertexPointer(imDrawList->VtxBuffer.Size * numElements, numElements);
		memcpy(vertices, imDrawList->VtxBuffer.Data, imDrawList->VtxBuffer.Size * numElements * sizeof(GLfloat));
		firstCmd.geometry().releaseVertexPointer();

		firstCmd.geometry().shareIbo(nullptr);
		GLushort *indices = firstCmd.geometry().acquireIndexPointer(imDrawList->IdxBuffer.Size);
		memcpy(indices, imDrawList->IdxBuffer.Data, imDrawList->IdxBuffer.Size * sizeof(GLushort));
		firstCmd.geometry().releaseIndexPointer();

		if (lastLayerValue_ != theApplication().guiSettings().imguiLayer)
		{
			// It is enough to set the uniform value once as every ImGui command share the same shader
			const float depth = RenderCommand::calculateDepth(theApplication().guiSettings().imguiLayer, -1.0f, 1.0f);
			firstCmd.material().uniform(Material::DepthUniformName)->setFloatValue(depth);
			lastLayerValue_ = theApplication().guiSettings().imguiLayer;
		}

		for (int cmdIdx = 0; cmdIdx < imDrawList->CmdBuffer.Size; cmdIdx++)
		{
			const ImDrawCmd *imCmd = &imDrawList->CmdBuffer[cmdIdx];
			RenderCommand &currCmd = (cmdIdx == 0) ? firstCmd : *retrieveCommandFromPool();

			// Project scissor/clipping rectangles into framebuffer space
			ImVec2 clipMin((imCmd->ClipRect.x - clipOff.x) * clipScale.x, (imCmd->ClipRect.y - clipOff.y) * clipScale.y);
			ImVec2 clipMax((imCmd->ClipRect.z - clipOff.x) * clipScale.x, (imCmd->ClipRect.w - clipOff.y) * clipScale.y);
			if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
				continue;

			// Apply scissor/clipping rectangle (Y is inverted in OpenGL)
			currCmd.setScissor(static_cast<GLint>(clipMin.x), static_cast<GLint>(static_cast<float>(fbHeight) - clipMax.y),
			                   static_cast<GLsizei>(clipMax.x - clipMin.x), static_cast<GLsizei>(clipMax.y - clipMin.y));

			if (cmdIdx > 0)
			{
				currCmd.geometry().shareVbo(&firstCmd.geometry());
				currCmd.geometry().shareIbo(&firstCmd.geometry());
			}

			currCmd.geometry().setNumIndices(imCmd->ElemCount);
			currCmd.geometry().setFirstIndex(imCmd->IdxOffset);
			currCmd.geometry().setFirstVertex(imCmd->VtxOffset);
			currCmd.setLayer(theApplication().guiSettings().imguiLayer);
			currCmd.setVisitOrder(numCmd);
			currCmd.material().setTexture(reinterpret_cast<GLTexture *>(imCmd->GetTexID()));

			renderQueue.addCommand(&currCmd);
			numCmd++;
		}
	}
}

void ImGuiDrawing::setupBuffersAndShader()
{
	vbo_ = nctl::makeUnique<GLBufferObject>(GL_ARRAY_BUFFER);
	ibo_ = nctl::makeUnique<GLBufferObject>(GL_ELEMENT_ARRAY_BUFFER);

	imguiShaderUniforms_ = nctl::makeUnique<GLShaderUniforms>(imguiShaderProgram_.get());
	imguiShaderUniforms_->setUniformsDataPointer(uniformsBuffer_);
	imguiShaderUniforms_->uniform(Material::TextureUniformName)->setIntValue(0); // GL_TEXTURE0

	imguiShaderProgram_->attribute(Material::PositionAttributeName)->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, pos)));
	imguiShaderProgram_->attribute(Material::TexCoordsAttributeName)->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, uv)));
	imguiShaderProgram_->attribute(Material::ColorAttributeName)->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, col)));
	imguiShaderProgram_->attribute(Material::ColorAttributeName)->setType(GL_UNSIGNED_BYTE);
	imguiShaderProgram_->attribute(Material::ColorAttributeName)->setNormalized(true);
}

void ImGuiDrawing::draw()
{
	ImDrawData *drawData = ImGui::GetDrawData();

	const int fbWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
	const int fbHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
	if (fbWidth <= 0 || fbHeight <= 0)
		return;
	const ImVec2 clipOff = drawData->DisplayPos;
	const ImVec2 clipScale = drawData->FramebufferScale;

	GLBlending::State blendingState = GLBlending::state();
	GLBlending::enable();
	GLBlending::setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLCullFace::State cullFaceState = GLCullFace::state();
	GLCullFace::disable();
	GLDepthTest::State depthTestState = GLDepthTest::state();
	GLDepthTest::disable();

	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList *imDrawList = drawData->CmdLists[n];
		const ImDrawIdx *firstIndex = nullptr;

		// Always define vertex format (and bind VAO) before uploading data to buffers
		imguiShaderProgram_->defineVertexFormat(vbo_.get(), ibo_.get());
		vbo_->bufferData(static_cast<GLsizeiptr>(imDrawList->VtxBuffer.Size) * sizeof(ImDrawVert), static_cast<const GLvoid *>(imDrawList->VtxBuffer.Data), GL_STREAM_DRAW);
		ibo_->bufferData(static_cast<GLsizeiptr>(imDrawList->IdxBuffer.Size) * sizeof(ImDrawIdx), static_cast<const GLvoid *>(imDrawList->IdxBuffer.Data), GL_STREAM_DRAW);
		imguiShaderProgram_->use();

		for (int cmdIdx = 0; cmdIdx < imDrawList->CmdBuffer.Size; cmdIdx++)
		{
			const ImDrawCmd *imCmd = &imDrawList->CmdBuffer[cmdIdx];

			// Project scissor/clipping rectangles into framebuffer space
			ImVec2 clipMin((imCmd->ClipRect.x - clipOff.x) * clipScale.x, (imCmd->ClipRect.y - clipOff.y) * clipScale.y);
			ImVec2 clipMax((imCmd->ClipRect.z - clipOff.x) * clipScale.x, (imCmd->ClipRect.w - clipOff.y) * clipScale.y);
			if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
				continue;

			// Apply scissor/clipping rectangle (Y is inverted in OpenGL)
			GLScissorTest::enable(static_cast<GLint>(clipMin.x), static_cast<GLint>(static_cast<float>(fbHeight) - clipMax.y),
			                      static_cast<GLsizei>(clipMax.x - clipMin.x), static_cast<GLsizei>(clipMax.y - clipMin.y));

			// Bind texture, Draw
			GLTexture::bindHandle(GL_TEXTURE_2D, reinterpret_cast<GLTexture *>(imCmd->GetTexID())->glHandle());
#if (defined(WITH_OPENGLES) && !GL_ES_VERSION_3_2) || defined(__EMSCRIPTEN__)
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(imCmd->ElemCount), sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, firstIndex);
#else
			glDrawElementsBaseVertex(GL_TRIANGLES, static_cast<GLsizei>(imCmd->ElemCount), sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
			                         reinterpret_cast<void *>(static_cast<intptr_t>(imCmd->IdxOffset * sizeof(ImDrawIdx))), static_cast<GLint>(imCmd->VtxOffset));
#endif
			firstIndex += imCmd->ElemCount;
		}
	}

	GLScissorTest::disable();
	GLDepthTest::setState(depthTestState);
	GLCullFace::setState(cullFaceState);
	GLBlending::setState(blendingState);
}

}
