#include <ncine/config.h>
#include <ncine/common_constants.h>
#include <ncine/imgui.h>

#include "apptest_viewports.h"
#include <ncine/Random.h>
#include <ncine/Application.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/TextNode.h>
#include <ncine/Viewport.h>
#include <ncine/Camera.h>
#include <ncine/TextureSaverPng.h>
#if NCINE_WITH_WEBP
	#include <ncine/TextureSaverWebP.h>
#endif
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

const char *ColorFormatLabels[] = { "R8", "RG8", "RGB8", "RGBA8" };
const char *DepthFormatLabels[] = { "None", "Depth16", "Depth24", "Depth24_Stencil8" };
const char *ClearModeLabels[] = { "Every draw", "Every frame", "This frame only", "Next frame only", "Never" };
const char *PositionPresetsLabels[] = { "Top Left", "Top Centre", "Top Right", "Centre Left", "Centre", "Centre Right", "Bottom Left", "Bottom Centre", "Bottom Right" };
nctl::String comboString(64);

nc::Texture::Format screenFormat = nc::Texture::Format::RGB8;
bool showImGui = true;

const char *stringOnOff(bool enabled)
{
	return enabled ? "on" : "off";
}

nc::Vector2f positionPresets(const nc::Vector2f &spriteSize, unsigned int posIndex)
{
	const float screenWidth = nc::theApplication().width();
	const float screenHeight = nc::theApplication().height();
	const nc::Vector2f centreCamera(screenWidth * 0.5f, screenHeight * 0.5f);

	switch (posIndex)
	{
		case 0: return nc::Vector2f(spriteSize.x * 0.5f, screenHeight - spriteSize.y * 0.5) - centreCamera;
		case 1: return nc::Vector2f(screenWidth * 0.5f, screenHeight - spriteSize.y * 0.5) - centreCamera;
		case 2: return nc::Vector2f(screenWidth - spriteSize.x * 0.5f, screenHeight - spriteSize.y * 0.5) - centreCamera;
		case 3: return nc::Vector2f(spriteSize.x * 0.5f, screenHeight * 0.5) - centreCamera;
		default:
		case 4: return nc::Vector2f(screenWidth * 0.5f, screenHeight * 0.5) - centreCamera;
		case 5: return nc::Vector2f(screenWidth - spriteSize.x * 0.5f, screenHeight * 0.5) - centreCamera;
		case 6: return nc::Vector2f(spriteSize.x * 0.5f, spriteSize.y * 0.5f) - centreCamera;
		case 7: return nc::Vector2f(screenWidth * 0.5f, spriteSize.y * 0.5f) - centreCamera;
		case 8: return nc::Vector2f(screenWidth - spriteSize.x * 0.5f, spriteSize.y * 0.5f) - centreCamera;
	}
}

