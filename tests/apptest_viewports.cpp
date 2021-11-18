#include <ncine/common_constants.h>
#include "apptest_viewports.h"
#include <ncine/Random.h>
#include <ncine/Application.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/TextNode.h>
#include <ncine/Viewport.h>
#include <ncine/Camera.h>
#include <ncine/imgui.h>
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

const float MoveSpeed = 500.0f;
const float RotateSpeed = 30.0f;
const float ScaleSpeed = 0.3f;

const float MaxCameraScale = 3.0f;
const float MinCameraScale = 0.5f;
const float DoubleClickDelay = 0.3f;

const float ViewHalfWidth = 1500.0f;
const float ViewHalfHeight = 1000.0f;

const char *ColorFormatLabels[] = { "RGB8", "RGBA8" };
const char *DepthFormatLabels[] = { "None", "Depth16", "Depth24", "Depth24_Stencil8" };

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
	config.withDebugOverlay = false;
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();
	nc::Viewport &rootViewport = nc::theApplication().rootViewport();

	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture1File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture2File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture3File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture4File)).data()));

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());

	cameraNode_ = nctl::makeUnique<nc::SceneNode>(&rootNode);

	debugString_ = nctl::makeUnique<nctl::String>(128);
	debugText_ = nctl::makeUnique<nc::TextNode>(&rootNode, font_.get());
	debugText_->setPosition((nc::theApplication().width() - debugText_->width()) * 0.5f,
	                        nc::theApplication().height() - debugText_->lineHeight() * 0.5f * 2.0f);
	debugText_->setColor(255, 255, 0, 255);
	debugText_->setAlignment(nc::TextNode::Alignment::CENTER);

	nc::Vector2i vpSizes[NumViewports] = { nc::Vector2i(640, 360), nc::Vector2i(320, 180) };
	nc::Colorf vpClearColors[NumViewports] = { nc::Colorf(0.25f, 0.25f, 0.25f, 0.25f), nc::Colorf(0.15f, 0.15f, 0.15f, 0.15f) };
	for (unsigned int i = 0; i < NumViewports; i++)
	{
		vpNodes_.pushBack(nctl::makeUnique<nc::SceneNode>());
		cameras_.pushBack(nctl::makeUnique<nc::Camera>());
		viewports_.pushBack(nctl::makeUnique<nc::Viewport>(vpSizes[i]));
		viewports_.back()->setRootNode(vpNodes_[i].get());
		viewports_.back()->setClearColor(vpClearColors[i]);
		vpSprites_.pushBack(nctl::makeUnique<nc::Sprite>(&rootNode, viewports_[i]->texture(), viewports_[i]->width() / 2, viewports_[i]->height() / 2));
		vpSprites_.back()->setLayer(1000);
	}
	rootViewport.setNextViewport(viewports_[0].get());
	rootViewport.initTexture(128, 128);
	viewports_[0]->setNextViewport(viewports_[1].get());
	vpSprites_[1]->setPosition(nc::theApplication().widthInt() - viewports_[1]->width() / 2, nc::theApplication().heightInt() - viewports_[1]->height() / 2);
	viewports_[0]->setCamera(cameras_[0].get());

	for (unsigned int i = 0; i < NumSprites / 2; i++)
	{
		const float randomX = nc::random().real(-ViewHalfWidth, ViewHalfWidth);
		const float randomY = nc::random().real(-ViewHalfHeight, ViewHalfHeight);
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(cameraNode_.get(), textures_[i % NumTextures].get(), randomX, randomY));
		sprites_.back()->setScale(0.5f);
		spritePos_.emplaceBack(randomX, randomY);
		// sprites_.back()->setLayer(i); // Fixes Z-fighting at the expense of batching
	}

	for (unsigned int i = 0; i < NumSprites / 4; i++)
	{
		const float randomX = nc::random().real(-ViewHalfWidth, ViewHalfWidth);
		const float randomY = nc::random().real(-ViewHalfHeight, ViewHalfHeight);
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(vpNodes_[0].get(), textures_[i % NumTextures].get(), randomX, randomY));
		sprites_.back()->setScale(0.5f);
		spritePos_.emplaceBack(randomX, randomY);
		// sprites_.back()->setLayer(i); // Fixes Z-fighting at the expense of batching
	}

	for (unsigned int i = 0; i < NumSprites / 4; i++)
	{
		const float randomX = nc::random().real(-ViewHalfWidth, ViewHalfWidth);
		const float randomY = nc::random().real(-ViewHalfHeight, ViewHalfHeight);
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(vpNodes_[1].get(), textures_[i % NumTextures].get(), randomX, randomY));
		sprites_.back()->setScale(0.5f);
		spritePos_.emplaceBack(randomX, randomY);
		// sprites_.back()->setLayer(i); // Fixes Z-fighting at the expense of batching
	}

	pause_ = false;
	angle_ = 0.0f;
	resetCamera();

	scrollOrigin_ = nc::Vector2f::Zero;
	scrollMove_ = nc::Vector2f::Zero;
	scrollOrigin2_ = nc::Vector2f::Zero;
	scrollMove2_ = nc::Vector2f::Zero;
	joyVectorLeft_ = nc::Vector2f::Zero;
	joyVectorRight_ = nc::Vector2f::Zero;
}

