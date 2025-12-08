#include "imgui.h"

#include "ImGuiDrawing.h"
#include "GLTexture.h"
#include "GLShaderProgram.h"
#include "GLScissorTest.h"
#include "GLBlending.h"
#include "GLDepthTest.h"
#include "GLCullFace.h"
#include "RenderResources.h"
#include "Application.h"

#if defined(WITH_SCENEGRAPH)
	#include "RenderQueue.h"
	#include "RenderCommandPool.h"
#else
	#include "GLBufferObject.h"
#endif

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

namespace {
	namespace UniformNames {
		const char *GuiProjectionMatrix = "uGuiProjection";
		const char *Depth = "uDepth";
		const char *Texture = "uTexture";
	}

	namespace AttributeNames {
		const char *Position = "aPosition";
		const char *TexCoords = "aTexCoords";
		const char *Color = "aColor";
	}
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ImGuiDrawing::ImGuiDrawing(bool withSceneGraph)
    : withSceneGraph_(withSceneGraph), textures_(32),
      lastFrameWidth_(0), lastFrameHeight_(0), lastLayerValue_(0)
{
	ImGuiIO &io = ImGui::GetIO();
#if defined(WITH_OPENGLES) || defined(__EMSCRIPTEN__)
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
	io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures; // We can honor ImGuiPlatformIO::Textures[] requests during render.

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
}

ImGuiDrawing::~ImGuiDrawing()
{
	// Destroy all textures
	for (ImTextureData *tex : ImGui::GetPlatformIO().Textures)
		if (tex->RefCount == 1)
			destroyTexture(tex);

	ImGuiIO &io = ImGui::GetIO();
	io.BackendRendererName = nullptr;
	io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasTextures);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

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
			imguiShaderUniforms_->uniform(UniformNames::GuiProjectionMatrix)->setFloatVector(projectionMatrix_.data());
			imguiShaderUniforms_->uniform(UniformNames::Depth)->setFloatValue(0.0f);
			imguiShaderUniforms_->commitUniforms();
		}
	}

	ImGui::NewFrame();
}

#ifdef WITH_SCENEGRAPH
void ImGuiDrawing::endFrame(RenderQueue &renderQueue)
{
	ImGui::EndFrame();
	ImGui::Render();

	draw(renderQueue);
}
#endif

