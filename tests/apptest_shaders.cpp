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
const char *Texture1File = "texture1_ETC2.ktx";
const char *Texture2File = "texture2_ETC2.ktx";
const char *Texture3File = "texture3_ETC2.ktx";
const char *Texture4File = "texture4_ETC2.ktx";
const char *FontTextureFile = "DroidSans32_256_ETC2.ktx";
#else
const char *Texture1File = "texture1.png";
const char *Texture2File = "texture2.png";
const char *Texture3File = "texture3.png";
const char *Texture4File = "texture4.png";
const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

const int NumTexelPoints = 3;
const nc::Vector2f TexelPoints[] = {
	{ 4.0f, 2.0f }, { 124.0f, 2.0f }, { 64.0f, 122.0f }
};

const char *stringOnOff(bool enabled)
{
	return enabled ? "on" : "off";
}

auto updateSpriteAngle = [](nc::DrawableNode &node)
{
	nc::Sprite &sprite = static_cast<nc::Sprite &>(node);
	nc::ShaderState &ss = sprite.shaderState();
	const float angle = ss.userData() ? *reinterpret_cast<float *>(ss.userData()) : 0.0f;
	ss.setUniformFloat("InstanceBlock", "angle", angle);
};

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
}

void MyEventHandler::onInit()
{
	const float width = nc::theApplication().width();
	const float height = nc::theApplication().height();

	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture1File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture2File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture3File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture4File)).data()));

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());

	viewport_ = nctl::makeUnique<nc::Viewport>(nc::theApplication().width(), nc::theApplication().height());
	rootNode_ = nctl::makeUnique<nc::SceneNode>();
	viewport_->setClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	viewport_->setRootNode(rootNode_.get());

	vpBlurShader_ = nctl::makeUnique<nc::Shader>(nc::Shader::LoadMode::STRING, sprite_vs, sprite_blur_fs);
	FATAL_ASSERT(vpBlurShader_->isLinked());
	vpSprite_ = nctl::makeUnique<nc::Sprite>(nullptr, viewport_->texture(), nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	vpSprite_->setFlippedY(true);
	nc::ShaderState &vpShaderState = vpSprite_->shaderState();
	vpShaderState.setShader(vpBlurShader_.get());
	vpShaderState.setUniformFloat(nullptr, "uResolution", static_cast<float>(viewport_->width()), static_cast<float>(viewport_->height()));

	debugString_ = nctl::makeUnique<nctl::String>(128);
	debugText_ = nctl::makeUnique<nc::TextNode>(rootNode_.get(), font_.get());
	debugText_->setLayer(1);
	debugText_->setPosition((nc::theApplication().width() - debugText_->width()) * 0.5f,
	                        nc::theApplication().height() - debugText_->lineHeight() * 0.5f * 2.0f);
	debugText_->setColor(255, 255, 0, 255);
	debugText_->setAlignment(nc::TextNode::Alignment::CENTER);

	spriteShader_ = nctl::makeUnique<nc::Shader>(nc::Shader::LoadMode::STRING, sprite_vs, sprite_fs);
	FATAL_ASSERT(spriteShader_->isLinked());
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(rootNode_.get(), textures_[i % NumTextures].get(), width * 0.15f + width * 0.1f * i, height * 0.5f));
		sprites_.back()->setScale(0.5f);

		nc::ShaderState &shaderState = sprites_.back()->shaderState();
		shaderState.setShader(spriteShader_.get());
		shaderState.setUserData(&angle_);
		shaderState.setUpdateFunction(updateSpriteAngle);
	}

	meshShader_ = nctl::makeUnique<nc::Shader>(nc::Shader::LoadMode::STRING, meshsprite_vs, meshsprite_fs);
	FATAL_ASSERT(meshShader_->isLinked());
	meshSprite_ = nctl::makeUnique<nc::MeshSprite>(rootNode_.get(), textures_[0].get(), width * 0.5f, height * 0.8f);
	meshSprite_->createVerticesFromTexels(NumTexelPoints, TexelPoints);
	nc::ShaderState &meshSpriteShaderState = meshSprite_->shaderState();
	meshSpriteShaderState.setShader(meshShader_.get());

	withViewport_ = false;
	setupViewport();
	pause_ = false;
	angle_ = 0.0f;
}

void MyEventHandler::onFrameStart()
{
	const float height = nc::theApplication().height();

	const nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
	debugString_->clear();
	debugString_->format("batching: %s, culling: %s, viewport: %s", stringOnOff(settings.batchingEnabled),
	                     stringOnOff(settings.cullingEnabled), stringOnOff(withViewport_));
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
	}

	meshSprite_->setRotation(angle_ * 20.0f);
	const float sin2First = sinf(angle_) * sinf(angle_);
	const float sin2Second = sinf(angle_ + 90.0f) * sinf(angle_ + 90.0f);
	const float sin2Third = sinf(angle_ + 180.0f) * sinf(angle_ + 180.0f);
	const float sin2Fourth = sinf(angle_ + 270.0f) * sinf(angle_ + 270.0f);
	meshSprite_->shaderState().setUniformFloat("InstanceBlock", "color", sin2First, sin2Second, sin2Third, 0.5f + 0.5f * sin2Fourth);
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	checkClick(event.pointers[0].x, event.pointers[0].y);
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
			nc::Shader *spriteShader = sprites_[i]->shaderState().shader() ? nullptr : spriteShader_.get();
			sprites_[i]->shaderState().setShader(spriteShader);
			sprites_[i]->shaderState().setUpdateFunction(updateSpriteAngle);
		}

		nc::Shader *meshShader = meshSprite_->shaderState().shader() ? nullptr : meshShader_.get();
		meshSprite_->shaderState().setShader(meshShader);
	}
	else if (event.sym == nc::KeySym::V)
	{
		withViewport_ = !withViewport_;
		setupViewport();
	}
	else if (event.sym == nc::KeySym::P)
		pause_ = !pause_;
	else if (event.sym == nc::KeySym::SPACE)
	{
		const bool isSuspended = nc::theApplication().isSuspended();
		nc::theApplication().setSuspended(!isSuspended);
	}
	else if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
}

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
		checkClick(static_cast<float>(event.x), static_cast<float>(event.y));
}

void MyEventHandler::setupViewport()
{
	if (withViewport_)
	{
		nc::theApplication().rootViewport().setNextViewport(viewport_.get());
		rootNode_->setParent(nullptr);
		vpSprite_->setParent(&nc::theApplication().rootNode());
	}
	else
	{
		nc::theApplication().rootViewport().setNextViewport(nullptr);
		rootNode_->setParent(&nc::theApplication().rootNode());
		vpSprite_->setParent(nullptr);
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
		if (xPos <= debugTextRect.w * 0.34f)
			settings.batchingEnabled = !settings.batchingEnabled;
		else if (xPos >= debugTextRect.w * 0.34f && xPos <= debugTextRect.w * 0.64f)
			settings.cullingEnabled = !settings.cullingEnabled;
		else if (xPos >= debugTextRect.w * 0.64f)
		{
			withViewport_ = !withViewport_;
			setupViewport();
		}
	}
}
