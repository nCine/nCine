#include "imgui.h"

#include "ImGuiDrawing.h"
#include "GLTexture.h"
#include "GLShaderProgram.h"
#include "GLScissorTest.h"
#include "GLBlending.h"
#include "GLDepthTest.h"
#include "GLCullFace.h"
#include "RenderQueue.h"
#include "DrawableNode.h"
#include "Application.h"

#if defined(WITH_GLFW)
	#include "ImGuiGlfwInput.h"
#elif defined(WITH_SDL)
	#include "ImGuiSdlInput.h"
#elif defined(WITH_QT5)
	#include "ImGuiQt5Input.h"
#elif defined(__ANDROID__)
	#include "ImGuiAndroidInput.h"
#endif

#ifdef WITH_EMBEDDED_SHADERS
	#include "shader_strings.h"
#else
	#include "IFile.h" // for dataPath()
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ImGuiDrawing::ImGuiDrawing(bool withSceneGraph)
    : withSceneGraph_(withSceneGraph),
      freeCommandsPool_(16), usedCommandsPool_(16),
      lastFrameWidth_(0), lastFrameHeight_(0)
{
	ImGuiIO &io = ImGui::GetIO();
#ifndef __ANDROID__
	io.BackendRendererName = "nCine_OpenGL";
#else
	io.BackendRendererName = "nCine_OpenGL_ES";
#endif

#if !(defined(__ANDROID__) && !GL_ES_VERSION_3_2) && !defined(WITH_ANGLE) && !defined(__EMSCRIPTEN__)
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
#endif

	const AppConfiguration &appCfg = theApplication().appConfiguration();
	const GLShaderProgram::QueryPhase queryPhase = appCfg.deferShaderQueries ? GLShaderProgram::QueryPhase::DEFERRED : GLShaderProgram::QueryPhase::IMMEDIATE;

	imguiShaderProgram_ = nctl::makeUnique<GLShaderProgram>(queryPhase);
#ifndef WITH_EMBEDDED_SHADERS
	imguiShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/imgui_vs.glsl").data());
	imguiShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/imgui_fs.glsl").data());
#else
	imguiShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::imgui_vs);
	imguiShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::imgui_fs);
#endif
	imguiShaderProgram_->link(GLShaderProgram::Introspection::ENABLED);
	FATAL_ASSERT(imguiShaderProgram_->status() != GLShaderProgram::Status::LINKING_FAILED);

	if (withSceneGraph == false)
		setupBuffersAndShader();

	unsigned char *pixels = nullptr;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
}

ImGuiDrawing::~ImGuiDrawing()
{
	ImGuiIO &io = ImGui::GetIO();
	io.Fonts->TexID = nullptr;
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
		projectionMatrix_ = Matrix4x4f::ortho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, 0.0f, 1.0f);

		if (withSceneGraph_ == false)
		{
			imguiShaderUniforms_->uniform("projection")->setFloatVector(projectionMatrix_.data());
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
	RenderCommand *retrievedCommand = nullptr;

	for (unsigned int i = 0; i < freeCommandsPool_.size(); i++)
	{
		const unsigned int poolSize = freeCommandsPool_.size();
		nctl::UniquePtr<RenderCommand> &command = freeCommandsPool_[i];
		if (command && command->material().shaderProgram() == imguiShaderProgram_.get())
		{
			retrievedCommand = command.get();
			usedCommandsPool_.pushBack(nctl::move(command));
			command = nctl::move(freeCommandsPool_[poolSize - 1]);
			freeCommandsPool_.setSize(poolSize - 1);
			break;
		}
	}

	if (retrievedCommand == nullptr)
	{
		nctl::UniquePtr<RenderCommand> newCommand = nctl::makeUnique<RenderCommand>();
		setupRenderCmd(*newCommand);
		retrievedCommand = newCommand.get();
		usedCommandsPool_.pushBack(nctl::move(newCommand));
	}

	return retrievedCommand;
}