void ImGuiDrawing::endFrame()
{
	ImGui::EndFrame();
	ImGui::Render();

	draw();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ImGuiDrawing::destroyTexture(ImTextureData *tex)
{
	GLTexture *texturePtr = (GLTexture *)(intptr_t)tex->TexID;
	textures_.remove(texturePtr);

	// Clear identifiers and mark as destroyed (in order to allow e.g. calling InvalidateDeviceObjects while running)
	tex->SetTexID(ImTextureID_Invalid);
	tex->SetStatus(ImTextureStatus_Destroyed);
}

void ImGuiDrawing::updateTexture(ImTextureData *tex)
{
	// FIXME: Consider backing up and restoring
	if (tex->Status == ImTextureStatus_WantCreate || tex->Status == ImTextureStatus_WantUpdates)
	{
#ifdef GL_UNPACK_ROW_LENGTH // Not on WebGL/ES
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
#ifdef GL_UNPACK_ALIGNMENT
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#endif
	}

	if (tex->Status == ImTextureStatus_WantCreate)
	{
		// Create and upload new texture to graphics system
		//IMGUI_DEBUG_LOG("UpdateTexture #%03d: WantCreate %dx%d\n", tex->UniqueID, tex->Width, tex->Height);
		IM_ASSERT(tex->TexID == 0 && tex->BackendUserData == nullptr);
		IM_ASSERT(tex->Format == ImTextureFormat_RGBA32);
		const void *pixels = tex->GetPixels();

		// Upload texture to graphics system
		// (Bilinear sampling is required by default. Set 'io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines' or 'style.AntiAliasedLinesUseTex = false' to allow point/nearest sampling)
		GLint lastTexture = GLTexture::boundHandle(GL_TEXTURE_2D);
		nctl::UniquePtr<GLTexture> texture = nctl::makeUnique<GLTexture>(GL_TEXTURE_2D);
		texture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		texture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		texture->texParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		texture->texParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		texture->texImage2D(0, GL_RGBA, tex->Width, tex->Height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		if (textures_.loadFactor() >= 0.8f)
			textures_.rehash(textures_.capacity() * 2);
		GLTexture *texturePtr = texture.get();
		textures_.insert(texturePtr, nctl::move(texture));
		// Store identifiers
		tex->SetTexID((ImTextureID)(intptr_t)texturePtr);
		tex->SetStatus(ImTextureStatus_OK);

		// Restore state
		GLTexture::bindHandle(GL_TEXTURE_2D, lastTexture);
	}
	else if (tex->Status == ImTextureStatus_WantUpdates)
	{
		// Update selected blocks. We only ever write to textures regions which have never been used before!
		// This backend choose to use tex->Updates[] but you can use tex->UpdateRect to upload a single region.
		GLint lastTexture = GLTexture::boundHandle(GL_TEXTURE_2D);

		GLTexture *texturePtr = (GLTexture *)(intptr_t)tex->TexID;
		texturePtr->bind();

#ifdef GL_UNPACK_ROW_LENGTH // Not on WebGL/ES
		glPixelStorei(GL_UNPACK_ROW_LENGTH, tex->Width);
		for (ImTextureRect &r : tex->Updates)
			texturePtr->texSubImage2D(0, r.x, r.y, r.w, r.h, GL_RGBA, GL_UNSIGNED_BYTE, tex->GetPixelsAt(r.x, r.y));
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#else
		// GL ES doesn't have GL_UNPACK_ROW_LENGTH, so we need to (A) copy to a contiguous buffer or (B) upload line by line.
		for (ImTextureRect &r : tex->Updates)
		{
			const int srcPitch = r.w * tex->BytesPerPixel;
			if (tempTexBuffer_.size() < r.h * srcPitch)
				tempTexBuffer_.setSize(r.h * srcPitch);
			char *outP = tempTexBuffer_.data();
			for (int y = 0; y < r.h; y++, outP += srcPitch)
				memcpy(outP, tex->GetPixelsAt(r.x, r.y + y), srcPitch);
			texturePtr->texSubImage2D(0, r.x, r.y, r.w, r.h, GL_RGBA, GL_UNSIGNED_BYTE, tempTexBuffer_.data());
		}
#endif
		tex->SetStatus(ImTextureStatus_OK);
		GLTexture::bindHandle(GL_TEXTURE_2D, lastTexture); // Restore state
	}
	else if (tex->Status == ImTextureStatus_WantDestroy && tex->UnusedFrames > 0)
		destroyTexture(tex);
}

#ifdef WITH_SCENEGRAPH
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
	material.uniform(UniformNames::Texture)->setIntValue(0); // GL_TEXTURE0
	imguiShaderProgram_->attribute(AttributeNames::Position)->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, pos)));
	imguiShaderProgram_->attribute(AttributeNames::TexCoords)->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, uv)));
	imguiShaderProgram_->attribute(AttributeNames::Color)->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, col)));
	imguiShaderProgram_->attribute(AttributeNames::Color)->setType(GL_UNSIGNED_BYTE);
	imguiShaderProgram_->attribute(AttributeNames::Color)->setNormalized(true);
	material.setBlendingEnabled(true);
	material.setBlendingFactors(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	cmd.geometry().setNumElementsPerVertex(sizeof(ImDrawVert) / sizeof(GLfloat));
	cmd.geometry().setDrawParameters(GL_TRIANGLES, 0, 0);
}