void MyEventHandler::onFrameStart()
{
	ImGui::Begin("Viewports");

	static nc::Vector2i size = viewports_[0]->size();
	static nc::Recti viewportRect = viewports_[0]->viewportRect();
	static int comboColorFormatType = static_cast<int>(viewports_[0]->colorFormat());
	static int comboDepthFormatType = static_cast<int>(viewports_[0]->depthStencilFormat());

	nc::Vector2i currentSize = viewports_[0]->size();
	nc::Recti currentViewportRect = viewports_[0]->viewportRect();
	int currentColorFormatType = static_cast<int>(viewports_[0]->colorFormat());
	int currentDepthFormatType = static_cast<int>(viewports_[0]->depthStencilFormat());

	ImGui::SliderInt("Width", &size.x, 0, nc::theApplication().widthInt());
	ImGui::SliderInt("Height", &size.y, 0, nc::theApplication().heightInt());
	ImGui::Combo("Color Format", &comboColorFormatType, ColorFormatLabels, IM_ARRAYSIZE(ColorFormatLabels));
	ImGui::Combo("Depth Format", &comboDepthFormatType, DepthFormatLabels, IM_ARRAYSIZE(DepthFormatLabels));

	ImGui::SliderInt("Rect X", &viewportRect.x, 0, nc::theApplication().widthInt());
	ImGui::SliderInt("Rect Y", &viewportRect.y, 0, nc::theApplication().heightInt());
	ImGui::SliderInt("Rect Width", &viewportRect.w, 0, nc::theApplication().widthInt() - viewportRect.x);
	ImGui::SliderInt("Rect Height", &viewportRect.h, 0, nc::theApplication().heightInt() - viewportRect.y);

	if (ImGui::Button("Current"))
	{
		size = viewports_[0]->size();
		comboColorFormatType = static_cast<int>(viewports_[0]->colorFormat());
		comboDepthFormatType = static_cast<int>(viewports_[0]->depthStencilFormat());
	}
	ImGui::SameLine();
	if (ImGui::Button("Apply"))
	{
		if (!(size == currentSize) ||
		    comboColorFormatType != currentColorFormatType ||
		    comboDepthFormatType != currentDepthFormatType)
		{
			viewports_[0]->initTexture(size, nc::Viewport::ColorFormat(comboColorFormatType), nc::Viewport::DepthStencilFormat(comboDepthFormatType));
		}

		if (!(viewportRect == currentViewportRect) && viewportRect.w > 0 && viewportRect.h > 0)
			viewports_[0]->setViewportRect(viewportRect);
	}

	static nc::Rectf cameraRect(0.0f, 0.0f, nc::theApplication().width(), nc::theApplication().height());
	ImGui::SliderFloat("Camera X", &cameraRect.x, 0.0f, nc::theApplication().width());
	ImGui::SliderFloat("Camera Y", &cameraRect.y, 0.0f, nc::theApplication().height());
	ImGui::SliderFloat("Camera Width", &cameraRect.w, 0.0f, nc::theApplication().width() - cameraRect.x);
	ImGui::SliderFloat("Camera Height", &cameraRect.h, 0.0f, nc::theApplication().height() - cameraRect.y);
	if (ImGui::Button("Apply ortho"))
		cameras_[0]->setOrtoProjection(cameraRect, -1.0f, 1.0f);

	ImGui::End();

	const float interval = nc::theApplication().interval();
	if (!pause_)
		angle_ += 2.0f * interval;

	const nc::KeyboardState &keyState = nc::theApplication().inputManager().keyboardState();

	if (keyState.isKeyDown(nc::KeySym::D))
		camPos_.x -= MoveSpeed * interval;
	else if (keyState.isKeyDown(nc::KeySym::A))
		camPos_.x += MoveSpeed * interval;
	if (keyState.isKeyDown(nc::KeySym::W))
		camPos_.y -= MoveSpeed * interval;
	else if (keyState.isKeyDown(nc::KeySym::S))
		camPos_.y += MoveSpeed * interval;

	if (keyState.isKeyDown(nc::KeySym::RIGHT))
		camRot_ += RotateSpeed * interval;
	else if (keyState.isKeyDown(nc::KeySym::LEFT))
		camRot_ -= RotateSpeed * interval;

	if (keyState.isKeyDown(nc::KeySym::UP))
		camScale_ += ScaleSpeed * interval;
	else if (keyState.isKeyDown(nc::KeySym::DOWN))
		camScale_ -= ScaleSpeed * interval;

	if (joyVectorLeft_.length() > nc::IInputManager::LeftStickDeadZone)
	{
		camPos_.x -= joyVectorLeft_.x * MoveSpeed * interval;
		camPos_.y -= joyVectorLeft_.y * MoveSpeed * interval;
	}
	if (joyVectorRight_.length() > nc::IInputManager::RightStickDeadZone)
	{
		camRot_ += joyVectorRight_.x * RotateSpeed * interval;
		camScale_ += joyVectorRight_.y * ScaleSpeed * interval;
	}

	const nc::Vector2f scrollDiff = scrollMove_ - scrollOrigin_;
	if (scrollDiff.sqrLength() > 2.0f)
	{
		camPos_.x += scrollDiff.x;
		camPos_.y += scrollDiff.y;
		scrollOrigin_ = scrollMove_;
	}
	const nc::Vector2f scrollDiff2 = scrollMove2_ - scrollOrigin2_;
	if (scrollDiff2.sqrLength() > 2.0f)
	{
		camRot_ += scrollDiff2.x * 0.1f;
		camScale_ += scrollDiff2.y * 0.001f;
		scrollOrigin2_ = scrollMove2_;
	}

	if (camScale_ > MaxCameraScale)
		camScale_ = MaxCameraScale;
	else if (camScale_ < MinCameraScale)
		camScale_ = MinCameraScale;

	const float scaledWidth = ViewHalfWidth * 2.0f * camScale_;
	const float scaledHeight = ViewHalfHeight * 2.0f * camScale_;

	float rotatedWidth = scaledWidth;
	float rotatedHeight = scaledHeight;
	if (camRot_ > 0.01f || camRot_ < -0.01f)
	{
		camRot_ = fmodf(camRot_, 360.0f);
		const float sinRot = sinf(camRot_ * nc::fDegToRad);
		const float cosRot = cosf(camRot_ * nc::fDegToRad);
		rotatedWidth = fabsf(scaledHeight * sinRot) + fabsf(scaledWidth * cosRot);
		rotatedHeight = fabsf(scaledWidth * sinRot) + fabsf(scaledHeight * cosRot);
	}

	const float maxX = rotatedWidth * 0.5f;
	const float minX = (-rotatedWidth * 0.5f) + nc::theApplication().width();
	const float maxY = rotatedHeight * 0.5f;
	const float minY = (-rotatedHeight * 0.5f) + nc::theApplication().height();

	if (camPos_.x > maxX)
		camPos_.x = maxX;
	if (camPos_.x < minX)
		camPos_.x = minX;
	if (camPos_.y > maxY)
		camPos_.y = maxY;
	if (camPos_.y < minY)
		camPos_.y = minY;

	const nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
	debugString_->clear();
	debugString_->format("x: %.2f, y: %.2f, scale: %.2f, angle: %.2f", -camPos_.x, -camPos_.y, camScale_, camRot_);
	debugString_->formatAppend("\nbatching: %s, culling: %s", settings.batchingEnabled ? "on" : "off", settings.cullingEnabled ? "on" : "off");
	debugText_->setString(*debugString_);

	cameraNode_->setPosition(camPos_);
	cameraNode_->setRotation(camRot_);
	cameraNode_->setScale(camScale_);

	for (unsigned int i = 0; i < NumSprites; i++)
	{
		const float t = i / static_cast<float>(NumSprites);
		const float scaleX = 50.0f * (2.0f * t - 1.0f);
		const float scaleY = 50.0f * (-2.0f * t + 1.0f);
		const float moveX = scaleX * sinf(angle_) * cosf(angle_ * 0.5f * t);
		const float moveY = scaleY * sinf(angle_ * 0.5f * t) * cosf(angle_);
		sprites_[i]->setPosition(spritePos_[i].x + moveX, spritePos_[i].y + moveY);
	}
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

	if (event.count > 1)
	{
		scrollMove2_.x = event.pointers[1].x;
		scrollMove2_.y = event.pointers[1].y;
	}
}