void initTexture(nc::Viewport &viewport, nc::Texture &texture, const nc::Vector2i &size, nc::Texture::Format colorFormat, nc::Viewport::DepthStencilFormat depthStencilFormat)
{
	viewport.removeAllTextures();
	texture.init(nullptr, colorFormat, size);
	viewport.setTexture(&texture);
	viewport.setDepthStencilFormat(depthStencilFormat);
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
	nc::SceneNode &rootNode = nc::theApplication().rootNode();
	nc::Viewport &screenViewport = nc::theApplication().screenViewport();

	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture1File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture2File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture3File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture4File)).data()));

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());

	debugString_ = nctl::makeUnique<nctl::String>(128);
	debugText_ = nctl::makeUnique<nc::TextNode>(&rootNode, font_.get());
	debugText_->setPosition((-debugText_->width()) * 0.5f, nc::theApplication().height() * 0.5f - debugText_->lineHeight() * 0.5f * 2.0f);
	debugText_->setColor(255, 255, 0, 255);
	debugText_->setAlignment(nc::TextNode::Alignment::CENTER);

	viewportCreationData[0].withTexture = false;
	viewportCreationData[0].rect = nc::Recti(0, nc::theApplication().height() * 0.5f, nc::theApplication().width(), nc::theApplication().height() * 0.5f);
	viewportCreationData[1].assignRootNode = false;
	viewportCreationData[1].withTexture = false;
	viewportCreationData[1].rect = nc::Recti(0, 0, nc::theApplication().width(), nc::theApplication().height() * 0.5f);
	viewportCreationData[2].rect = nc::Recti(0, 0, nc::theApplication().width() * 0.5f, nc::theApplication().height() * 0.5f);
	viewportCreationData[2].clearColor = nc::Colorf(0.25f, 0.25f, 0.25f, 0.25f);
	viewportCreationData[2].spritePositionIndex = 6;
	viewportCreationData[3].rect = nc::Recti(0, 0, nc::theApplication().width() * 0.25f, nc::theApplication().height() * 0.25f);
	viewportCreationData[3].clearColor = nc::Colorf(0.15f, 0.15f, 0.15f, 0.15f);
	viewportCreationData[3].spritePositionIndex = 2;
	viewportCreationData[4].assignCamera = false;
	viewportCreationData[4].withTexture = false;

	screenCamera_ = nctl::makeUnique<nc::Camera>();
	screenViewport.setCamera(screenCamera_.get());
	for (unsigned int i = 0; i < NumViewports; i++)
	{
		if (viewportCreationData[i].withTexture)
		{
			const nc::Recti &rect = viewportCreationData[i].rect;
			viewportData[i].texture = nctl::makeUnique<nc::Texture>(nullptr, nc::Texture::Format::RGB8, rect.w, rect.h);
			viewportData[i].viewport = nctl::makeUnique<nc::Viewport>(viewportData[i].texture.get(), nc::Viewport::DepthStencilFormat::DEPTH24_STENCIL8);
		}
		else
		{
			viewportData[i].viewport = nctl::makeUnique<nc::Viewport>();
			const nc::Viewport &prevViewport = (i > 0) ? *viewportData[i - 1].viewport : screenViewport;
			viewportData[i].viewport->setViewportRect(prevViewport.viewportRect());
		}

		viewportData[i].camera = nctl::makeUnique<nc::Camera>();
		viewportData[i].camera->setView(-nc::theApplication().width() * 0.5f, -nc::theApplication().height() * 0.5f, 0.0f, 1.0f);
		viewportData[i].rootNode = nctl::makeUnique<nc::SceneNode>();

		nc::Viewport *viewport = viewportData[i].viewport.get();

		if (viewportCreationData[i].rect.w != 0 && viewportCreationData[i].rect.h != 0)
			viewport->setViewportRect(viewportCreationData[i].rect);

		if (viewportCreationData[i].withTexture == false)
		{
			const nc::Recti &vpRect = viewportData[i].viewport->viewportRect();
			nc::Camera::ProjectionValues projValues(0, vpRect.w, 0, vpRect.h);
			viewportData[i].camera->setOrthoProjection(projValues);
		}

		if (viewportCreationData[i].assignCamera)
			viewport->setCamera(viewportData[i].camera.get());
		else
		{
			nc::Camera *camera = (i > 0) ? viewportData[i - 1].camera.get() : screenCamera_.get();
			viewport->setCamera(camera);
		}

		if (viewportCreationData[i].assignRootNode)
			viewport->setRootNode(viewportData[i].rootNode.get());
		else
		{
			nc::SceneNode *node = (i > 0) ? viewportData[i - 1].rootNode.get() : &rootNode;
			viewport->setRootNode(node);
		}

		if (viewportCreationData[i].rect.w != 0 && viewportCreationData[i].rect.h != 0)
		{
			viewport->setClearColor(viewportCreationData[i].clearColor);
			viewportData[i].sprite = nctl::makeUnique<nc::Sprite>(&rootNode, viewport->texture(), viewport->width() / 2, viewport->height() / 2);
			nc::Sprite *sprite = viewportData[i].sprite.get();
			// Vertical flip as the texture is not loaded but generated by OpenGL and should not be flipped by shaders
			sprite->setFlippedY(true);
			sprite->setLayer(1000);
			sprite->setPosition(positionPresets(sprite->absSize(), viewportCreationData[i].spritePositionIndex));
		}
	}
	if (NumViewports > 0)
	{
		for (unsigned int i = 0; i < NumViewports; i++)
			nc::Viewport::chain().pushBack(viewportData[i].viewport.get());
	}

	for (unsigned int i = 0; i < NumSprites; i++)
	{
		const int index = (NumViewports > 0) ? (i % NumViewports) : 0;
		nc::SceneNode *node = (NumViewports > 0) ? viewportData[index].rootNode.get() : &rootNode;
		const float randomX = nc::random().real(-nc::theApplication().width(), nc::theApplication().width());
		const float randomY = nc::random().real(-nc::theApplication().height(), nc::theApplication().height());
		const int texIndex = (index <= 1) ? 0 : (index - 1) % NumTextures; // use the same texture for the spit screen viewports
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(node, textures_[texIndex].get(), randomX, randomY));
		sprites_.back()->setScale(0.5f);
		spritePos_.emplaceBack(randomX, randomY);
	}

	pause_ = false;
	inputEnabled_ = true;
	angle_ = 0.0f;
	resetCamera();

	scrollOrigin_ = nc::Vector2f::Zero;
	scrollMove_ = nc::Vector2f::Zero;
	scrollOrigin2_ = nc::Vector2f::Zero;
	scrollMove2_ = nc::Vector2f::Zero;

	const nc::DisplayMode displayMode = nc::theApplication().gfxDevice().displayMode();
	if (displayMode.alphaBits() == 8)
		screenFormat = nc::Texture::Format::RGBA8;
}

