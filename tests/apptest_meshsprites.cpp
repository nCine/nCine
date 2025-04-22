#include <nctl/algorithms.h>
#include "apptest_meshsprites.h"
#include <ncine/Application.h>
#include <ncine/Texture.h>
#include <ncine/Viewport.h>
#include <ncine/Camera.h>
#include <ncine/Sprite.h>
#include <ncine/MeshSprite.h>
#include <ncine/TextNode.h>
#include "apptest_datapath.h"

namespace {

#ifdef __ANDROID__
const char *MegaTextureFile = "megatexture_256_cutout_ETC2.ktx";
const char *Texture1File = "texture1_cutout_ETC2.ktx";
const char *Texture2File = "texture2_cutout_ETC2.ktx";
const char *Texture3File = "texture3_cutout_ETC2.ktx";
const char *Texture4File = "texture4_cutout_ETC2.ktx";
const char *FontTextureFile = "DroidSans32_256_ETC2.ktx";
#else
const char *MegaTextureFile = "megatexture_256_cutout.png";
const char *Texture1File = "texture1_cutout.png";
const char *Texture2File = "texture2_cutout.png";
const char *Texture3File = "texture3_cutout.png";
const char *Texture4File = "texture4_cutout.png";
const char *FontTextureFile = "DroidSans32_256.png";
#endif
const char *FontFntFile = "DroidSans32_256.fnt";

const float SpriteScale = 0.16f;
const float SpriteSpacing = 1.25f;
const float SpriteActiveAmount = MyEventHandler::NumSprites * 0.5f;
const float EnableSpriteSpeed = MyEventHandler::NumSprites * 0.15f;

const float ScaleSpeed = 0.3f;
const float MaxCameraScale = 3.0f;
const float MinCameraScale = 0.5f;
const float DoubleClickDelay = 0.3f;

const int NumTexelPoints[MyEventHandler::NumTextures] = { 3, 4, 5, 4 };
const nc::Vector2f TexelPoints[] = {
	// clang-format off
	{4.0f, 2.0f}, {124.0f, 2.0f}, {64.0f, 122.0f},
	{13.0f, 13.0f}, {115.0f, 13.0f}, {13.0f, 115.0f}, {115.0f, 115.0f},
	{3.0f, 79.0f}, {26.0f, 2.0f}, {64.0f, 125.0f}, {102.0f, 2.0f}, {125.0f, 79.0f},
	{20.0f, 20.0f}, {107.0f, 20.0f}, {20.0f, 107.0f}, {107.0f, 107.0f},
	// clang-format on
};

const int NumTranspTexelPoints[MyEventHandler::NumTextures] = { 8, 10, 12, 10 };
const nc::Vector2f TranspTexelPoints[] = {
	// clang-format off
	{0.0f, 0.0f}, {4.0f, 2.0f}, {64.0f, 122.0f}, {64.0f, 128.0f}, {124.0f, 2.0f}, {128.0f, 0.0f}, {4.0f, 2.0f}, {0.0f, 0.0f},
	{0.0f, 0.0f}, {13.0f, 13.0f}, {0.0f, 128.0f}, {13.0f, 115.0f}, {128.0f, 128.0f}, {115.0f, 115.0f}, {128.0f, 0.0f}, {115.0f, 13.0f}, {0.0f, 0.0f}, {13.0f, 13.0f},
	{24.0f, 0.0f}, {26.0f, 2.0f}, {0.0f, 79.0f}, {3.0f, 79.0f}, {64.0f, 128.0f}, {64.0f, 125.0f}, {128.0f, 79.0f}, {125.0f, 79.0f}, {102.0f, 2.0f}, {104.0f, 0.0f}, {26.0f, 2.0f}, {24.0f, 0.0f},
	{0.0f, 0.0f}, {20.0f, 20.0f}, {0.0f, 128.0f}, {20.0f, 107.0f}, {128.0f, 128.0f}, {107.0f, 107.0f}, {128.0f, 0.0f}, {107.0f, 20.0f}, {0.0f, 0.0f}, {20.0f, 20.0f},
	// clang-format on
};

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
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	megaTexture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", MegaTextureFile)).data());
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture1File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture2File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture3File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture4File)).data()));

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());

	cameraNode_ = nctl::makeUnique<nc::SceneNode>(&rootNode, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	// Ignore inter-siblings drawing order to allow batching without a texture atlas
	cameraNode_->setVisitOrderState(nc::SceneNode::VisitOrderState::DISABLED);
	viewport_ = nctl::makeUnique<nc::Viewport>();
	viewport_->setRootNode(cameraNode_.get());
	camera_ = nctl::makeUnique<nc::Camera>();
	viewport_->setCamera(camera_.get());

	debugString_ = nctl::makeUnique<nctl::String>(128);
	debugText_ = nctl::makeUnique<nc::TextNode>(&rootNode, font_.get());
	debugText_->setLayer(1);
	debugText_->setPosition((nc::theApplication().width() - debugText_->width()) * 0.5f,
	                        nc::theApplication().height() - debugText_->lineHeight() * 0.5f * 2.0f);
	debugText_->setColor(255, 255, 0, 255);
	debugText_->setAlignment(nc::TextNode::Alignment::CENTER);

	// Starting with mesh sprites enabled
	meshSpritesEnabled_ = true;

	Direction direction = Direction::DOWN;
	int numBeforeChange = 1;
	int moreToChange = 1;
	nc::Vector2f pos(0.0f, 0.0f);
	int texelStartIndex = 0;
	int transpTexelStartIndex = 0;
	for (unsigned int i = 0; i < NumSprites; i++)
	{
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(cameraNode_.get(), textures_[i % NumTextures].get(), pos.x, pos.y));
		sprites_.back()->setScale(SpriteScale);
		sprites_.back()->setEnabled(!meshSpritesEnabled_);
		meshSprites_.pushBack(nctl::makeUnique<nc::MeshSprite>(cameraNode_.get(), textures_[i % NumTextures].get(), pos.x, pos.y));
		meshSprites_.back()->setBlendingEnabled(false);
		meshSprites_.back()->setScale(SpriteScale);
		meshSprites_.back()->setEnabled(meshSpritesEnabled_);
		transpMeshSprites_.pushBack(nctl::makeUnique<nc::MeshSprite>(cameraNode_.get(), textures_[i % NumTextures].get(), pos.x, pos.y));
		transpMeshSprites_.back()->setScale(SpriteScale);
		transpMeshSprites_.back()->setEnabled(meshSpritesEnabled_);

		// Setting mesh vertices
		if (i < NumTextures)
		{
			meshSprites_[i]->createVerticesFromTexels(NumTexelPoints[i], &TexelPoints[texelStartIndex]);
			texelStartIndex += NumTexelPoints[i];
			transpMeshSprites_[i]->createVerticesFromTexels(NumTranspTexelPoints[i], &TranspTexelPoints[transpTexelStartIndex]);
			transpTexelStartIndex += NumTranspTexelPoints[i];
		}
		else
		{
			meshSprites_[i]->setVertices(*meshSprites_[i % NumTextures]);
			transpMeshSprites_[i]->setVertices(*transpMeshSprites_[i % NumTextures]);
		}

		// Setting sprites position
		switch (direction)
		{
			case Direction::RIGHT: pos.x += sprites_[i]->width() * SpriteSpacing; break;
			case Direction::DOWN: pos.y -= sprites_[i]->height() * SpriteSpacing; break;
			case Direction::LEFT: pos.x -= sprites_[i]->width() * SpriteSpacing; break;
			case Direction::UP: pos.y += sprites_[i]->height() * SpriteSpacing; break;
		}
		if (moreToChange == numBeforeChange)
		{
			direction = static_cast<Direction>((direction + 1) % 4);
			moreToChange = 0;
			numBeforeChange += (direction % 2);
		}
		moreToChange++;
	}
	activeSpritesFloat_ = SpriteActiveAmount;
	updateActiveSprites_ = true;

	withAtlas_ = false;
	withAtlas_ ? setupAtlas() : setupTextures();
	withViewport_ = true;
	setupViewport();

	pause_ = false;
	animDivider_ = 1;
	angle_ = 0.0f;
	resetCamera();

	scrollOrigin_ = nc::Vector2f::Zero;
	scrollMove_ = nc::Vector2f::Zero;
	joyVectorLeft_ = nc::Vector2f::Zero;
	joyVectorRight_ = nc::Vector2f::Zero;
}