void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	if (lastClickTime_.secondsSince() < DoubleClickDelay)
		resetCamera();
	lastClickTime_ = nc::TimeStamp::now();
}

void MyEventHandler::onPointerDown(const nc::TouchEvent &event)
{
	if (event.count == 2)
	{
		scrollOrigin2_.x = event.pointers[1].x;
		scrollOrigin2_.y = event.pointers[1].y;
		scrollMove2_ = scrollOrigin2_;
	}
}
#endif

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();
	nc::IDebugOverlay::DisplaySettings &overlaySettings = nc::theApplication().debugOverlaySettings();

	if (event.sym == nc::KeySym::B)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.sym == nc::KeySym::C)
		renderingSettings.cullingEnabled = !renderingSettings.cullingEnabled;
	else if (event.sym == nc::KeySym::H)
	{
		overlaySettings.showProfilerGraphs = !overlaySettings.showProfilerGraphs;
		overlaySettings.showInfoText = !overlaySettings.showInfoText;
	}
	else if (event.sym == nc::KeySym::BACKQUOTE)
		overlaySettings.showInterface = !overlaySettings.showInterface;
	else if (event.sym == nc::KeySym::P)
		pause_ = !pause_;
	else if (event.sym == nc::KeySym::R)
		resetCamera();
	else if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
	else if (event.sym == nc::KeySym::SPACE)
	{
		const bool isSuspended = nc::theApplication().isSuspended();
		nc::theApplication().setSuspended(!isSuspended);
	}
}

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
	{
		scrollOrigin_.x = static_cast<float>(event.x);
		scrollOrigin_.y = static_cast<float>(event.y);
		scrollMove_ = scrollOrigin_;

		checkClick(static_cast<float>(event.x), static_cast<float>(event.y));
	}
	else if (event.isRightButton())
	{
		scrollOrigin2_.x = static_cast<float>(event.x);
		scrollOrigin2_.y = static_cast<float>(event.y);
		scrollMove2_ = scrollOrigin2_;
	}
}

