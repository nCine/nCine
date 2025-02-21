#include <ncine/config.h>
#include <ncine/imgui.h>

#include "apptest_scaling.h"
#include <nctl/StaticString.h>
#include <ncine/Application.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/TextNode.h>
#include <ncine/TimeStamp.h>
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

nctl::StaticString<512> auxString;
nctl::StaticString<4096> comboString;
float scalingFactor = 1.0f;
float referenceScalingFactor = 1.0f;
nc::Vector2i referenceWindowSize;

int selectedVideoMode[nc::IGfxDevice::MaxMonitors] = {};
int imguiWindowSizeFrames = 3;
int swapInterval = 1;

const float fpsShowTime = 0.25f;
float showFrameTime = 0.0f;
nc::TimeStamp showFpsTimestamp;

bool showImGui = true;

const char *backendName()
{
#ifdef __ANDROID__
	return "Android";
#else
	#if NCINE_WITH_SDL
		#ifdef __EMSCRIPTEN__
	return "SDL2 (Emscripten)";
		#else
	return "SDL2";
		#endif
	#elif NCINE_WITH_GLFW
		#ifdef __EMSCRIPTEN__
	return "GLFW (Emscripten)";
		#else
	return "GLFW";
		#endif
	#elif NCINE_WITH_QT5
	return "Qt5";
	#endif
#endif
	return "Unknown";
}

bool centerWindow(unsigned int monitorIndex)
{
	nc::IGfxDevice &gfxDevice = nc::theApplication().gfxDevice();

	if (monitorIndex > gfxDevice.numMonitors() - 1)
		return false;

	const bool windowScaling = nc::theApplication().appConfiguration().windowScaling;
	const nc::IGfxDevice::VideoMode &videoMode = gfxDevice.currentVideoMode(monitorIndex);
	const nc::IGfxDevice::Monitor &monitor = gfxDevice.monitor(monitorIndex);

	const nc::Vector2i halfScreenResolution(videoMode.width / 2, videoMode.height / 2);
	const float factorRatio = windowScaling ? gfxDevice.windowScalingFactor() / referenceScalingFactor : 1.0f;
	const nc::Vector2i scaledWindowSize(referenceWindowSize.x * factorRatio, referenceWindowSize.y * factorRatio);
	const bool isFullscreen = gfxDevice.isFullScreen();
	gfxDevice.setFullScreen(false);
	gfxDevice.setWindowPosition(monitor.position + halfScreenResolution - scaledWindowSize / 2);
	if (isFullscreen && windowScaling)
		gfxDevice.setWindowSize(scaledWindowSize);

	return true;
}

bool goFullScreen(unsigned int monitorIndex, unsigned int modeIndex)
{
	nc::IGfxDevice &gfxDevice = nc::theApplication().gfxDevice();

	if (monitorIndex > gfxDevice.numMonitors() - 1)
		return false;

	const nc::IGfxDevice::Monitor &monitor = gfxDevice.monitor(monitorIndex);
	if (modeIndex > monitor.numVideoModes)
		return false;

	const nc::IGfxDevice::VideoMode &currentMode = gfxDevice.currentVideoMode(monitorIndex);
	if (gfxDevice.isFullScreen() && gfxDevice.windowMonitorIndex() == monitorIndex && currentMode == monitor.videoModes[modeIndex])
		return false;

	// Go full screen to another monitor
	if (gfxDevice.windowMonitorIndex() != monitorIndex)
	{
		gfxDevice.setFullScreen(false);
		gfxDevice.setWindowPosition(monitor.position);
	}
	gfxDevice.setVideoMode(modeIndex);
	gfxDevice.setFullScreen(true);

	return true;
}

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
	//config.windowScaling = false;
}

void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture1File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture2File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture3File)).data()));
	textures_.pushBack(nctl::makeUnique<nc::Texture>((prefixDataPath("textures", Texture4File)).data()));

	for (unsigned int i = 0; i < NumSprites; i++)
		sprites_.pushBack(nctl::makeUnique<nc::Sprite>(&rootNode, textures_[i % NumTextures].get()));

	font_ = nctl::makeUnique<nc::Font>((prefixDataPath("fonts", FontFntFile)).data(),
	                                   (prefixDataPath("fonts", FontTextureFile)).data());
	textNode_ = nctl::makeUnique<nc::TextNode>(&rootNode, font_.get(), 256);
	textNode_->setColor(255, 255, 0, 255);

	nc::IGfxDevice &gfxDevice = nc::theApplication().gfxDevice();
	referenceScalingFactor = gfxDevice.windowScalingFactor();
	referenceWindowSize = gfxDevice.resolution();
	// If starting in full screen, set the reference window size to a scaled half full screen resolution
	if (gfxDevice.isFullScreen())
	{
		referenceWindowSize.x *= referenceScalingFactor / 2.0f;
		referenceWindowSize.y *= referenceScalingFactor / 2.0f;
	}
	gfxDevice.setSwapInterval(swapInterval);
	showFpsTimestamp = nc::TimeStamp::now();
}