void MyEventHandler::onFrameStart()
{
	const float frameTime = nc::theApplication().frameTime();
	if (!pause_)
		angle_ += 20.0f * frameTime;

	const nc::KeyboardState &keyState = nc::theApplication().inputManager().keyboardState();

	if (keyState.isKeyDown(nc::KeySym::UP) || keyState.isKeyDown(nc::KeySym::W))
		camScale_ += ScaleSpeed * frameTime;
	else if (keyState.isKeyDown(nc::KeySym::DOWN) || keyState.isKeyDown(nc::KeySym::S))
		camScale_ -= ScaleSpeed * frameTime;

	if (keyState.isKeyDown(nc::KeySym::RIGHT) || keyState.isKeyDown(nc::KeySym::D))
	{
		activeSpritesFloat_ += EnableSpriteSpeed * frameTime;
		updateActiveSprites_ = true;
	}
	else if (keyState.isKeyDown(nc::KeySym::LEFT) || keyState.isKeyDown(nc::KeySym::A))
	{
		activeSpritesFloat_ -= EnableSpriteSpeed * frameTime;
		updateActiveSprites_ = true;
	}

	if (joyVectorLeft_.length() > nc::IInputManager::LeftStickDeadZone)
	{
		activeSpritesFloat_ += joyVectorLeft_.x * EnableSpriteSpeed * frameTime;
		updateActiveSprites_ = true;
	}

	if (joyVectorRight_.length() > nc::IInputManager::RightStickDeadZone)
		camScale_ += joyVectorRight_.y * ScaleSpeed * frameTime;

	const nc::Vector2f scrollDiff = scrollMove_ - scrollOrigin_;
	if (scrollDiff.sqrLength() > 2.0f)
	{
		camScale_ += scrollDiff.y * 0.001f;
		activeSpritesFloat_ += scrollDiff.x * 0.001f * EnableSpriteSpeed;
		scrollOrigin_ = scrollMove_;
		updateActiveSprites_ = true;
	}

	if (camScale_ > MaxCameraScale)
		camScale_ = MaxCameraScale;
	else if (camScale_ < MinCameraScale)
		camScale_ = MinCameraScale;
	activeSpritesFloat_ = nctl::clamp(activeSpritesFloat_, 0.0f, static_cast<float>(NumSprites));

	const nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
	debugString_->clear();
	debugString_->format("%s sprites: %d, scale: %.2f", meshSpritesEnabled_ ? "Mesh" : "Regular", static_cast<int>(activeSpritesFloat_), -camScale_);
	debugString_->formatAppend("\nbatching: %s, culling: %s, texture atlas: %s, viewport: %s", stringOnOff(settings.batchingEnabled),
	                           stringOnOff(settings.cullingEnabled), stringOnOff(withAtlas_), stringOnOff(withViewport_));
	debugText_->setString(*debugString_);

	if (withViewport_)
	{
		const nc::Vector2f camPos(nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
		camera_->setView(-camPos, 0.0f, camScale_);
	}
	else
		cameraNode_->setScale(camScale_);

	if (!pause_)
	{
		for (unsigned int i = 0; i < NumSprites; i++)
		{
			if ((i + 1) % animDivider_ != 0)
				continue;

			const float rotationAngle = angle_ + NumSprites * 0.5f - i;
			sprites_[i]->setRotation(rotationAngle);
			meshSprites_[i]->setRotation(rotationAngle);
			transpMeshSprites_[i]->setRotation(rotationAngle);
		}
	}

	if (updateActiveSprites_)
	{
		updateEnabledSprites();
		updateActiveSprites_ = false;
	}
}

void MyEventHandler::onResizeWindow(int width, int height)
{
	viewport_->setViewportRect(0, 0, width, height);
	camera_->setOrthoProjection(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

	debugText_->setPosition(width * 0.5f, height - debugText_->lineHeight() * 0.5f * 2.0f);
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	scrollOrigin_.x = event.pointers[0].x;
	scrollOrigin_.y = event.pointers[0].y;
	scrollMove_ = scrollOrigin_;

	checkClick(event.pointers[0].x, event.pointers[0].y);
}

void MyEventHandler::onTouchMove(const nc::TouchEvent &event)
{
	scrollMove_.x = event.pointers[0].x;
	scrollMove_.y = event.pointers[0].y;
}

void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	if (lastClickTime_.secondsSince() < DoubleClickDelay)
		resetCamera();
	lastClickTime_ = nc::TimeStamp::now();
}
#endif

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();

	if (event.sym == nc::KeySym::B)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.sym == nc::KeySym::C)
		renderingSettings.cullingEnabled = !renderingSettings.cullingEnabled;
	else if (event.sym == nc::KeySym::T)
	{
		withAtlas_ = !withAtlas_;
		withAtlas_ ? setupAtlas() : setupTextures();
	}
	else if (event.sym == nc::KeySym::V)
	{
		withViewport_ = !withViewport_;
		setupViewport();
	}
	else if (event.sym == nc::KeySym::M)
	{
		meshSpritesEnabled_ = !meshSpritesEnabled_;
		toggleSpritesType();
	}
	else if (event.sym == nc::KeySym::R)
		resetCamera();
	else if (event.sym == nc::KeySym::N1)
		animDivider_ = 1;
	else if (event.sym == nc::KeySym::N2)
		animDivider_ = 2;
	else if (event.sym == nc::KeySym::N3)
		animDivider_ = 3;
	else if (event.sym == nc::KeySym::N4)
		animDivider_ = 4;
	else if (event.sym == nc::KeySym::N8)
		animDivider_ = 8;
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
	if (event.button == nc::MouseButton::LEFT)
		checkClick(static_cast<float>(event.x), static_cast<float>(event.y));
}

