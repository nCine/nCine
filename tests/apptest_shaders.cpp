#include <ncine/config.h>

#if NCINE_WITH_IMGUI
	#include <ncine/imgui.h>
#endif

#include "apptest_shaders.h"
#include "apptest_shaders_sources.h"
#include <ncine/Application.h>
#include <ncine/Viewport.h>
#include <ncine/Shader.h>
#include <ncine/ShaderState.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/MeshSprite.h>
#include <ncine/TextNode.h>
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const char *MegaTextureFile = "megatexture_256_ETC2.ktx";
const char *Texture1File = "texture1_ETC2.ktx";
const char *Texture2File = "texture2_ETC2.ktx";
const char *Texture3File = "texture3_ETC2.ktx";
const char *Texture4File = "texture4_ETC2.ktx";
const char *DiffuseMapFile = "diffuse.webp";
const char *NormalMapFile = "normal.webp";
const char *FontTextureFile = "DroidSans32_256_ETC2.ktx";
#else
const char *MegaTextureFile = "megatexture_256.png";
const char *Texture1File = "texture1.png";
const char *Texture2File = "texture2.png";
const char *Texture3File = "texture3.png";
const char *Texture4File = "texture4.png";
const char *DiffuseMapFile = "diffuse.png";
const char *NormalMapFile = "normal.png";
const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

const int NumTexelPoints[MyEventHandler::NumTextures] = { 3, 4, 5, 4 };
const nc::Vector2f TexelPoints[] = {
	// clang-format off
	{4.0f, 2.0f}, {124.0f, 2.0f}, {64.0f, 122.0f},
	{13.0f, 13.0f}, {115.0f, 13.0f}, {13.0f, 115.0f}, {115.0f, 115.0f},
	{3.0f, 79.0f}, {26.0f, 2.0f}, {64.0f, 125.0f}, {102.0f, 2.0f}, {125.0f, 79.0f},
	{20.0f, 20.0f}, {107.0f, 20.0f}, {20.0f, 107.0f}, {107.0f, 107.0f},
	// clang-format on
};

struct InvertedVertexTextureZ
{
	float u, v;
	float x, y, z;
};

struct MultiTextureShaderData
{
	nc::Vector4f lightPosition = nc::Vector4f(0.0f, 0.0f, 50.0f, 0.0f);
	nc::Colorf ambientColor = nc::Colorf(1.0f, 1.0f, 1.0f, 0.05f);
	nc::Colorf diffuseColor = nc::Colorf(1.0f, 1.0f, 1.0f, 1.0f);
	nc::Colorf specularColor = nc::Colorf(1.0f, 1.0f, 1.0f, 1.0f);
	nc::Vector4f attFactors = nc::Vector4f(1.0f, 0.00003f, 0.000003f, 32.0f);
} multiTextureShaderData;
bool shaderDataChanged = true;
bool uniformBlockHasStructSize = false;

enum ViewportSetup
{
	NONE = 0,
	BLUR,
	BLOOM
};
const char *ViewportSetupLabels[] = { "None", "Blur", "Bloom" };

int downsampleFactor = 4;
bool captureMouse = true;
bool captureKeyboard = true;

#if NCINE_WITH_IMGUI
bool showImGui = true;
#endif

const char *stringOnOff(bool enabled)
{
	return enabled ? "on" : "off";
}

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
	//config.resizable = true;
}