void ImGuiDrawing::resetCommandPool()
{
	for (nctl::UniquePtr<RenderCommand> &command : usedCommandsPool_)
		freeCommandsPool_.pushBack(nctl::move(command));
	usedCommandsPool_.clear();
}

void ImGuiDrawing::setupRenderCmd(RenderCommand &cmd)
{
	cmd.setProfilingType(RenderCommand::CommandTypes::IMGUI);

	Material &material = cmd.material();
	material.setShaderProgram(imguiShaderProgram_.get());
	material.setUniformsDataPointer(nullptr);
	material.uniform("uTexture")->setIntValue(0); // GL_TEXTURE0
	material.attribute("aPosition")->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, pos)));
	material.attribute("aTexCoords")->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, uv)));
	material.attribute("aColor")->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, col)));
	material.attribute("aColor")->setType(GL_UNSIGNED_BYTE);
	material.attribute("aColor")->setNormalized(true);
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
	drawData->ScaleClipRects(drawData->FramebufferScale);

	resetCommandPool();

	unsigned int numCmd = 0;
	const ImVec2 pos = drawData->DisplayPos;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList *imCmdList = drawData->CmdLists[n];

		RenderCommand &firstCmd = *retrieveCommandFromPool();
		if (lastFrameWidth_ != static_cast<int>(io.DisplaySize.x) ||
		    lastFrameHeight_ != static_cast<int>(io.DisplaySize.y))
		{
			firstCmd.material().uniform("projection")->setFloatVector(projectionMatrix_.data());
			lastFrameWidth_ = static_cast<int>(io.DisplaySize.x);
			lastFrameHeight_ = static_cast<int>(io.DisplaySize.y);
		}

		firstCmd.geometry().shareVbo(nullptr);
		GLfloat *vertices = firstCmd.geometry().acquireVertexPointer(imCmdList->VtxBuffer.Size * numElements, numElements);
		memcpy(vertices, imCmdList->VtxBuffer.Data, imCmdList->VtxBuffer.Size * numElements * sizeof(GLfloat));
		firstCmd.geometry().releaseVertexPointer();

		firstCmd.geometry().shareIbo(nullptr);
		GLushort *indices = firstCmd.geometry().acquireIndexPointer(imCmdList->IdxBuffer.Size);
		memcpy(indices, imCmdList->IdxBuffer.Data, imCmdList->IdxBuffer.Size * sizeof(GLushort));
		firstCmd.geometry().releaseIndexPointer();

		for (int cmdIdx = 0; cmdIdx < imCmdList->CmdBuffer.Size; cmdIdx++)
		{
			const ImDrawCmd *imCmd = &imCmdList->CmdBuffer[cmdIdx];
			RenderCommand &currCmd = (cmdIdx == 0) ? firstCmd : *retrieveCommandFromPool();

			const ImVec4 clipRect = ImVec4(imCmd->ClipRect.x - pos.x, imCmd->ClipRect.y - pos.y, imCmd->ClipRect.z - pos.x, imCmd->ClipRect.w - pos.y);
			if (clipRect.x < fbWidth && clipRect.y < fbHeight && clipRect.z >= 0.0f && clipRect.w >= 0.0f)
			{
				currCmd.setScissor(static_cast<GLint>(clipRect.x), static_cast<GLint>(fbHeight - clipRect.w),
				                   static_cast<GLsizei>(clipRect.z - clipRect.x), static_cast<GLsizei>(clipRect.w - clipRect.y));

				if (cmdIdx > 0)
				{
					currCmd.geometry().shareVbo(&firstCmd.geometry());
					currCmd.geometry().shareIbo(&firstCmd.geometry());
				}

				currCmd.geometry().setNumIndices(imCmd->ElemCount);
				currCmd.geometry().setFirstIndex(imCmd->IdxOffset);
				currCmd.geometry().setFirstVertex(imCmd->VtxOffset);
				currCmd.setLayer(DrawableNode::imGuiLayer() + numCmd);
				currCmd.material().setTexture(reinterpret_cast<GLTexture *>(imCmd->TextureId));

				renderQueue.addCommand(&currCmd);
			}
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
	imguiShaderUniforms_->uniform("uTexture")->setIntValue(0); // GL_TEXTURE0

	imguiShaderAttributes_ = nctl::makeUnique<GLShaderAttributes>(imguiShaderProgram_.get());
	imguiShaderAttributes_->attribute("aPosition")->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, pos)));
	imguiShaderAttributes_->attribute("aTexCoords")->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, uv)));
	imguiShaderAttributes_->attribute("aColor")->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, col)));
	imguiShaderAttributes_->attribute("aColor")->setType(GL_UNSIGNED_BYTE);
	imguiShaderAttributes_->attribute("aColor")->setNormalized(true);
}

