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
const char *ClearModeLabels[] = { "Every frame", "This frame only", "Next frame only", "Never" };
nctl::String comboString(64);

nc::Vector2i viewportSize;
nc::Recti viewportRect;

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

	cameras_.pushBack(nctl::makeUnique<nc::Camera>());
	nc::theApplication().rootViewport().setCamera(cameras_.back().get());
	nc::Vector2i vpSizes[NumViewports] = { nc::Vector2i(640, 360), nc::Vector2i(320, 180) };
	nc::Colorf vpClearColors[NumViewports] = { nc::Colorf(0.25f, 0.25f, 0.25f, 0.25f), nc::Colorf(0.15f, 0.15f, 0.15f, 0.15f) };
	for (unsigned int i = 0; i < NumViewports; i++)
	{
		vpNodes_.pushBack(nctl::makeUnique<nc::SceneNode>());
		cameras_.pushBack(nctl::makeUnique<nc::Camera>());
		viewports_.pushBack(nctl::makeUnique<nc::Viewport>(vpSizes[i]));
		viewports_.back()->setRootNode(vpNodes_.back().get());
		viewports_.back()->setCamera(cameras_.back().get());
		viewports_.back()->setClearColor(vpClearColors[i]);
		vpSprites_.pushBack(nctl::makeUnique<nc::Sprite>(&rootNode, viewports_.back()->texture(), viewports_.back()->width() / 2, viewports_.back()->height() / 2));
		vpSprites_.back()->setLayer(1000);
	}
	rootViewport.setNextViewport(viewports_[0].get());
	viewports_[0]->setNextViewport(viewports_[1].get());
	vpSprites_[1]->setPosition(nc::theApplication().widthInt() - viewports_[1]->width() / 2, nc::theApplication().heightInt() - viewports_[1]->height() / 2);

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

	comboString.clear();
	comboString.append("Screen");
	comboString.setLength(comboString.length() + 1);
	for (unsigned int i = 0; i < NumViewports; i++)
	{
		comboString.formatAppend("Viewport %d", i);
		comboString.setLength(comboString.length() + 1);
	}
	comboString.setLength(comboString.length() + 1);
	// Append a second '\0' to signal the end of the combo item list
	comboString[comboString.length() - 1] = '\0';

	static int currentComboViewport = 0;
	const bool viewportChanged = ImGui::Combo("Viewport", &currentComboViewport, comboString.data());
	nc::Viewport &currentViewport = (currentComboViewport > 0) ? *viewports_[currentComboViewport - 1] : nc::theApplication().rootViewport();

	const nc::Vector2i currentViewportSize = currentViewport.size();
	const nc::Recti currentViewportRect = currentViewport.viewportRect();
	const int currentColorFormatType = static_cast<int>(currentViewport.colorFormat());
	const int currentDepthFormatType = static_cast<int>(currentViewport.depthStencilFormat());

	if (currentComboViewport == 0)
	{
		if (ImGui::TreeNodeEx("Surface", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Width: %d", currentViewportSize.x);
			ImGui::Text("Height: %d", currentViewportSize.y);
			ImGui::Text("Color Format: %s", ColorFormatLabels[currentColorFormatType]);
			ImGui::Text("Depth Format: %s", DepthFormatLabels[currentDepthFormatType]);
			ImGui::TreePop();
		}
	}
	else
	{
		if (ImGui::TreeNodeEx("Surface", ImGuiTreeNodeFlags_DefaultOpen))
		{
			viewportSize = currentViewport.size();
			int comboColorFormatType = static_cast<int>(currentViewport.colorFormat());
			int comboDepthFormatType = static_cast<int>(currentViewport.depthStencilFormat());

			ImGui::SliderInt("Width", &viewportSize.x, 0, nc::theApplication().widthInt());
			ImGui::SliderInt("Height", &viewportSize.y, 0, nc::theApplication().heightInt());
			ImGui::Combo("Color Format", &comboColorFormatType, ColorFormatLabels, IM_ARRAYSIZE(ColorFormatLabels));
			ImGui::Combo("Depth Format", &comboDepthFormatType, DepthFormatLabels, IM_ARRAYSIZE(DepthFormatLabels));

			if (ImGui::Button("Current"))
			{
				viewportSize = currentViewport.size();
				comboColorFormatType = static_cast<int>(currentViewport.colorFormat());
				comboDepthFormatType = static_cast<int>(currentViewport.depthStencilFormat());
			}
			ImGui::SameLine();
			if (ImGui::Button("Apply"))
			{
				if (!(viewportSize == currentViewportSize) ||
				    comboColorFormatType != currentColorFormatType ||
				    comboDepthFormatType != currentDepthFormatType)
				{
					currentViewport.initTexture(viewportSize, nc::Viewport::ColorFormat(comboColorFormatType), nc::Viewport::DepthStencilFormat(comboDepthFormatType));
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Viewport Rectangle"))
		{
			static nc::Recti viewportRect = currentViewport.viewportRect();

			ImGui::SliderInt("Rect X", &viewportRect.x, 0, currentViewport.width());
			ImGui::SliderInt("Rect Y", &viewportRect.y, 0, currentViewport.height());
			ImGui::SliderInt("Rect Width", &viewportRect.w, 0, currentViewport.width() - viewportRect.x);
			ImGui::SliderInt("Rect Height", &viewportRect.h, 0, currentViewport.height() - viewportRect.y);

			if (ImGui::Button("Current"))
				viewportRect = currentViewport.viewportRect();
			ImGui::SameLine();
			if (ImGui::Button("Apply"))
			{
				if (!(viewportRect == currentViewportRect) && viewportRect.w > 0 && viewportRect.h > 0)
					currentViewport.setViewportRect(viewportRect);
			}

			ImGui::TreePop();
		}
	}

	if (currentComboViewport > 0 && ImGui::TreeNode("Sprite"))
	{
		nc::Sprite &sprite = *vpSprites_[currentComboViewport - 1];

		bool isEnabled = sprite.isEnabled();
		ImGui::Checkbox("Enabled", &isEnabled);
		sprite.setEnabled(isEnabled);

		ImGui::SliderFloat("Pos X", &sprite.x, currentViewport.width() / 2, nc::theApplication().widthInt() - currentViewport.width() / 2);
		ImGui::SliderFloat("Pos Y", &sprite.y, currentViewport.height() / 2, nc::theApplication().heightInt() - currentViewport.height() / 2);
		float rotation = sprite.rotation();
		ImGui::SliderFloat("Rotation", &rotation, 0.0f, 360.0f);
		nc::Vector2f scaleFactors = sprite.scale();
		ImGui::SliderFloat2("Scale", scaleFactors.data(), 0.01f, 3.0f);
		if (ImGui::Button("Reset"))
		{
			sprite.x = currentViewport.width() / 2;
			sprite.y = currentViewport.height() / 2;
			rotation = 0.0f;
			scaleFactors.set(1.0f, 1.0f);
		}
		sprite.setRotation(rotation);
		sprite.setScale(scaleFactors);

		ImGui::TreePop();
	}

	nc::Colorf clearColor = currentViewport.clearColor();
	ImGui::ColorEdit4("Clear Color", clearColor.data(), ImGuiColorEditFlags_AlphaBar);
	currentViewport.setClearColor(clearColor);

	int comboClearMode = static_cast<int>(currentViewport.clearMode());
	ImGui::Combo("Clear Mode", &comboClearMode, ClearModeLabels, IM_ARRAYSIZE(ClearModeLabels));
	currentViewport.setClearMode(nc::Viewport::ClearMode(comboClearMode));

	const char *nextViewportString = "Next Viewport";
	if (currentViewport.nextViewport() == nullptr)
		ImGui::Text("%s: None", nextViewportString);
	else if (currentViewport.nextViewport() == &nc::theApplication().rootViewport())
		ImGui::Text("%s: Screen", nextViewportString);
	else
	{
		for (unsigned int i = 0; i < NumViewports; i++)
		{
			if (currentViewport.nextViewport() == viewports_[i].get())
			{
				ImGui::Text("%s: Viewport %d", nextViewportString, i);
				break;
			}
		}
	}

	nc::Camera &currentCamera = *currentViewport.camera();
	if (ImGui::TreeNode("Projection Matrix"))
	{
		static nc::Camera::ProjectionValues values = currentCamera.projectionValues();
		if (viewportChanged)
			values = currentCamera.projectionValues();
		static bool applyEveryframe = false;
		bool valueChanged = false;

		ImGui::Text("Update Frame: %lu", currentCamera.updateFrameProjectionMatrix());
		valueChanged |= ImGui::SliderFloat("Left", &values.left, 0.0f, static_cast<float>(currentViewport.width()));
		valueChanged |= ImGui::SliderFloat("Right", &values.right, values.left, static_cast<float>(currentViewport.width()));
		valueChanged |= ImGui::SliderFloat("Top", &values.top, 0.0f, static_cast<float>(currentViewport.height()));
		valueChanged |= ImGui::SliderFloat("Bottom", &values.bottom, values.top, static_cast<float>(currentViewport.height()));
		ImGui::Checkbox("Apply Every Frame", &applyEveryframe);
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			values.left = 0.0f;
			values.right = static_cast<float>(currentViewport.width());
			values.top = 0.0f;
			values.bottom = static_cast<float>(currentViewport.height());
			valueChanged = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Apply") || (applyEveryframe && valueChanged))
			currentCamera.setOrthoProjection(values);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("View Matrix"))
	{
		static nc::Camera::ViewValues values = currentCamera.viewValues();
		if (viewportChanged)
			values = currentCamera.viewValues();
		static bool applyEveryframe = false;
		bool valueChanged = false;

		ImGui::Text("Update Frame: %lu", currentCamera.updateFrameViewMatrix());
		valueChanged |= ImGui::SliderFloat("Position X", &values.x, 0.0f, nc::theApplication().width());
		valueChanged |= ImGui::SliderFloat("Position Y", &values.y, 0.0f, nc::theApplication().height());
		valueChanged |= ImGui::SliderFloat("Rotation", &values.rotation, 0.0f, 360.0f);
		valueChanged |= ImGui::SliderFloat("Scale", &values.scale, 0.01f, 3.0f);
		ImGui::Checkbox("Apply Every Frame", &applyEveryframe);
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			values.x = 0.0f;
			values.y = 0.0f;
			values.rotation = 0.0f;
			values.scale = 1.0f;
			valueChanged = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Apply") || (applyEveryframe && valueChanged))
			currentCamera.setView(values);
		ImGui::TreePop();
	}

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