void MyEventHandler::onInit()
{
	const float width = nc::theApplication().width();
	const float height = nc::theApplication().height();

	megaTexture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", MegaTextureFile)).data());
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture1File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture2File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture3File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture4File)).data()));

	texture0_ = nctl::makeUnique<nc::Texture>("Ping texture", nc::Texture::Format::RGB8, nc::theApplication().resolutionInt());
	texture1_ = nctl::makeUnique<nc::Texture>("Pong texture", nc::Texture::Format::RGB8, nc::theApplication().resolutionInt());
	downsampleTexture0_ = nctl::makeUnique<nc::Texture>("Downsample ping texture", nc::Texture::Format::RGB8, nc::theApplication().resolutionInt() / downsampleFactor);
	downsampleTexture1_ = nctl::makeUnique<nc::Texture>("Downsample pong texture", nc::Texture::Format::RGB8, nc::theApplication().resolutionInt() / downsampleFactor);
	bloomTexture_ = nctl::makeUnique<nc::Texture>("Bloom texture", nc::Texture::Format::RGB8, nc::theApplication().resolutionInt());

	diffuseTexture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", DiffuseMapFile)).data());
	normalTexture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", NormalMapFile)).data());

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());

	rootNode_ = nctl::makeUnique<nc::SceneNode>();
	rootNodeMrt_ = nctl::makeUnique<nc::SceneNode>();
	sceneViewport_ = nctl::makeUnique<nc::Viewport>(texture0_.get());
	sceneViewportMrt_ = nctl::makeUnique<nc::Viewport>(texture0_.get());
	sceneViewportMrt_->setTexture(1, bloomTexture_.get());
	pingViewport_ = nctl::makeUnique<nc::Viewport>(texture1_.get());
	pongViewport_ = nctl::makeUnique<nc::Viewport>(texture0_.get());
	downsampleViewport_ = nctl::makeUnique<nc::Viewport>(downsampleTexture0_.get());
	upsampleViewport_ = nctl::makeUnique<nc::Viewport>(bloomTexture_.get());

	vpBlurShader_ = nctl::makeUnique<nc::Shader>("SeparableBlur_Shader", nc::Shader::LoadMode::STRING, nc::Shader::DefaultVertex::SPRITE, sprite_blur_fs);
	FATAL_ASSERT(vpBlurShader_->isLinked());
	vpPingSprite_ = nctl::makeUnique<nc::Sprite>(nullptr, texture0_.get(), nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	vpPingSpriteShaderState_ = nctl::makeUnique<nc::ShaderState>(vpPingSprite_.get(), vpBlurShader_.get());
	vpPingSpriteShaderState_->setUniformFloat(nullptr, "uResolution", static_cast<float>(texture0_->width()), static_cast<float>(texture0_->height()));

	vpPongSprite_ = nctl::makeUnique<nc::Sprite>(nullptr, texture1_.get(), nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	vpPongSpriteShaderState_ = nctl::makeUnique<nc::ShaderState>(vpPongSprite_.get(), vpBlurShader_.get());
	vpPongSpriteShaderState_->setUniformFloat(nullptr, "uResolution", static_cast<float>(texture1_->width()), static_cast<float>(texture1_->height()));

	sceneSprite_ = nctl::makeUnique<nc::Sprite>(nullptr, texture1_.get(), nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	vpDownsampleSprite_ = nctl::makeUnique<nc::Sprite>(nullptr, bloomTexture_.get(), nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	vpUpsampleSprite_ = nctl::makeUnique<nc::Sprite>(nullptr, downsampleTexture0_.get(), nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	vpUpsampleSprite_->setScale(static_cast<float>(downsampleFactor));

	sceneViewport_->setRootNode(rootNode_.get());
	sceneViewportMrt_->setRootNode(rootNodeMrt_.get());
	pingViewport_->setRootNode(vpPingSprite_.get());
	pongViewport_->setRootNode(vpPongSprite_.get());
	downsampleViewport_->setRootNode(vpDownsampleSprite_.get());
	upsampleViewport_->setRootNode(vpUpsampleSprite_.get());

	blendingViewport_ = nctl::makeUnique<nc::Viewport>(texture1_.get());
	vpBlendingShader_ = nctl::makeUnique<nc::Shader>("Blending_Shader", nc::Shader::LoadMode::STRING, nc::Shader::DefaultVertex::SPRITE, blending_fs);
	FATAL_ASSERT(vpBlendingShader_->isLinked());
	vpBlendingSprite_ = nctl::makeUnique<nc::Sprite>(nullptr, texture0_.get(), nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	vpBlendingShaderState_ = nctl::makeUnique<nc::ShaderState>(vpBlendingSprite_.get(), vpBlendingShader_.get());
	vpBlendingShaderState_->setTexture(0, texture0_.get()); // GL_TEXTURE0
	vpBlendingShaderState_->setUniformInt(nullptr, "uTexture0", 0); // GL_TEXTURE0
	vpBlendingShaderState_->setTexture(1, bloomTexture_.get()); // GL_TEXTURE1
	vpBlendingShaderState_->setUniformInt(nullptr, "uTexture1", 1); // GL_TEXTURE1
	blendingViewport_->setRootNode(vpBlendingSprite_.get());

	debugString_ = nctl::makeUnique<nctl::String>(128);
	debugText_ = nctl::makeUnique<nc::TextNode>(rootNode_.get(), font_.get());
	debugText_->setLayer(1);
	debugText_->setPosition((nc::theApplication().width() - debugText_->width()) * 0.5f,
	                        nc::theApplication().height() - debugText_->lineHeight() * 0.5f * 2.0f);
	debugText_->setColor(255, 255, 0, 255);
	debugText_->setAlignment(nc::TextNode::Alignment::CENTER);

	spriteShader_ = nctl::makeUnique<nc::Shader>("Sprite_Shader", nc::Shader::LoadMode::STRING, sprite_vs, sprite_fs);
	FATAL_ASSERT(spriteShader_->isLinked());
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(rootNode_.get(), textures_[i % NumTextures].get(), width * 0.15f + width * 0.1f * i, height * 0.5f));
		sprites_.back()->setScale(0.5f);
		spriteShaderStates_.pushBack(nctl::makeUnique<nc::ShaderState>(sprites_.back().get(), spriteShader_.get()));
	}
	batchedSpriteShader_ = nctl::makeUnique<nc::Shader>("Batched_Sprite_Shader", nc::Shader::LoadMode::STRING, nc::Shader::Introspection::NO_UNIFORMS_IN_BLOCKS, batched_sprite_vs, sprite_fs);
	FATAL_ASSERT(batchedSpriteShader_->isLinked());
	spriteShader_->registerBatchedShader(*batchedSpriteShader_);

	meshShader_ = nctl::makeUnique<nc::Shader>("MeshSprite_Shader", nc::Shader::LoadMode::STRING, meshsprite_vs, meshsprite_fs);
	FATAL_ASSERT(meshShader_->isLinked());
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		meshSprites_.pushBack(nctl::makeUnique<nc::MeshSprite>(rootNode_.get(), textures_[i % NumTextures].get(), width * 0.15f + width * 0.1f * i, height * 0.75f));
		meshSprites_.back()->setScale(0.5f);
		meshSpriteShaderStates_.pushBack(nctl::makeUnique<nc::ShaderState>(meshSprites_.back().get(), meshShader_.get()));
	}
	setupMeshVertices(false);

	batchedMeshShader_ = nctl::makeUnique<nc::Shader>("Batched_MeshSprite_Shader", nc::Shader::LoadMode::STRING, nc::Shader::Introspection::NO_UNIFORMS_IN_BLOCKS, batched_meshsprite_vs, meshsprite_fs);
	FATAL_ASSERT(batchedMeshShader_->isLinked());
	meshShader_->registerBatchedShader(*batchedMeshShader_);

	meshShader_->setAttribute("aPosition", sizeof(InvertedVertexTextureZ), 8);
	meshShader_->setAttribute("aDepth", sizeof(InvertedVertexTextureZ), 16);
	meshShader_->setAttribute("aTexCoords", sizeof(InvertedVertexTextureZ), 0);

	batchedMeshShader_->setAttribute("aPosition", sizeof(InvertedVertexTextureZ) + 4, 8);
	batchedMeshShader_->setAttribute("aDepth", sizeof(InvertedVertexTextureZ) + 4, 16);
	batchedMeshShader_->setAttribute("aTexCoords", sizeof(InvertedVertexTextureZ) + 4, 0);
	batchedMeshShader_->setAttribute("aMeshIndex", sizeof(InvertedVertexTextureZ) + 4, 20);

	multitextureShader_ = nctl::makeUnique<nc::Shader>("Multitexture_Shader", nc::Shader::LoadMode::STRING, multitexture_vs, multitexture_fs);
	FATAL_ASSERT(multitextureShader_->isLinked());
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		multitextureSprites_.pushBack(nctl::makeUnique<nc::Sprite>(rootNode_.get(), diffuseTexture_.get(), width * 0.15f + width * 0.1f * i, height * 0.25f));
		multitextureSprites_.back()->setScale(0.5f);
		multitextureShaderStates_.pushBack(nctl::makeUnique<nc::ShaderState>(multitextureSprites_.back().get(), multitextureShader_.get()));

		multitextureShaderStates_.back()->setTexture(0, diffuseTexture_.get()); // GL_TEXTURE0
		multitextureShaderStates_.back()->setUniformInt(nullptr, "uTexture0", 0); // GL_TEXTURE0
		multitextureShaderStates_.back()->setTexture(1, normalTexture_.get()); // GL_TEXTURE1
		multitextureShaderStates_.back()->setUniformInt(nullptr, "uTexture1", 1); // GL_TEXTURE1
	}
	batchedMultitextureShader_ = nctl::makeUnique<nc::Shader>("Batched_Multitexture_Shader", nc::Shader::LoadMode::STRING, nc::Shader::Introspection::NO_UNIFORMS_IN_BLOCKS, batched_multitexture_vs, multitexture_fs);
	FATAL_ASSERT(batchedMultitextureShader_->isLinked());
	multitextureShader_->registerBatchedShader(*batchedMultitextureShader_);

	// Shader permutation for Multiple Render Targets (MRTs)
	nctl::String multitexture_fs_mrt(nctl::strnlen(multitexture_fs, 1024) + 32);
	multitexture_fs_mrt.append("#define WITH_MRT\n#line 0\n");
	multitexture_fs_mrt.formatAppend("%s", multitexture_fs);
	multitextureMrtShader_ = nctl::makeUnique<nc::Shader>("Multitexture_MRT_Shader", nc::Shader::LoadMode::STRING, multitexture_vs, multitexture_fs_mrt.data());
	FATAL_ASSERT(multitextureMrtShader_->isLinked());
	batchedMultitextureMrtShader_ = nctl::makeUnique<nc::Shader>("Batched_Multitexture_MRT_Shader", nc::Shader::LoadMode::STRING,
	                                                             nc::Shader::Introspection::NO_UNIFORMS_IN_BLOCKS, batched_multitexture_vs, multitexture_fs_mrt.data());
	FATAL_ASSERT(batchedMultitextureMrtShader_->isLinked());
	multitextureMrtShader_->registerBatchedShader(*batchedMultitextureMrtShader_);

	uniformBlockHasStructSize = (multitextureShaderStates_[0]->uniformBlockSize("DataBlock") == sizeof(MultiTextureShaderData));
	multiTextureShaderData.lightPosition.x = nc::theApplication().width() * 0.5f;
	multiTextureShaderData.lightPosition.y = nc::theApplication().height() * 0.5f;

	withAtlas_ = false;
	withAtlas_ ? setupAtlas() : setupTextures();
	setupViewport();
	pause_ = false;
	angle_ = 0.0f;
	numBlurPasses_ = 2;
	currentViewportSetup_ = ViewportSetup::NONE;
}

void MyEventHandler::onFrameStart()
{
	const float width = nc::theApplication().width();
	const float height = nc::theApplication().height();

#if NCINE_WITH_IMGUI
	captureMouse = (ImGui::GetIO().WantCaptureMouse == false);
	captureKeyboard = (ImGui::GetIO().WantCaptureKeyboard == false);
	if (showImGui)
	{
		ImGui::SetNextWindowSize(ImVec2(410.0f, 320.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(30.0f, 60.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("apptest_shaders", &showImGui))
		{
			shaderDataChanged |= ImGui::InputFloat3("Position", multiTextureShaderData.lightPosition.data());
			shaderDataChanged |= ImGui::ColorEdit3("Ambient Color", multiTextureShaderData.ambientColor.data());
			shaderDataChanged |= ImGui::ColorEdit3("Diffuse Color", multiTextureShaderData.diffuseColor.data());
			shaderDataChanged |= ImGui::ColorEdit3("Specular Color", multiTextureShaderData.specularColor.data());
			shaderDataChanged |= ImGui::InputFloat3("Attenuation", multiTextureShaderData.attFactors.data(), "%.6f");

			shaderDataChanged |= ImGui::SliderFloat("Ambient Intensity", &multiTextureShaderData.ambientColor.data()[3], 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			shaderDataChanged |= ImGui::SliderFloat("Diffuse Intensity", &multiTextureShaderData.diffuseColor.data()[3], 0.0f, 2.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			shaderDataChanged |= ImGui::SliderFloat("Specular Intensity", &multiTextureShaderData.specularColor.data()[3], 0.0f, 2.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			shaderDataChanged |= ImGui::SliderFloat("Specular Scatter", &multiTextureShaderData.attFactors.w, 1.0f, 64.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

			if (ImGui::Button("Reset"))
			{
				multiTextureShaderData = MultiTextureShaderData();
				multiTextureShaderData.lightPosition.x = width * 0.5f;
				multiTextureShaderData.lightPosition.y = height * 0.5f;
				shaderDataChanged = true;
			}

			ImGui::Separator();
			if (ImGui::SliderInt("Blur passes", &numBlurPasses_, 1, 3, "%d", ImGuiSliderFlags_AlwaysClamp))
				setupViewport();
			if (ImGui::Combo("Viewport setup", &currentViewportSetup_, ViewportSetupLabels, IM_ARRAYSIZE(ViewportSetupLabels)))
				setupViewport();
		}
		ImGui::End();
	}
#endif

	if (joyVectorLeft_.length() > nc::IInputManager::LeftStickDeadZone)
	{
		multiTextureShaderData.lightPosition.x = width * joyVectorLeft_.x;
		multiTextureShaderData.lightPosition.y = height * joyVectorLeft_.y;
		shaderDataChanged = true;
	}

	if (shaderDataChanged)
	{
		for (unsigned int i = 0; i < NumSprites; i++)
		{
			if (uniformBlockHasStructSize)
				multitextureShaderStates_[i]->copyToUniformBlock("DataBlock", reinterpret_cast<unsigned char *>(&multiTextureShaderData));
			else
			{
				multitextureShaderStates_[i]->setUniformFloat("DataBlock", "lightPos", multiTextureShaderData.lightPosition.data());
				multitextureShaderStates_[i]->setUniformFloat("DataBlock", "ambientColor", multiTextureShaderData.ambientColor.data());
				multitextureShaderStates_[i]->setUniformFloat("DataBlock", "diffuseColor", multiTextureShaderData.diffuseColor.data());
				multitextureShaderStates_[i]->setUniformFloat("DataBlock", "specularColor", multiTextureShaderData.specularColor.data());
				multitextureShaderStates_[i]->setUniformFloat("DataBlock", "attFactors", multiTextureShaderData.attFactors.data());
			}
		}
		shaderDataChanged = false;
	}

	const nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
	debugString_->clear();
	debugString_->format("batching: %s, culling: %s, texture atlas: %s, viewport: %s", stringOnOff(settings.batchingEnabled),
	                     stringOnOff(settings.cullingEnabled), stringOnOff(withAtlas_), stringOnOff(currentViewportSetup_ != ViewportSetup::NONE));
	debugText_->setString(*debugString_);

	if (pause_ == false)
	{
		angle_ += 1.0f * nc::theApplication().interval();
		if (angle_ > 360.0f)
			angle_ -= 360.0f;
	}

	for (unsigned int i = 0; i < NumSprites; i++)
	{
		sprites_[i]->setPositionY(height * 0.3f + fabsf(sinf(angle_ + 5.0f * i)) * (height * (0.25f + 0.02f * i)));
		sprites_[i]->setRotation(angle_ * 20.0f);
		meshSprites_[i]->setRotation(angle_ * 20.0f);
		multitextureSprites_[i]->setPositionY(height * 0.2f + fabsf(sinf(angle_ + 5.0f * i)) * (height * (0.2f + 0.02f * i)));
		multitextureSprites_[i]->setRotation(angle_ * 20.0f);
	}
}

void MyEventHandler::onPostUpdate()
{
	// In the `onPostUpdate()` callback it is possible to query the culling
	// state of a node for the current frame, before drawing it.
	if (pause_ == false)
	{
		const unsigned long int numFrames = nc::theApplication().numFrames();
		// Dirtying a uniform cache of a node that is not going to be rendered would not call OpenGL uniform functions.
		// Nevertheless, it is still possible to avoid touching the cache altogether if a node is not going to be rendered.
		for (unsigned int i = 0; i < NumSprites; i++)
		{
			const nc::DrawableNode *sprite = spriteShaderStates_[i]->node();
			const bool spriteIsCulled = (sprite && sprite->lastFrameRendered() < numFrames);
			if (sprite && sprite->isDrawEnabled() && spriteIsCulled == false)
				spriteShaderStates_[i]->setUniformFloat("InstanceBlock", "angle", angle_);

			const nc::DrawableNode *meshSprite = meshSpriteShaderStates_[i]->node();
			const bool meshSpriteIsCulled = (meshSprite && meshSprite->lastFrameRendered() < numFrames);
			if (meshSprite && meshSprite->isDrawEnabled() && meshSpriteIsCulled == false)
			{
				const float angle = angle_ + 15.0f * static_cast<float>(i);
				const float sin2First = sinf(angle) * sinf(angle);
				const float sin2Second = sinf(angle + 90.0f) * sinf(angle + 90.0f);
				const float sin2Third = sinf(angle + 180.0f) * sinf(angle + 180.0f);
				const float sin2Fourth = sinf(angle + 270.0f) * sinf(angle + 270.0f);
				meshSpriteShaderStates_[i]->setUniformFloat("InstanceBlock", "color", sin2First, sin2Second, sin2Third, 0.5f + 0.5f * sin2Fourth);
			}

			const nc::DrawableNode *multitextureSprite = multitextureShaderStates_[i]->node();
			const bool multitextureSpriteIsCulled = (multitextureSprite && multitextureSprite->lastFrameRendered() < numFrames);
			if (multitextureSprite && multitextureSprite->isDrawEnabled() && multitextureSpriteIsCulled == false)
				multitextureShaderStates_[i]->setUniformFloat("InstanceBlock", "rotation", multitextureSprite->absRotation() * ncine::fDegToRad);
		}
	}
}

void MyEventHandler::onDrawViewport(nc::Viewport &viewport)
{
	// Dirtying the uniform cache value at each blur pass
	if (&viewport == pingViewport_.get())
	{
		vpPingSpriteShaderState_->setUniformFloat(nullptr, "uDirection", 1.0f, 0.0f);
		vpPongSpriteShaderState_->setUniformFloat(nullptr, "uDirection", 0.0f, 1.0f);
	}
}

void MyEventHandler::onResizeWindow(int width, int height)
{
	texture0_->init("Ping texture", nc::Texture::Format::RGB8, width, height);
	texture1_->init("Pong texture", nc::Texture::Format::RGB8, width, height);
	downsampleTexture0_->init("Downsample ping texture", nc::Texture::Format::RGB8, nc::Vector2i(width, height) / downsampleFactor);
	downsampleTexture1_->init("Downsample pong texture", nc::Texture::Format::RGB8, nc::Vector2i(width, height) / downsampleFactor);
	bloomTexture_->init("Bloom texture", nc::Texture::Format::RGB8, width, height);

	sceneViewport_->removeAllTextures();
	sceneViewport_->setTexture(texture0_.get());
	sceneViewportMrt_->removeAllTextures();
	sceneViewportMrt_->setTexture(0, texture0_.get());
	sceneViewportMrt_->setTexture(1, bloomTexture_.get());
	pingViewport_->removeAllTextures();
	pongViewport_->removeAllTextures();
	if (currentViewportSetup_ == ViewportSetup::BLOOM)
	{
		pingViewport_->setTexture(downsampleTexture1_.get());
		pongViewport_->setTexture(downsampleTexture0_.get());
	}
	else
	{
		pingViewport_->setTexture(texture1_.get());
		pongViewport_->setTexture(texture0_.get());
	}
	downsampleViewport_->removeAllTextures();
	downsampleViewport_->setTexture(downsampleTexture0_.get());
	upsampleViewport_->removeAllTextures();
	upsampleViewport_->setTexture(bloomTexture_.get());
	blendingViewport_->removeAllTextures();
	blendingViewport_->setTexture(texture1_.get());

	vpPingSprite_->resetTexture();
	vpPongSprite_->resetTexture();
	vpDownsampleSprite_->resetTexture();
	vpUpsampleSprite_->resetTexture();
	vpBlendingSprite_->resetTexture();
	sceneSprite_->resetTexture();

	vpPingSprite_->setPosition(width * 0.5f, height * 0.5f);
	vpPongSprite_->setPosition(width * 0.5f, height * 0.5f);
	vpDownsampleSprite_->setPosition(width * 0.5f, height * 0.5f);
	vpUpsampleSprite_->setPosition(width * 0.5f, height * 0.5f);
	vpBlendingSprite_->setPosition(width * 0.5f, height * 0.5f);
	sceneSprite_->setPosition(width * 0.5f, height * 0.5f);

	vpBlendingShaderState_->setTexture(1, bloomTexture_.get());
	vpPingSpriteShaderState_->setUniformFloat(nullptr, "uResolution",
	                                          static_cast<float>(pingViewport_->texture()->width()),
	                                          static_cast<float>(pingViewport_->texture()->height()));
	vpPongSpriteShaderState_->setUniformFloat(nullptr, "uResolution",
	                                          static_cast<float>(pongViewport_->texture()->width()),
	                                          static_cast<float>(pongViewport_->texture()->height()));

	for (unsigned int i = 0; i < NumSprites; i++)
	{
		sprites_[i]->setPosition(width * 0.15f + width * 0.1f * i, height * 0.5f);
		meshSprites_[i]->setPosition(width * 0.15f + width * 0.1f * i, height * 0.75f);
		multitextureSprites_[i]->setPosition(width * 0.15f + width * 0.1f * i, height * 0.25f);
	}
	debugText_->setPosition(width * 0.5f, height - debugText_->lineHeight() * 0.5f * 2.0f);
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	if (captureMouse)
	{
		checkClick(event.pointers[0].x, event.pointers[0].y);
		multiTextureShaderData.lightPosition.x = event.pointers[0].x;
		multiTextureShaderData.lightPosition.y = event.pointers[0].y;
		shaderDataChanged = true;
	}
}

void MyEventHandler::onTouchMove(const nc::TouchEvent &event)
{
	if (captureMouse)
	{
		multiTextureShaderData.lightPosition.x = event.pointers[0].x;
		multiTextureShaderData.lightPosition.y = event.pointers[0].y;
		shaderDataChanged = true;
	}
}
#endif

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();

	if (event.sym == nc::KeySym::B)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.sym == nc::KeySym::C)
		renderingSettings.cullingEnabled = !renderingSettings.cullingEnabled;
	else if (event.sym == nc::KeySym::R)
	{
		for (unsigned int i = 0; i < NumSprites; i++)
		{
			spriteShaderStates_[i]->setShader(spriteShaderStates_[i]->shader() ? nullptr : spriteShader_.get());
			meshSpriteShaderStates_[i]->setShader(meshSpriteShaderStates_[i]->shader() ? nullptr : meshShader_.get());
		}
		setupMeshVertices(meshSpriteShaderStates_[0]->shader() == nullptr);
	}
	else if (event.sym == nc::KeySym::T)
	{
		withAtlas_ = !withAtlas_;
		withAtlas_ ? setupAtlas() : setupTextures();
	}
	else if (event.sym == nc::KeySym::V)
	{
		if (currentViewportSetup_ != ViewportSetup::BLUR)
			currentViewportSetup_ = ViewportSetup::BLUR;
		else
			currentViewportSetup_ = ViewportSetup::NONE;
		setupViewport();
	}
	else if (event.sym == nc::KeySym::L)
	{
		if (currentViewportSetup_ != ViewportSetup::BLOOM)
			currentViewportSetup_ = ViewportSetup::BLOOM;
		else
			currentViewportSetup_ = ViewportSetup::NONE;
		setupViewport();
	}
	else if (event.sym == nc::KeySym::N1 && captureKeyboard)
	{
		numBlurPasses_ = 1;
		setupViewport();
	}
	else if (event.sym == nc::KeySym::N2 && captureKeyboard)
	{
		numBlurPasses_ = 2;
		setupViewport();
	}
	else if (event.sym == nc::KeySym::N3 && captureKeyboard)
	{
		numBlurPasses_ = 3;
		setupViewport();
	}
#if NCINE_WITH_IMGUI
	else if (event.mod & nc::KeyMod::CTRL && event.sym == nc::KeySym::H)
		showImGui = !showImGui;
#endif
	else if (event.sym == nc::KeySym::F)
	{
		nc::IGfxDevice &gfxDevice = nc::theApplication().gfxDevice();
		gfxDevice.setFullScreen(!gfxDevice.isFullScreen());
		if (gfxDevice.isFullScreen() == false)
			gfxDevice.setWindowSize(nc::theApplication().appConfiguration().resolution);
	}
	else if (event.sym == nc::KeySym::P)
		pause_ = !pause_;
	else if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.button == nc::MouseButton::LEFT && captureMouse)
	{
		checkClick(static_cast<float>(event.x), static_cast<float>(event.y));
		multiTextureShaderData.lightPosition.x = event.x;
		multiTextureShaderData.lightPosition.y = event.y;
		shaderDataChanged = true;
	}
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state)
{
	if (state.isButtonDown(nc::MouseButton::LEFT) && captureMouse)
	{
		multiTextureShaderData.lightPosition.x = state.x;
		multiTextureShaderData.lightPosition.y = state.y;
		shaderDataChanged = true;
	}
}

void MyEventHandler::onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event)
{
	if (event.axisName == nc::AxisName::LX)
		joyVectorLeft_.x = event.value;
	else if (event.axisName == nc::AxisName::LY)
		joyVectorLeft_.y = -event.value;
}

void MyEventHandler::onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();

	if (event.buttonName == nc::ButtonName::A)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.buttonName == nc::ButtonName::B)
		renderingSettings.cullingEnabled = !renderingSettings.cullingEnabled;
	else if (event.buttonName == nc::ButtonName::X)
	{
		withAtlas_ = !withAtlas_;
		withAtlas_ ? setupAtlas() : setupTextures();
	}
	else if (event.buttonName == nc::ButtonName::Y)
	{
		multiTextureShaderData.lightPosition.x = nc::theApplication().width() * 0.5f;
		multiTextureShaderData.lightPosition.y = nc::theApplication().height() * 0.5f;
		shaderDataChanged = true;
	}
	else if (event.buttonName == nc::ButtonName::RBUMPER)
	{
		if (currentViewportSetup_ == ViewportSetup::NONE)
			currentViewportSetup_ = ViewportSetup::BLUR;
		else if (currentViewportSetup_ == ViewportSetup::BLUR)
			currentViewportSetup_ = ViewportSetup::BLOOM;
		else if (currentViewportSetup_ == ViewportSetup::BLOOM)
			currentViewportSetup_ = ViewportSetup::NONE;
		setupViewport();
	}
	else if (event.buttonName == nc::ButtonName::START)
		pause_ = !pause_;
	else if (event.buttonName == nc::ButtonName::GUIDE)
		nc::theApplication().quit();
}

void MyEventHandler::onJoyDisconnected(const nc::JoyConnectionEvent &event)
{
	joyVectorLeft_ = nc::Vector2f::Zero;
}

void MyEventHandler::setupAtlas()
{
	nc::Recti texRects[4];
	texRects[0] = nc::Recti(0, 0, 128, 128);
	texRects[1] = nc::Recti(128, 0, 128, 128);
	texRects[2] = nc::Recti(0, 128, 128, 128);
	texRects[3] = nc::Recti(128, 128, 128, 128);

	for (unsigned int i = 0; i < NumSprites; i++)
	{
		sprites_[i]->setTexture(megaTexture_.get());
		sprites_[i]->setTexRect(texRects[i % NumTextures]);
		meshSprites_[i]->setTexture(megaTexture_.get());
		meshSprites_[i]->setTexRect(texRects[i % NumTextures]);
	}
}

void MyEventHandler::setupTextures()
{
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		sprites_[i]->setTexture(textures_[i % NumTextures].get());
		sprites_[i]->setTexRect(textures_[i % NumTextures]->rect());
		meshSprites_[i]->setTexture(textures_[i % NumTextures].get());
		meshSprites_[i]->setTexRect(textures_[i % NumTextures]->rect());
	}
}