void ImGuiDrawing::draw()
{
	ImDrawData *drawData = ImGui::GetDrawData();

	const int fbWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
	const int fbHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
	if (fbWidth <= 0 || fbHeight <= 0)
		return;
	drawData->ScaleClipRects(drawData->FramebufferScale);

	GLBlending::pushState();
	GLBlending::enable();
	GLBlending::blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLCullFace::pushState();
	GLCullFace::disable();
	GLDepthTest::pushState();
	GLDepthTest::disable();

	const ImVec2 pos = drawData->DisplayPos;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList *imCmdList = drawData->CmdLists[n];
		const ImDrawIdx *firstIndex = nullptr;

		vbo_->bufferData(static_cast<GLsizeiptr>(imCmdList->VtxBuffer.Size) * sizeof(ImDrawVert), static_cast<const GLvoid *>(imCmdList->VtxBuffer.Data), GL_STREAM_DRAW);
		ibo_->bufferData(static_cast<GLsizeiptr>(imCmdList->IdxBuffer.Size) * sizeof(ImDrawIdx), static_cast<const GLvoid *>(imCmdList->IdxBuffer.Data), GL_STREAM_DRAW);
		imguiShaderProgram_->use();
		imguiShaderAttributes_->defineVertexFormat(vbo_.get(), ibo_.get());

		for (int cmdIdx = 0; cmdIdx < imCmdList->CmdBuffer.Size; cmdIdx++)
		{
			const ImDrawCmd *imCmd = &imCmdList->CmdBuffer[cmdIdx];

			const ImVec4 clipRect = ImVec4(imCmd->ClipRect.x - pos.x, imCmd->ClipRect.y - pos.y, imCmd->ClipRect.z - pos.x, imCmd->ClipRect.w - pos.y);
			if (clipRect.x < fbWidth && clipRect.y < fbHeight && clipRect.z >= 0.0f && clipRect.w >= 0.0f)
			{
				GLScissorTest::enable(static_cast<GLint>(clipRect.x), static_cast<GLint>(fbHeight - clipRect.w),
				                      static_cast<GLsizei>(clipRect.z - clipRect.x), static_cast<GLsizei>(clipRect.w - clipRect.y));
				GLTexture::bindHandle(GL_TEXTURE_2D, reinterpret_cast<GLTexture *>(imCmd->TextureId)->glHandle());
#if (defined(__ANDROID__) && !GL_ES_VERSION_3_2) || defined(WITH_ANGLE) || defined(__EMSCRIPTEN__)
				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(imCmd->ElemCount), sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, firstIndex);
#else
				glDrawElementsBaseVertex(GL_TRIANGLES, static_cast<GLsizei>(imCmd->ElemCount), sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
				                         reinterpret_cast<void *>(static_cast<intptr_t>(imCmd->IdxOffset * sizeof(ImDrawIdx))), static_cast<GLint>(imCmd->VtxOffset));
#endif
			}
			firstIndex += imCmd->ElemCount;
		}
	}

	GLScissorTest::disable();
	GLDepthTest::popState();
	GLCullFace::popState();
	GLBlending::popState();
}

}