void MyEventHandler::onFrameStart()
{
	if (showImGui)
	{
		nc::IGfxDevice &gfxDevice = nc::theApplication().gfxDevice();
		if (ImGui::GetIO().FontGlobalScale != scalingFactor)
		{
			ImGui::GetIO().FontGlobalScale = scalingFactor;
			ImGui::GetStyle() = ImGuiStyle();
			ImGui::GetStyle().ScaleAllSizes(scalingFactor);
		}
		const ImGuiCond conditionFlag = (imguiWindowSizeFrames-- > 0) ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
		ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f), conditionFlag);
		ImGui::SetNextWindowPos(ImVec2(40.0f * scalingFactor, 40.0f * scalingFactor), conditionFlag);
		if (ImGui::Begin("apptest_scaling", &showImGui))
		{
			ImGui::Text("Backend: %s", backendName());

			const unsigned int numMonitors = gfxDevice.numMonitors();
			for (unsigned int monitorIndex = 0; monitorIndex < numMonitors; monitorIndex++)
			{
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;

				const nc::IGfxDevice::Monitor &monitor = gfxDevice.monitor(monitorIndex);
				auxString.format("Monitor #%u: \"%s\"", monitorIndex, monitor.name);
				if (monitorIndex == gfxDevice.primaryMonitorIndex())
					auxString.append(" [Primary]");
				if (monitorIndex == gfxDevice.windowMonitorIndex())
				{
					auxString.formatAppend(" [%s]", gfxDevice.isFullScreen() ? "Full Screen" : "Window");
					flags |= ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected;
				}

				if (ImGui::TreeNodeEx(auxString.data(), flags))
				{
					ImGui::Text("Position: <%d, %d>", monitor.position.x, monitor.position.y);
					ImGui::Text("DPI: <%d, %d>", monitor.dpi.x, monitor.dpi.y);
					ImGui::Text("Scale: <%.2f, %.2f>", monitor.scale.x, monitor.scale.y);

					const nc::IGfxDevice::VideoMode &currentMode = gfxDevice.currentVideoMode(monitorIndex);
					ImGui::Text("Current mode: %u x %u, %.2f Hz", currentMode.width, currentMode.height, currentMode.refreshRate);

					const unsigned int numVideoModes = monitor.numVideoModes;
					comboString.clear();
					for (unsigned int modeIndex = 0; modeIndex < numVideoModes; modeIndex++)
					{
						const nc::IGfxDevice::VideoMode &mode = monitor.videoModes[modeIndex];
						comboString.formatAppend("#%u: %u x %u, %.2f Hz", modeIndex, mode.width, mode.height, mode.refreshRate);
						comboString.setLength(comboString.length() + 1);
					}
					comboString.setLength(comboString.length() + 1);
					// Append a second '\0' to signal the end of the combo item list
					comboString[comboString.length() - 1] = '\0';

					if (selectedVideoMode[monitorIndex] < 0)
						selectedVideoMode[monitorIndex] = 0;
					if (selectedVideoMode[monitorIndex] > monitor.numVideoModes - 1)
						selectedVideoMode[monitorIndex] = monitor.numVideoModes - 1;

					ImGui::Combo("Video Mode", &selectedVideoMode[monitorIndex], comboString.data());

					const bool alreadyFullScreenHere = gfxDevice.isFullScreen() && gfxDevice.windowMonitorIndex() == monitorIndex &&
					                                   monitor.videoModes[selectedVideoMode[monitorIndex]] == gfxDevice.currentVideoMode(monitorIndex);
					ImGui::BeginDisabled(alreadyFullScreenHere);
					if (ImGui::Button("Go full screen"))
						goFullScreen(monitorIndex, selectedVideoMode[monitorIndex]);
					ImGui::EndDisabled();

#ifndef __ANDROID__
					const bool alreadyWindowedHere = gfxDevice.isFullScreen() == false && gfxDevice.windowMonitorIndex() == monitorIndex;
					ImGui::SameLine();
					ImGui::BeginDisabled(alreadyWindowedHere);
					if (ImGui::Button("Go windowed"))
						centerWindow(monitorIndex);
					ImGui::EndDisabled();
#endif

					ImGui::TreePop();
				}
			}

			const bool windowScaling = nc::theApplication().appConfiguration().windowScaling;
			ImGui::Text("Window resolution: %d x %d (resizable: %s, scaled: %s)", gfxDevice.width(), gfxDevice.height(), gfxDevice.isResizable() ? "yes" : "no", windowScaling ? "yes" : "no");
			ImGui::Text("Drawable resolution: %d x %d", gfxDevice.drawableWidth(), gfxDevice.drawableHeight());
#if !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
			ImGui::Text("Window position: <%d, %d>", gfxDevice.windowPositionX(), gfxDevice.windowPositionY());

			ImGui::SameLine();
			ImGui::BeginDisabled(gfxDevice.isFullScreen());
			if (ImGui::Button("Center"))
			{
				const unsigned int monitorIndex = gfxDevice.windowMonitorIndex();
				const nc::IGfxDevice::Monitor &monitor = gfxDevice.monitor(monitorIndex);
				const nc::IGfxDevice::VideoMode &videoMode = gfxDevice.currentVideoMode(monitorIndex);
				const nc::Vector2i halfScreenResolution(videoMode.width / 2, videoMode.height / 2);
				gfxDevice.setWindowPosition(monitor.position + halfScreenResolution - gfxDevice.resolution() / 2);
			}
			ImGui::EndDisabled();
#endif

			ImGui::Separator();
#ifndef __ANDROID__
			if (ImGui::SliderInt("Swap interval", &swapInterval, -1, 2, "%d", ImGuiSliderFlags_AlwaysClamp))
				gfxDevice.setSwapInterval(swapInterval);
#endif

			if (showFpsTimestamp.secondsSince() > fpsShowTime)
			{
				showFpsTimestamp = nc::TimeStamp::now();
				showFrameTime = nc::theApplication().frameTime();
			}

			ImGui::Text("FPS: %u (%.2f ms)", static_cast<unsigned int>(1.0f / showFrameTime), showFrameTime * 1000.0f);
		}
		ImGui::End();
	}

	const float width = nc::theApplication().width();
	const float height = nc::theApplication().height();

	auxString.format("Monitor scaling at %u%%", static_cast<unsigned int>(scalingFactor * 100));
	textNode_->setString(auxString.data());
	textNode_->setScale(scalingFactor);
	textNode_->setPosition(width * 0.5f, height - textNode_->height() * 0.5f);

	for (unsigned int i = 0; i < NumSprites; i++)
	{
		sprites_[i]->setScale(0.5f * scalingFactor);
		sprites_[i]->setPosition(width * 0.1f + width * 0.8f / (NumSprites - 1) * i, sinf(360.0f * nc::fDegToRad / (NumSprites - 1) * i) * height * 0.25f + height * 0.5f);
	}
}

void MyEventHandler::onChangeScalingFactor(float factor)
{
	scalingFactor = factor;
	if (nc::theApplication().appConfiguration().windowScaling)
		imguiWindowSizeFrames = 3;
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym >= nc::KeySym::N1 && event.sym <= nc::KeySym::N4)
		centerWindow(static_cast<unsigned int>(event.sym) - static_cast<unsigned int>(nc::KeySym::N1));
	else if (event.sym >= nc::KeySym::N5 && event.sym <= nc::KeySym::N8)
		goFullScreen(static_cast<unsigned int>(event.sym) - static_cast<unsigned int>(nc::KeySym::N5), 0);
	else if (event.mod & nc::KeyMod::CTRL && event.sym == nc::KeySym::H)
		showImGui = !showImGui;
	else if (event.sym == nc::KeySym::F)
	{
		const bool isFullScreen = nc::theApplication().gfxDevice().isFullScreen();
		const unsigned int monitorIndex = nc::theApplication().gfxDevice().windowMonitorIndex();
		if (isFullScreen)
			centerWindow(monitorIndex);
		else
			goFullScreen(monitorIndex, 0);
	}
	else if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}