void MyEventHandler::setupViewport()
{
	nc::Viewport::chain().clear();

	if (currentViewportSetup_ != ViewportSetup::NONE)
	{
		if (currentViewportSetup_ == ViewportSetup::BLOOM)
		{
			nc::Viewport::chain().pushBack(blendingViewport_.get());

			vpPingSprite_->setScale(downsampleFactor);
			vpPingSprite_->setTexture(downsampleTexture0_.get());
			vpPingSpriteShaderState_->setUniformFloat(nullptr, "uResolution", static_cast<float>(downsampleTexture0_->width()), static_cast<float>(downsampleTexture0_->height()));
			vpPongSprite_->setScale(downsampleFactor);
			vpPongSprite_->setTexture(downsampleTexture1_.get());
			vpPongSpriteShaderState_->setUniformFloat(nullptr, "uResolution", static_cast<float>(downsampleTexture1_->width()), static_cast<float>(downsampleTexture1_->height()));
			nc::theApplication().screenViewport().setRootNode(sceneSprite_.get());

			nc::Viewport::chain().pushBack(upsampleViewport_.get());
		}
		else if (currentViewportSetup_ == ViewportSetup::BLUR)
		{
			vpPingSprite_->setScale(1.0f);
			vpPingSprite_->setTexture(texture0_.get());
			vpPingSpriteShaderState_->setUniformFloat(nullptr, "uResolution", static_cast<float>(texture0_->width()), static_cast<float>(texture0_->height()));
			vpPongSprite_->setScale(1.0f);
			vpPongSprite_->setTexture(texture1_.get());
			vpPongSpriteShaderState_->setUniformFloat(nullptr, "uResolution", static_cast<float>(texture1_->width()), static_cast<float>(texture1_->height()));
			nc::theApplication().screenViewport().setRootNode(vpPongSprite_.get());
		}

		// Ping-pong passes of the separable blur shader
		for (unsigned int i = 0; i < numBlurPasses_; i++)
		{
			nc::Viewport::chain().pushBack(pongViewport_.get());
			nc::Viewport::chain().pushBack(pingViewport_.get());
		}

		if (currentViewportSetup_ == ViewportSetup::BLOOM)
			nc::Viewport::chain().pushBack(downsampleViewport_.get());

		nc::Viewport::chain().pushBack(sceneViewport_.get());

		if (currentViewportSetup_ == ViewportSetup::BLOOM)
			nc::Viewport::chain().pushBack(sceneViewportMrt_.get());
	}
	else
		nc::theApplication().screenViewport().setRootNode(rootNode_.get());

	if (currentViewportSetup_ == ViewportSetup::BLOOM)
	{
		for (unsigned int i = 0; i < NumSprites; i++)
		{
			multitextureSprites_[i]->setParent(rootNodeMrt_.get());
			multitextureShaderStates_[i]->setShader(multitextureMrtShader_.get());
			multitextureShaderStates_[i]->setUniformInt(nullptr, "uTexture0", 0); // GL_TEXTURE0
			multitextureShaderStates_[i]->setUniformInt(nullptr, "uTexture1", 1); // GL_TEXTURE1
		}
		// Only the MRT scene viewport should clear the texture
		sceneViewport_->setClearMode(nc::Viewport::ClearMode::NEVER);

		pingViewport_->removeAllTextures();
		pingViewport_->setTexture(downsampleTexture1_.get());
		pongViewport_->removeAllTextures();
		pongViewport_->setTexture(downsampleTexture0_.get());
	}
	else
	{
		for (unsigned int i = 0; i < NumSprites; i++)
		{
			multitextureSprites_[i]->setParent(rootNode_.get());
			multitextureShaderStates_[i]->setShader(multitextureShader_.get());
			multitextureShaderStates_[i]->setUniformInt(nullptr, "uTexture0", 0); // GL_TEXTURE0
			multitextureShaderStates_[i]->setUniformInt(nullptr, "uTexture1", 1); // GL_TEXTURE1
		}
		sceneViewport_->setClearMode(nc::Viewport::ClearMode::EVERY_FRAME);

		pingViewport_->removeAllTextures();
		pingViewport_->setTexture(texture1_.get());
		pongViewport_->removeAllTextures();
		pongViewport_->setTexture(texture0_.get());
	}
}