void ImGuiDrawing::draw(RenderQueue &renderQueue)
{
	ImDrawData *drawData = ImGui::GetDrawData();

	const unsigned int numElements = sizeof(ImDrawVert) / sizeof(GLfloat);

	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO &io = ImGui::GetIO();
	const int fbWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
	const int fbHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
	if (fbWidth <= 0 || fbHeight <= 0)
		return;

	// Will project scissor/clipping rectangles into framebuffer space
	const ImVec2 clipOff = drawData->DisplayPos;
	const ImVec2 clipScale = drawData->FramebufferScale;

	// Catch up with texture updates. Most of the times, the list will have 1 element with an OK status, aka nothing to do.
	// (This almost always points to ImGui::GetPlatformIO().Textures[] but is part of ImDrawData to allow overriding or disabling texture updates).
	if (drawData->Textures != nullptr)
		for (ImTextureData *tex : *drawData->Textures)
			if (tex->Status != ImTextureStatus_OK)
				updateTexture(tex);

	unsigned int numCmd = 0;
	for (const ImDrawList *imDrawList : drawData->CmdLists)
	{
		RenderCommand &firstCmd = *retrieveCommandFromPool();
		if (lastFrameWidth_ != static_cast<int>(io.DisplaySize.x) ||
		    lastFrameHeight_ != static_cast<int>(io.DisplaySize.y))
		{
			firstCmd.material().uniform(UniformNames::GuiProjectionMatrix)->setFloatVector(projectionMatrix_.data());
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
			firstCmd.material().uniform(UniformNames::Depth)->setFloatValue(depth);
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
#endif

void ImGuiDrawing::setupBuffersAndShader()
{
	vbo_ = nctl::makeUnique<GLBufferObject>(GL_ARRAY_BUFFER);
	ibo_ = nctl::makeUnique<GLBufferObject>(GL_ELEMENT_ARRAY_BUFFER);

	imguiShaderUniforms_ = nctl::makeUnique<GLShaderUniforms>(imguiShaderProgram_.get());
	imguiShaderUniforms_->setUniformsDataPointer(uniformsBuffer_);
	imguiShaderUniforms_->uniform(UniformNames::Texture)->setIntValue(0); // GL_TEXTURE0

	imguiShaderProgram_->attribute(AttributeNames::Position)->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, pos)));
	imguiShaderProgram_->attribute(AttributeNames::TexCoords)->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, uv)));
	imguiShaderProgram_->attribute(AttributeNames::Color)->setVboParameters(sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, col)));
	imguiShaderProgram_->attribute(AttributeNames::Color)->setType(GL_UNSIGNED_BYTE);
	imguiShaderProgram_->attribute(AttributeNames::Color)->setNormalized(true);
}

void ImGuiDrawing::draw()
{
	ImDrawData *drawData = ImGui::GetDrawData();

	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	const int fbWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
	const int fbHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
	if (fbWidth <= 0 || fbHeight <= 0)
		return;

	// Will project scissor/clipping rectangles into framebuffer space
	const ImVec2 clipOff = drawData->DisplayPos;
	const ImVec2 clipScale = drawData->FramebufferScale;

	// Catch up with texture updates. Most of the times, the list will have 1 element with an OK status, aka nothing to do.
	// (This almost always points to ImGui::GetPlatformIO().Textures[] but is part of ImDrawData to allow overriding or disabling texture updates).
	if (drawData->Textures != nullptr)
		for (ImTextureData *tex : *drawData->Textures)
			if (tex->Status != ImTextureStatus_OK)
				updateTexture(tex);

	GLBlending::State blendingState = GLBlending::state();
	GLBlending::enable();
	GLBlending::setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLCullFace::State cullFaceState = GLCullFace::state();
	GLCullFace::disable();
	GLDepthTest::State depthTestState = GLDepthTest::state();
	GLDepthTest::disable();

	for (const ImDrawList *imDrawList : drawData->CmdLists)
	{
#if (defined(WITH_OPENGLES) && !GL_ES_VERSION_3_2) || defined(__EMSCRIPTEN__)
		const ImDrawIdx *firstIndex = nullptr;
#endif

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
			firstIndex += imCmd->ElemCount;
#else
			glDrawElementsBaseVertex(GL_TRIANGLES, static_cast<GLsizei>(imCmd->ElemCount), sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
			                         reinterpret_cast<void *>(static_cast<intptr_t>(imCmd->IdxOffset * sizeof(ImDrawIdx))), static_cast<GLint>(imCmd->VtxOffset));
#endif
		}
	}

	GLScissorTest::disable();
	GLDepthTest::setState(depthTestState);
	GLCullFace::setState(cullFaceState);
	GLBlending::setState(blendingState);
}

}
