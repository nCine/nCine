#include <ncine/config.h>

#include "apptest_bunnymark.h"
#include <nctl/StaticString.h>
#include <ncine/Application.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/Viewport.h>
#include <ncine/Random.h>
#include <ncine/IFrameTimer.h>
#include <ncine/TimeStamp.h>
#include "apptest_datapath.h"

#if NCINE_WITH_IMGUI
	#include <ncine/imgui.h>
#endif

/// Based on https://github.com/openfl/openfl-samples/tree/master/demos/BunnyMark
/// Original BunnyMark (and sprite) by Iain Lobb

namespace {

#ifdef __ANDROID__
const char *TextureFile = "bunny_ETC2.ktx";
#else
const char *TextureFile = "bunny.png";
#endif

const char *InitialWindowTitle = "BunnyMark";
#if defined(__ANDROID__) || defined(__EMSCRIPTEN__)
const unsigned int InitialCapacity = 2500;
const unsigned int InitialSize = 250;
#else
const unsigned int InitialCapacity = 10000;
const unsigned int InitialSize = 1000;
#endif
static_assert(InitialCapacity >= InitialSize, "Initial capacity cannot be smaller than the initial size");
int appendSize = InitialSize;

float gravity = -0.5f;
// Set to zero to disable it
float winTitleInterval = 0.2f;
// Set to zero to disable it
float loggingInterval = 5.0f;

nctl::StaticString<256> windowTitle;

#if NCINE_WITH_IMGUI
bool showImGui = true;
bool passMouseEvents = true;
#endif

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
	config.windowTitle = InitialWindowTitle;
	//config.resolution.set(800, 600); // window size for the original BunnyMark
}

void MyEventHandler::onInit()
{
	nc::theApplication().screenViewport().setClearColor(0.392f, 0.584f, 0.929f, 1.0f);

	texture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", TextureFile)).data());
	sprites_.setCapacity(InitialCapacity);
	transforms_.setCapacity(InitialCapacity);

	setNumBunnies(InitialSize);

	withVSync_ = nc::theApplication().appConfiguration().withVSync;
	pause_ = false;

	nc::IFrameTimer &frameTimer = nc::theApplication().frameTimer();
	frameTimer.setAverageInterval(winTitleInterval);
	frameTimer.setLoggingInterval(loggingInterval);
}

void MyEventHandler::onFrameStart()
{
	nc::IFrameTimer &frameTimer = nc::theApplication().frameTimer();
	if (frameTimer.averageEnabled())
		windowTitle.format("%s - %u bunnies (%.2f FPS, %.2f ms)", InitialWindowTitle, sprites_.size(), frameTimer.averageFps(), frameTimer.averageFrameTime() * 1000.0f);
	else
		windowTitle.format("%s - %u bunnies", InitialWindowTitle, sprites_.size());
	nc::theApplication().gfxDevice().setWindowTitle(windowTitle.data());

	if (pause_ == false)
	{
		const float minX = texture_->width() * 0.5f;
		const float minY = texture_->height() * 0.5f;
		const float maxX = nc::theApplication().width() - texture_->width() * 0.5f;
		const float maxY = nc::theApplication().height() - texture_->height() * 0.5f;

		for (unsigned int i = 0; i < transforms_.size(); i++)
		{
			transforms_[i].position += transforms_[i].velocity;
			transforms_[i].velocity += nc::Vector2f(0.0f, gravity);

			if (transforms_[i].position.x > maxX)
			{
				transforms_[i].velocity.x *= -1.0f;
				transforms_[i].position.x = maxX;
			}
			else if (transforms_[i].position.x < minX)
			{
				transforms_[i].velocity.x *= -1.0f;
				transforms_[i].position.x = minX;
			}

			if (transforms_[i].position.y < minY)
			{
				transforms_[i].velocity.y *= -0.8f;
				transforms_[i].position.y = minY;

				// Flip a coin
				if (nc::random().fastInteger(0, 2) > 0)
					transforms_[i].velocity += nc::Vector2f(0.0f, 3.0f + nc::random().fastReal() * 4.0f);
			}
			else if (transforms_[i].position.y > maxY)
			{
				transforms_[i].velocity.y = 0.0f;
				transforms_[i].position.y = maxY;
			}

			sprites_[i]->setPosition(transforms_[i].position);
		}
	}

#if NCINE_WITH_IMGUI
	if (showImGui)
	{
		ImGui::SetNextWindowSize(ImVec2(375.0f, 190.0f), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(40.0f, 40.0f), ImGuiCond_FirstUseEver);

		windowTitle.append("###apptest_bunny");
		if (ImGui::Begin(windowTitle.data(), &showImGui))
		{
			passMouseEvents = (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) == false);

			if (ImGui::Button("Quit"))
				nc::theApplication().quit();
			ImGui::SameLine();
			ImGui::Checkbox("Pause", &pause_);

			ImGui::SameLine();
			bool batchingEnabled = nc::theApplication().renderingSettings().batchingEnabled;
			ImGui::Checkbox("Batching", &batchingEnabled);
			nc::theApplication().renderingSettings().batchingEnabled = batchingEnabled;

			ImGui::SameLine();
			ImGui::Checkbox("V-Sync", &withVSync_);
			nc::theApplication().gfxDevice().setSwapInterval(withVSync_ ? 1 : 0);

			bool winTitleEnabled = frameTimer.averageEnabled();
			ImGui::BeginDisabled(winTitleEnabled == false);
			ImGui::SliderFloat("Update title", &winTitleInterval, 0.05f, 1.0f, "%.2f s", ImGuiSliderFlags_AlwaysClamp);
			frameTimer.setAverageInterval(winTitleInterval);
			ImGui::EndDisabled();
			ImGui::SameLine();
			ImGui::Checkbox("##EnableWinTitle", &winTitleEnabled);
			frameTimer.setAverageInterval(winTitleEnabled ? winTitleInterval : 0.0f);

			int logLevel = static_cast<int>(frameTimer.logLevel());
			ImGui::Combo("Log Level", &logLevel, "Off\0Verbose\0Debug\0Info\0Warning\0Error\0Fatal\0");
			frameTimer.setLogLevel(static_cast<nc::ILogger::LogLevel>(logLevel));

			bool loggingEnabled = frameTimer.loggingEnabled();
			ImGui::BeginDisabled(loggingEnabled == false);
			ImGui::SliderFloat("Print log", &loggingInterval, 1.0f, 10.0f, "%.2f s", ImGuiSliderFlags_AlwaysClamp);
			ImGui::EndDisabled();
			ImGui::SameLine();
			ImGui::Checkbox("##EnableLogging", &loggingEnabled);
			const bool logLevelIsOff = (frameTimer.logLevel() == nc::ILogger::LogLevel::OFF);
			frameTimer.setLoggingInterval((loggingEnabled || logLevelIsOff) ? loggingInterval : 0.0f);

			ImGui::SliderFloat("Gravity", &gravity, 0.0f, -5.0f);

			ImGui::Text("Bunnies: %u (%u capacity)", sprites_.size(), sprites_.capacity());

			ImGui::SliderInt("##Amount", &appendSize, 1, 10000, "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SameLine();
			if (ImGui::Button("Add"))
				setNumBunnies(transforms_.size() + appendSize);
			ImGui::SameLine();
			if (ImGui::Button("Remove"))
			{
				appendSize = (appendSize <= transforms_.size()) ? appendSize : transforms_.size();
				setNumBunnies(transforms_.size() - appendSize);
			}
		}
		ImGui::End();
	}
#endif
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	if (event.pointers[0].x < nc::theApplication().width() * 0.5f)
		setNumBunnies(InitialSize);
	else
		setNumBunnies(transforms_.size() + appendSize);
}
#endif

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
#if NCINE_WITH_IMGUI
	if (passMouseEvents == false)
		return;