void MyEventHandler::onFrameStart()
{
	static bool viewMatrixChanged = false;
	static int currentComboViewport = 0;

	if (showImGui)
	{
		ImGui::SetNextWindowPos(ImVec2(30.0f, 40.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(350.0f, 350.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Viewports", &showImGui))
		{
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

			const bool viewportChanged = ImGui::Combo("Viewport", &currentComboViewport, comboString.data());
			nc::Viewport &currentViewport = (currentComboViewport > 0) ? *viewportData[currentComboViewport - 1].viewport : nc::theApplication().screenViewport();
			nc::Texture *currentTexture = (currentComboViewport > 0) ? viewportData[currentComboViewport - 1].texture.get() : nullptr;

			const nc::Viewport *nextViewport = (currentComboViewport < nc::Viewport::chain().size()) ? nc::Viewport::chain()[currentComboViewport] : nullptr;
			const char *nextViewportString = "Next Viewport";
			if (nextViewport == nullptr)
				ImGui::Text("%s: None", nextViewportString);
			else
			{
				for (unsigned int i = 0; i < NumViewports; i++)
				{
					if (nextViewport == viewportData[i].viewport.get())
					{
						ImGui::Text("%s: Viewport %d", nextViewportString, i);
						break;
					}
				}
			}

			if ((currentComboViewport == 0 || currentViewport.texture() != nullptr) &&
			    ImGui::TreeNodeEx("Surface", ImGuiTreeNodeFlags_DefaultOpen))
			{
				const nc::Vector2i currentViewportSize = currentViewport.size();
				const int currentColorFormatType = static_cast<int>((currentTexture != nullptr) ? currentTexture->format() : screenFormat);
				const int currentDepthFormatType = static_cast<int>(currentViewport.depthStencilFormat());

				if (currentComboViewport == 0)
				{
					ImGui::Text("Width: %d", currentViewportSize.x);
					ImGui::Text("Height: %d", currentViewportSize.y);
					ImGui::Text("Color Format: %s", ColorFormatLabels[currentColorFormatType]);
					ImGui::Text("Depth Format: %s", DepthFormatLabels[currentDepthFormatType]);
				}
				else
				{
					static nc::Vector2i viewportSize = currentViewport.size();
					static int comboColorFormatType = static_cast<int>(currentTexture->format());
					static int comboDepthFormatType = static_cast<int>(currentViewport.depthStencilFormat());
					if (viewportChanged)
					{
						viewportSize = currentViewport.size();
						comboColorFormatType = static_cast<int>(currentTexture->format());
						comboDepthFormatType = static_cast<int>(currentViewport.depthStencilFormat());
					}

					ImGui::SliderInt("Width", &viewportSize.x, 0, nc::theApplication().widthInt());
					ImGui::SliderInt("Height", &viewportSize.y, 0, nc::theApplication().heightInt());
					ImGui::Combo("Color Format", &comboColorFormatType, ColorFormatLabels, IM_ARRAYSIZE(ColorFormatLabels));
					ImGui::Combo("Depth Format", &comboDepthFormatType, DepthFormatLabels, IM_ARRAYSIZE(DepthFormatLabels));

					if (ImGui::Button("Current"))
					{
						viewportSize = currentViewport.size();
						comboColorFormatType = static_cast<int>(currentTexture->format());
						comboDepthFormatType = static_cast<int>(currentViewport.depthStencilFormat());
					}
					ImGui::SameLine();
					if (ImGui::Button("Apply"))
					{
						if (!(viewportSize == currentViewportSize) ||
						    comboColorFormatType != currentColorFormatType ||
						    comboDepthFormatType != currentDepthFormatType)
						{
							initTexture(currentViewport, *currentTexture, viewportSize, nc::Texture::Format(comboColorFormatType), nc::Viewport::DepthStencilFormat(comboDepthFormatType));
							viewportData[currentComboViewport - 1].sprite->resetTexture();
						}
					}
#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)
					ImGui::SameLine();
					if (ImGui::Button("Save PNG"))
					{
						nc::TextureSaverPng saver;
						nc::TextureSaverPng::Properties props;
						props.width = currentViewportSize.x;
						props.height = currentViewportSize.y;
						props.format = nc::ITextureSaver::Format::RGB8;
						// Vertical flip as the texture is generated by OpenGL and saved from bottom to top
						props.verticalFlip = true;
						if (currentTexture->format() == nc::Texture::Format::RGBA8)
							props.format = nc::ITextureSaver::Format::RGBA8;

						nc::Texture &tex = *currentViewport.texture();
						nctl::UniquePtr<unsigned char[]> buffer = nctl::makeUnique<unsigned char[]>(tex.dataSize());
						const bool savedToMemory = tex.saveToMemory(buffer.get());
						if (savedToMemory)
						{
							props.pixels = buffer.get();
							// Recycling comboString for screenshot filename
							comboString.format("viewport%d_%dx%d.png", currentComboViewport, currentViewportSize.x, currentViewportSize.y);
							saver.saveToFile(props, comboString.data());
						}
					}
	#if NCINE_WITH_WEBP
					ImGui::SameLine();
					if (ImGui::Button("Save WebP"))
					{
						nc::TextureSaverWebP saver;
						nc::TextureSaverWebP::Properties props;
						nc::TextureSaverWebP::WebPProperties webpProps;
						props.width = currentViewportSize.x;
						props.height = currentViewportSize.y;
						props.format = nc::ITextureSaver::Format::RGB8;
						// Vertical flip as the texture is generated by OpenGL and saved from bottom to top
						props.verticalFlip = true;
						webpProps.lossless = true;
						if (currentTexture->format() == nc::Texture::Format::RGBA8)
							props.format = nc::ITextureSaver::Format::RGBA8;

						nc::Texture &tex = *currentViewport.texture();
						nctl::UniquePtr<unsigned char[]> buffer = nctl::makeUnique<unsigned char[]>(tex.dataSize());
						const bool savedToMemory = tex.saveToMemory(buffer.get());
						if (savedToMemory)
						{
							props.pixels = buffer.get();
							// Recycling comboString for screenshot filename
							comboString.format("viewport%d_%dx%d.webp", currentComboViewport, currentViewportSize.x, currentViewportSize.y);
							saver.saveToFile(props, webpProps, comboString.data());
						}
					}
	#endif
#endif
				}

				nc::Colorf clearColor = currentViewport.clearColor();
				ImGui::ColorEdit4("Clear Color", clearColor.data(), ImGuiColorEditFlags_AlphaBar);
				currentViewport.setClearColor(clearColor);

				int comboClearMode = static_cast<int>(currentViewport.clearMode());
				ImGui::Combo("Clear Mode", &comboClearMode, ClearModeLabels, IM_ARRAYSIZE(ClearModeLabels));
				currentViewport.setClearMode(nc::Viewport::ClearMode(comboClearMode));

				ImGui::TreePop();
			}

			int maxWidth = currentViewport.width();
			int maxHeight = currentViewport.height();
			if (maxWidth == 0 || maxHeight == 0)
			{
				for (int i = currentComboViewport - 1; i >= 0; i--)
				{
					nc::Viewport &prevViewport = (i == 0) ? nc::theApplication().screenViewport() : *viewportData[i].viewport;
					if (prevViewport.width() != 0 && prevViewport.height() != 0)
					{
						maxWidth = prevViewport.width();
						maxHeight = prevViewport.height();
						break;
					}
				}
			}

			if (currentComboViewport > 0 && ImGui::TreeNode("Viewport Rectangle"))
			{
				const nc::Recti currentViewportRect = currentViewport.viewportRect();
				static nc::Recti viewportRect = currentViewport.viewportRect();
				if (viewportChanged)
					viewportRect = currentViewport.viewportRect();
				static bool applyEveryframe = false;
				bool valueChanged = false;

				valueChanged |= ImGui::SliderInt("Rect X", &viewportRect.x, 0, maxWidth);
				valueChanged |= ImGui::SliderInt("Rect Y", &viewportRect.y, 0, maxHeight);
				valueChanged |= ImGui::SliderInt("Rect Width", &viewportRect.w, 0, maxWidth - viewportRect.x);
				valueChanged |= ImGui::SliderInt("Rect Height", &viewportRect.h, 0, maxHeight - viewportRect.y);

				if (viewportRect.w > maxWidth - viewportRect.x)
					viewportRect.w = maxWidth - viewportRect.x;
				if (viewportRect.h > maxHeight - viewportRect.y)
					viewportRect.h = maxHeight - viewportRect.y;

				if (ImGui::Checkbox("Apply Every Frame", &applyEveryframe))
					valueChanged = true;
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
				{
					viewportRect.x = 0;
					viewportRect.y = 0;
					viewportRect.w = maxWidth;
					viewportRect.h = maxHeight;
					valueChanged = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("Current"))
					viewportRect = currentViewport.viewportRect();
				ImGui::SameLine();
				if (ImGui::Button("Apply") || (applyEveryframe && valueChanged))
				{
					if (!(viewportRect == currentViewportRect) && viewportRect.w > 0 && viewportRect.h > 0)
						currentViewport.setViewportRect(viewportRect);
				}

				ImGui::TreePop();
			}

			if (currentComboViewport > 0 && ImGui::TreeNode("Scissor Rectangle"))
			{
				const nc::Recti currentScissorRect = currentViewport.scissorRect();
				static nc::Recti scissorRect = currentViewport.scissorRect();
				if (viewportChanged)
					scissorRect = currentViewport.scissorRect();
				static bool applyEveryframe = false;
				bool valueChanged = false;

				valueChanged |= ImGui::SliderInt("Rect X", &scissorRect.x, 0, maxWidth);
				valueChanged |= ImGui::SliderInt("Rect Y", &scissorRect.y, 0, maxHeight);
				valueChanged |= ImGui::SliderInt("Rect Width", &scissorRect.w, 0, maxWidth - scissorRect.x);
				valueChanged |= ImGui::SliderInt("Rect Height", &scissorRect.h, 0, maxHeight - scissorRect.y);

				if (scissorRect.w > maxWidth - scissorRect.x)
					scissorRect.w = maxWidth - scissorRect.x;
				if (scissorRect.h > maxHeight - scissorRect.y)
					scissorRect.h = maxHeight - scissorRect.y;

				if (ImGui::Checkbox("Apply Every Frame", &applyEveryframe))
					valueChanged = true;
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
				{
					scissorRect.x = 0;
					scissorRect.y = 0;
					scissorRect.w = maxWidth;
					scissorRect.h = maxHeight;
					valueChanged = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("Current"))
					scissorRect = currentViewport.scissorRect();
				ImGui::SameLine();
				if (ImGui::Button("Apply") || (applyEveryframe && valueChanged))
				{
					if (!(scissorRect == currentScissorRect) && scissorRect.w > 0 && scissorRect.h > 0)
						currentViewport.setScissorRect(scissorRect);
				}

				ImGui::TreePop();
			}

			if (currentComboViewport > 0 && viewportData[currentComboViewport - 1].sprite && ImGui::TreeNode("Sprite"))
			{
				nc::Sprite &sprite = *viewportData[currentComboViewport - 1].sprite;

				nc::Vector2f position = sprite.position();
				ImGui::SliderFloat("Pos X", &position.x, (-nc::theApplication().widthInt() + sprite.width()) / 2, (nc::theApplication().widthInt() - sprite.width()) / 2);
				ImGui::SliderFloat("Pos Y", &position.y, (-nc::theApplication().heightInt() + sprite.height()) / 2, (nc::theApplication().heightInt() - sprite.height()) / 2);
				sprite.setPosition(position);

				static int positionIndex = viewportCreationData[currentComboViewport - 1].spritePositionIndex;
				if (viewportChanged)
					positionIndex = viewportCreationData[currentComboViewport - 1].spritePositionIndex;
				if (ImGui::Combo("Position Presets", &positionIndex, PositionPresetsLabels, IM_ARRAYSIZE(PositionPresetsLabels)))
					sprite.setPosition(positionPresets(sprite.absSize(), positionIndex));

				float rotation = sprite.rotation();
				ImGui::SliderFloat("Rotation", &rotation, 0.0f, 360.0f);
				nc::Vector2f scaleFactors = sprite.scale();
				ImGui::SliderFloat2("Scale", scaleFactors.data(), 0.01f, 3.0f);

				bool isEnabled = sprite.isEnabled();
				ImGui::Checkbox("Enabled", &isEnabled);
				sprite.setEnabled(isEnabled);
				ImGui::SameLine();

				if (ImGui::Button("Reset"))
				{
					rotation = 0.0f;
					scaleFactors.set(1.0f, 1.0f);
					// Set scaling factors before updating the position
					sprite.setScale(scaleFactors);
					positionIndex = viewportCreationData[currentComboViewport - 1].spritePositionIndex;
					sprite.setPosition(positionPresets(sprite.size(), positionIndex));
				}
				sprite.setRotation(rotation);
				sprite.setScale(scaleFactors);

				ImGui::TreePop();
			}

			ImGui::Separator();

			nc::Camera &currentCamera = *currentViewport.camera();
			if (ImGui::TreeNode("Projection Matrix"))
			{
				static nc::Camera::ProjectionValues values = currentCamera.projectionValues();
				if (viewportChanged)
					values = currentCamera.projectionValues();
				static bool applyEveryframe = false;
				bool valueChanged = false;

				ImGui::Text("Update Frame: %lu", currentCamera.updateFrameProjectionMatrix());
				valueChanged |= ImGui::SliderFloat("Left", &values.left, 0.0f, static_cast<float>(nc::theApplication().width()));
				valueChanged |= ImGui::SliderFloat("Right", &values.right, values.left, static_cast<float>(nc::theApplication().width()));
				valueChanged |= ImGui::SliderFloat("Top", &values.top, 0.0f, static_cast<float>(nc::theApplication().height()));
				valueChanged |= ImGui::SliderFloat("Bottom", &values.bottom, values.top, static_cast<float>(nc::theApplication().height()));
				if (ImGui::Checkbox("Apply Every Frame", &applyEveryframe))
					valueChanged = true;
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
				{
					values.left = 0.0f;
					values.right = static_cast<float>(nc::theApplication().width());
					values.top = 0.0f;
					values.bottom = static_cast<float>(nc::theApplication().height());
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
				if (viewportChanged || viewMatrixChanged)
				{
					values = currentCamera.viewValues();
					camPos_ = -values.position;
					camRot_ = -values.rotation;
					camScale_ = values.scale;
					viewMatrixChanged = false;
				}
				static bool applyEveryframe = false;
				bool valueChanged = false;

				ImGui::Text("Update Frame: %lu", currentCamera.updateFrameViewMatrix());
				valueChanged |= ImGui::SliderFloat("Position X", &values.position.x, -nc::theApplication().width(), nc::theApplication().width());
				valueChanged |= ImGui::SliderFloat("Position Y", &values.position.y, -nc::theApplication().height(), nc::theApplication().height());
				valueChanged |= ImGui::SliderFloat("Rotation", &values.rotation, 0.0f, 360.0f);
				valueChanged |= ImGui::SliderFloat("Scale", &values.scale, MinCameraScale, MaxCameraScale);
				if (ImGui::Checkbox("Apply Every Frame", &applyEveryframe))
					valueChanged = true;
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
				{
					values.position.set(-nc::theApplication().width() * 0.5f, -nc::theApplication().height() * 0.5f);
					values.rotation = 0.0f;
					values.scale = 1.0f;
					valueChanged = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("Apply") || (applyEveryframe && valueChanged))
				{
					camPos_ = -values.position;
					camRot_ = -values.rotation;
					camScale_ = values.scale;
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

	const float interval = nc::theApplication().interval();
	if (!pause_)
		angle_ += 2.0f * interval;

	if (inputEnabled_)
	{
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

	if (camRot_ > 0.01f || camRot_ < -0.01f)
		camRot_ = fmodf(camRot_, 360.0f);

	const nc::Application::RenderingSettings &settings = nc::theApplication().renderingSettings();
	debugString_->clear();
	debugString_->format("x: %.2f, y: %.2f, scale: %.2f, angle: %.2f", -camPos_.x, -camPos_.y, camScale_, -camRot_);
	debugString_->formatAppend("\nbatching: %s, culling: %s, input: %s", stringOnOff(settings.batchingEnabled),
	                           stringOnOff(settings.cullingEnabled), stringOnOff(inputEnabled_));
	debugText_->setString(*debugString_);

	nc::Viewport &currentViewport = (currentComboViewport > 0) ? *viewportData[currentComboViewport - 1].viewport : nc::theApplication().screenViewport();
	nc::Camera &currentCamera = *currentViewport.camera();

	const nc::Camera::ViewValues &viewValues = currentCamera.viewValues();
	if (-camPos_.x != viewValues.position.x || -camPos_.y != viewValues.position.y ||
	    -camRot_ != viewValues.rotation || camScale_ != viewValues.scale)
	{
		currentCamera.setView(-camPos_, -camRot_, camScale_);
		viewMatrixChanged = true;
	}

	if (!pause_)
	{
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
}

void MyEventHandler::onResizeWindow(int width, int height)
{
	for (unsigned int i = 0; i < NumViewports; i++)
	{
		const nc::Recti &oldRect = viewportCreationData[i].rect;
		nc::Recti rect = oldRect;

		if (rect.w != 0 && rect.h != 0)
		{
			const float widthRatio = width / static_cast<float>(nc::theApplication().appConfiguration().resolution.x);
			const float heightRatio = height / static_cast<float>(nc::theApplication().appConfiguration().resolution.y);
			rect.set(oldRect.x * widthRatio, oldRect.y * heightRatio, oldRect.w * widthRatio, oldRect.h * heightRatio);
		}

		if (viewportCreationData[i].withTexture)
		{
			viewportData[i].viewport->removeAllTextures();
			viewportData[i].texture->init(nullptr, nc::Texture::Format::RGB8, rect.w, rect.h);
			viewportData[i].viewport->setTexture(viewportData[i].texture.get());
		}
		else
		{
			const nc::Viewport &prevViewport = (i > 0) ? *viewportData[i - 1].viewport : nc::theApplication().screenViewport();
			viewportData[i].viewport->setViewportRect(prevViewport.viewportRect());
		}

		viewportData[i].camera->setView(-width * 0.5f, -height * 0.5f, 0.0f, 1.0f);

		if (rect.w != 0 && rect.h != 0)
			viewportData[i].viewport->setViewportRect(rect);

		if (viewportCreationData[i].withTexture == false)
		{
			const nc::Recti &vpRect = viewportData[i].viewport->viewportRect();
			nc::Camera::ProjectionValues projValues(0, vpRect.w, 0, vpRect.h);
			viewportData[i].camera->setOrthoProjection(projValues);
		}

		if (rect.w != 0 && rect.h != 0)
		{
			nc::Sprite *sprite = viewportData[i].sprite.get();
			sprite->resetTexture();
			sprite->setPosition(positionPresets(sprite->absSize(), viewportCreationData[i].spritePositionIndex));
		}
	}

	const float resizeRatio = width / static_cast<float>(nc::theApplication().gfxDevice().width());
	camPos_ *= resizeRatio;
	debugText_->setPosition(debugText_->position() * resizeRatio);
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();

	if (event.sym == nc::KeySym::B)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.sym == nc::KeySym::C)
		renderingSettings.cullingEnabled = !renderingSettings.cullingEnabled;
	else if (event.sym == nc::KeySym::I)
		inputEnabled_ = !inputEnabled_;
	else if (event.sym == nc::KeySym::R)
		resetCamera();
	else if (event.mod & nc::KeyMod::CTRL && event.sym == nc::KeySym::H)
		showImGui = !showImGui;
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
	if (inputEnabled_ == false)
		return;

	if (event.button == nc::MouseButton::LEFT)
	{
		scrollOrigin_.x = static_cast<float>(event.x);
		scrollOrigin_.y = static_cast<float>(event.y);
		scrollMove_ = scrollOrigin_;
	}
	else if (event.button == nc::MouseButton::RIGHT)
	{
		scrollOrigin2_.x = static_cast<float>(event.x);
		scrollOrigin2_.y = static_cast<float>(event.y);
		scrollMove2_ = scrollOrigin2_;
	}
}

void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
{
	if (inputEnabled_ == false)
		return;

	if (event.button == nc::MouseButton::LEFT)
	{
		if (lastClickTime_.secondsSince() < DoubleClickDelay)
			resetCamera();
		lastClickTime_ = nc::TimeStamp::now();
	}
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state)
{
	if (inputEnabled_ == false)
		return;

	if (state.isButtonDown(nc::MouseButton::LEFT))
	{
		scrollMove_.x = static_cast<float>(state.x);
		scrollMove_.y = static_cast<float>(state.y);
	}
	else if (state.isButtonDown(nc::MouseButton::RIGHT))
	{
		scrollMove2_.x = static_cast<float>(state.x);
		scrollMove2_.y = static_cast<float>(state.y);
	}
}

void MyEventHandler::onScrollInput(const nc::ScrollEvent &event)
{
	if (inputEnabled_ == false)
		return;

	camRot_ += 10.0f * event.x;
	camScale_ += 0.1f * event.y;
}

void MyEventHandler::resetCamera()
{
	camPos_.x = nc::theApplication().width() * 0.5f;
	camPos_.y = nc::theApplication().height() * 0.5f;
	camRot_ = 0.0f;
	camScale_ = 1.0f;
}