void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
{
	if (event.button == nc::MouseButton::LEFT)
	{
		if (lastClickTime_.secondsSince() < DoubleClickDelay)
			resetCamera();
		lastClickTime_ = nc::TimeStamp::now();
	}
}

void MyEventHandler::onScrollInput(const nc::ScrollEvent &event)
{
	camScale_ += 0.1f * event.y;
}

void MyEventHandler::onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event)
{
	if (event.axisName == nc::AxisName::LX)
		joyVectorLeft_.x = event.value;
	else if (event.axisName == nc::AxisName::RY)
		joyVectorRight_.y = -event.value;
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
		resetCamera();
	else if (event.buttonName == nc::ButtonName::RBUMPER)
	{
		withViewport_ = !withViewport_;
		setupViewport();
	}
	else if (event.buttonName == nc::ButtonName::LBUMPER)
	{
		meshSpritesEnabled_ = !meshSpritesEnabled_;
		toggleSpritesType();
	}
	else if (event.buttonName == nc::ButtonName::START)
		pause_ = !pause_;
	else if (event.buttonName == nc::ButtonName::GUIDE)
		nc::theApplication().quit();
}

void MyEventHandler::onJoyDisconnected(const nc::JoyConnectionEvent &event)
{
	joyVectorLeft_ = nc::Vector2f::Zero;
	joyVectorRight_ = nc::Vector2f::Zero;
}