void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
	{
		if (lastClickTime_.secondsSince() < DoubleClickDelay)
			resetCamera();
		lastClickTime_ = nc::TimeStamp::now();
	}
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state)
{
	if (state.isLeftButtonDown())
	{
		scrollMove_.x = static_cast<float>(state.x);
		scrollMove_.y = static_cast<float>(state.y);
	}
	else if (state.isRightButtonDown())
	{
		scrollMove2_.x = static_cast<float>(state.x);
		scrollMove2_.y = static_cast<float>(state.y);
	}
}

void MyEventHandler::onScrollInput(const nc::ScrollEvent &event)
{
	camRot_ += 10.0f * event.x;
	camScale_ += 0.1f * event.y;
}

void MyEventHandler::onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event)
{
	if (event.axisName == nc::AxisName::LX)
		joyVectorLeft_.x = event.value;
	else if (event.axisName == nc::AxisName::LY)
		joyVectorLeft_.y = -event.value;

	if (event.axisName == nc::AxisName::RX)
		joyVectorRight_.x = event.value;
	else if (event.axisName == nc::AxisName::RY)
		joyVectorRight_.y = -event.value;
}

void MyEventHandler::onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();
	nc::IDebugOverlay::DisplaySettings &overlaySettings = nc::theApplication().debugOverlaySettings();

	if (event.buttonName == nc::ButtonName::A)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.buttonName == nc::ButtonName::Y)
		renderingSettings.cullingEnabled = !renderingSettings.cullingEnabled;
	else if (event.buttonName == nc::ButtonName::BACK)
	{
		overlaySettings.showProfilerGraphs = !overlaySettings.showProfilerGraphs;
		overlaySettings.showInfoText = !overlaySettings.showInfoText;
	}
	else if (event.buttonName == nc::ButtonName::START)
		pause_ = !pause_;
	else if (event.buttonName == nc::ButtonName::B)
		resetCamera();
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
	camPos_.x = nc::theApplication().width() * 0.5f;
	camPos_.y = nc::theApplication().height() * 0.5f;
	camRot_ = 0.0f;
	camScale_ = 1.0f;
}

void MyEventHandler::checkClick(float x, float y)
{
	const nc::Rectf debugTextRect = nc::Rectf::fromCenterAndSize(debugText_->absPosition(), debugText_->absSize());

#ifdef __ANDROID__
	// Make it slightly easier to touch on Android
	if (debugTextRect.contains(x, y))
#else
	if (debugTextRect.contains(x, y) && y <= debugTextRect.y + debugTextRect.h * 0.5f)
#endif
	{
		nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
		const float xPos = x - debugTextRect.x;
		if (xPos <= debugTextRect.w * 0.33f)
			settings.batchingEnabled = !settings.batchingEnabled;
		else if (xPos >= debugTextRect.w * 0.33f && xPos <= debugTextRect.w * 0.6f)
			settings.cullingEnabled = !settings.cullingEnabled;
	}
}