void MyEventHandler::setupMeshVertices(bool defaultShader)
{
	unsigned int texelStartIndex = 0;
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		// Setting mesh vertices
		if (i < NumTextures)
		{
			meshSprites_[i]->createVerticesFromTexels(NumTexelPoints[i], &TexelPoints[texelStartIndex]);
			texelStartIndex += NumTexelPoints[i];

			if (defaultShader == false)
			{
				// Reorganize mesh vertices data for the custom attributes
				const unsigned int numVertices = meshSprites_[i]->numVertices();
				const nc::MeshSprite::Vertex *vertices = reinterpret_cast<const nc::MeshSprite::Vertex *>(meshSprites_[i]->vertices());
				nctl::UniquePtr<InvertedVertexTextureZ[]> newVertices = nctl::makeUnique<InvertedVertexTextureZ[]>(numVertices);
				for (unsigned int vertIt = 0; vertIt < numVertices; vertIt++)
				{
					const nc::MeshSprite::Vertex &src = vertices[vertIt];
					InvertedVertexTextureZ &dest = newVertices[vertIt];
					dest.u = src.u;
					dest.v = src.v;
					dest.x = src.x;
					dest.y = src.y;
					dest.z = 1.0f / static_cast<float>(vertIt + 1);
				}
				meshSprites_[i]->copyVertices(numVertices, sizeof(InvertedVertexTextureZ), newVertices.get());
			}
		}
		else
			meshSprites_[i]->setVertices(*meshSprites_[i % NumTextures]);
	}
}