void MyEventHandler::resetCamera()
{
	camScale_ = 1.0f;
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
		transpMeshSprites_[i]->setTexture(megaTexture_.get());
		transpMeshSprites_[i]->setTexRect(texRects[i % NumTextures]);
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
		transpMeshSprites_[i]->setTexture(textures_[i % NumTextures].get());
		transpMeshSprites_[i]->setTexRect(textures_[i % NumTextures]->rect());
	}
}

void MyEventHandler::setupViewport()
{
	if (withViewport_)
	{
		nc::Viewport::chain().pushBack(viewport_.get());
		cameraNode_->setParent(nullptr);
		cameraNode_->setPosition(0.0f, 0.0f);
		cameraNode_->setScale(1.0f);
	}
	else
	{
		nc::Viewport::chain().clear();
		cameraNode_->setParent(&nc::theApplication().rootNode());
		cameraNode_->setPosition(nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	}
}

void MyEventHandler::toggleSpritesType()
{
	const unsigned int activeSprites = static_cast<unsigned int>(activeSpritesFloat_);

	for (unsigned int i = 0; i < activeSprites; i++)
	{
		sprites_[i]->setEnabled(!meshSpritesEnabled_);
		meshSprites_[i]->setEnabled(meshSpritesEnabled_);
		transpMeshSprites_[i]->setEnabled(meshSpritesEnabled_);
	}
}

void MyEventHandler::updateEnabledSprites()
{
	const unsigned int activeSprites = static_cast<unsigned int>(activeSpritesFloat_);

	for (unsigned int i = 0; i < activeSprites; i++)
	{
		if (meshSpritesEnabled_)
		{
			meshSprites_[i]->setEnabled(true);
			transpMeshSprites_[i]->setEnabled(true);
		}
		else
			sprites_[i]->setEnabled(true);
	}
	for (unsigned int i = activeSprites; i < NumSprites; i++)
	{
		if (meshSpritesEnabled_)
		{
			meshSprites_[i]->setEnabled(false);
			transpMeshSprites_[i]->setEnabled(false);
		}
		else
			sprites_[i]->setEnabled(false);
	}
}

void MyEventHandler::checkClick(float x, float y)
{
	const nc::Rectf debugTextRect = nc::Rectf::fromCenterSize(debugText_->absPosition(), debugText_->absSize());

	if (debugTextRect.contains(x, y))
	{
		if (y <= debugTextRect.y + debugTextRect.h * 0.5f)
		{
			nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
			const float xPos = x - debugTextRect.x;
			if (xPos <= debugTextRect.w * 0.24f)
				settings.batchingEnabled = !settings.batchingEnabled;
			else if (xPos >= debugTextRect.w * 0.24f && xPos <= debugTextRect.w * 0.45f)
				settings.cullingEnabled = !settings.cullingEnabled;
			else if (xPos >= debugTextRect.w * 0.45f && xPos <= debugTextRect.w * 0.77f)
			{
				withAtlas_ = !withAtlas_;
				withAtlas_ ? setupAtlas() : setupTextures();
			}
			else if (xPos >= debugTextRect.w * 0.77f)
			{
				withViewport_ = !withViewport_;
				setupViewport();
			}
		}
		else
		{
			meshSpritesEnabled_ = !meshSpritesEnabled_;
			toggleSpritesType();
		}
	}
}