#endif

	if (event.isLeftButton())
		setNumBunnies(transforms_.size() + appendSize);
	else if (event.isRightButton())
		setNumBunnies(InitialSize);
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::B)
	{
		const bool batchingEnabled = nc::theApplication().renderingSettings().batchingEnabled;
		nc::theApplication().renderingSettings().batchingEnabled = !batchingEnabled;
	}
	else if (event.sym == nc::KeySym::V)
		withVSync_ = !withVSync_;
	else if (event.sym == nc::KeySym::SPACE)
		setNumBunnies(transforms_.size() + appendSize);
	else if (event.sym == nc::KeySym::R)
		setNumBunnies(InitialCapacity);
	else if (event.sym == nc::KeySym::F)
	{
		nc::IGfxDevice &gfxDevice = nc::theApplication().gfxDevice();
		gfxDevice.setFullScreen(!gfxDevice.isFullScreen());
		if (gfxDevice.isFullScreen() == false)
			gfxDevice.setWindowSize(nc::theApplication().appConfiguration().resolution);
	}
#if NCINE_WITH_IMGUI
	else if (event.mod & nc::KeyMod::CTRL && event.sym == nc::KeySym::H)
		showImGui = !showImGui;
#endif
	else if (event.sym == nc::KeySym::P)
		pause_ = !pause_;
	else if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}

void MyEventHandler::onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event)
{
	nc::Application::RenderingSettings &renderingSettings = nc::theApplication().renderingSettings();

	if (event.buttonName == nc::ButtonName::Y)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.buttonName == nc::ButtonName::X)
		withVSync_ = !withVSync_;
	else if (event.buttonName == nc::ButtonName::B)
		setNumBunnies(InitialCapacity);
	else if (event.buttonName == nc::ButtonName::A)
		setNumBunnies(transforms_.size() + appendSize);
#if NCINE_WITH_IMGUI
	else if (event.buttonName == nc::ButtonName::BACK)
		showImGui = !showImGui;
#endif
	else if (event.buttonName == nc::ButtonName::START)
		pause_ = !pause_;
	else if (event.buttonName == nc::ButtonName::GUIDE)
		nc::theApplication().quit();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool MyEventHandler::setNumBunnies(unsigned int count)
{
	ASSERT(sprites_.size() == transforms_.size());
	if (count == sprites_.size())
		return false;

	nc::SceneNode &rootNode = nc::theApplication().rootNode();
	const float height = nc::theApplication().height();

	if (count > sprites_.size())
	{
		const unsigned int amount = count - sprites_.size();
		for (unsigned int i = 0; i < amount; i++)
		{
			transforms_.emplaceBack(nc::Vector2f(0.0f, height), nc::Vector2f(nc::random().fastReal() * 5.0f, nc::random().fastReal(0.0f, 1.0f) * 5.0f - 2.5f));
			sprites_.pushBack(nctl::makeUnique<nc::Sprite>(&rootNode, texture_.get(), transforms_[i].position.x, transforms_[i].position.y));
		}
	}
	else
	{
		transforms_.setSize(count);
		sprites_.setSize(count);
	}

	return true;
}