void MyEventHandler::checkClick(float x, float y)
{
	const nc::Rectf debugTextRect = nc::Rectf::fromCenterSize(debugText_->absPosition(), debugText_->absSize());

#ifdef __ANDROID__
	// Make it slightly easier to touch on Android
	if (debugTextRect.contains(x, y))
#else
	if (debugTextRect.contains(x, y) && y <= debugTextRect.y + debugTextRect.h * 0.5f)
#endif
	{
		nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
		const float xPos = x - debugTextRect.x;
		if (xPos <= debugTextRect.w * 0.23f)
			settings.batchingEnabled = !settings.batchingEnabled;
		else if (xPos >= debugTextRect.w * 0.23f && xPos <= debugTextRect.w * 0.44f)
			settings.cullingEnabled = !settings.cullingEnabled;
		else if (xPos >= debugTextRect.w * 0.44f && xPos <= debugTextRect.w * 0.76f)
		{
			withAtlas_ = !withAtlas_;
			withAtlas_ ? setupAtlas() : setupTextures();
		}
		else if (xPos >= debugTextRect.w * 0.76f)
		{
			if (currentViewportSetup_ == ViewportSetup::NONE)
				currentViewportSetup_ = ViewportSetup::BLUR;
			else
				currentViewportSetup_ = ViewportSetup::NONE;
			setupViewport();
		}
	}
}
