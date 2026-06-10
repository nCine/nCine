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

#include <ncine/tracy.h>

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

const unsigned int MaxStatsSeconds = 3;
const unsigned int MaxStatsFrames = 1000;

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);
	config.window.title = InitialWindowTitle;
	//config.window.resolution.set(800, 600); // window size for the original BunnyMark
#ifndef __ANDROID__
	config.graphics.vsync = false;
#endif
}

void MyEventHandler::onInit()
{
#if NCINE_WITH_IMGUI && defined(__ANDROID__)
	const float scalingFactor = nc::theApplication().gfxDevice().windowScalingFactor();
	ImGuiStyle &style = ImGui::GetStyle();
	style.FontScaleMain = scalingFactor;
	style.ScaleAllSizes(scalingFactor);
#endif

	const unsigned int refreshRate = static_cast<unsigned int>(nc::theApplication().gfxDevice().currentVideoMode().refreshRate);
	const unsigned int numFrames = nctl::min(refreshRate * MaxStatsSeconds, MaxStatsFrames);
	frameStats_.setCapacity(numFrames);

	nc::theApplication().screenViewport().setClearColor(0.392f, 0.584f, 0.929f, 1.0f);

	texture_ = nctl::makeUnique<nc::Texture>((prefixDataPath("textures", TextureFile)).data());
	sprites_.setCapacity(InitialCapacity);
	transforms_.setCapacity(InitialCapacity);

	setNumBunnies(InitialSize);

	withVSync_ = nc::theApplication().appConfiguration().graphics.vsync;
	pause_ = false;

	nc::IFrameTimer &frameTimer = nc::theApplication().frameTimer();
	frameTimer.setAverageInterval(winTitleInterval);
	frameTimer.setLoggingInterval(loggingInterval);
}

void MyEventHandler::onFrameStart()
{
	TracyPlot("Bunnies", static_cast<int64_t>(sprites_.size()));

	nc::IFrameTimer &frameTimer = nc::theApplication().frameTimer();
	if (frameTimer.averageEnabled())
		windowTitle.format("%s - %u bunnies (%.2f FPS, %.2f ms)", InitialWindowTitle, sprites_.size(), frameTimer.averageFps(), frameTimer.averageFrameTime() * 1000.0f);
	else
		windowTitle.format("%s - %u bunnies", InitialWindowTitle, sprites_.size());
	nc::theApplication().gfxDevice().setWindowTitle(windowTitle.data());

	if (pause_ == false)
	{
		ZoneScopedN("Update bunnies");
		const nc::Vector2f minBounds(texture_->width() * 0.5f, texture_->height() * 0.5f);
		const nc::Vector2f maxBounds(nc::theApplication().width() - texture_->width() * 0.5f, nc::theApplication().height() - texture_->height() * 0.5f);

		for (unsigned int i = 0; i < transforms_.size(); i++)
			updateBunny(i, minBounds, maxBounds);
	}

#if NCINE_WITH_IMGUI
	if (showImGui)
	{
		ImGui::SetNextWindowSize(ImVec2(400.0f, 440.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(40.0f, 40.0f), ImGuiCond_FirstUseEver);

		windowTitle.append("###apptest_bunnymark");
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
			bool instancingEnabled = nc::theApplication().renderingSettings().instancingEnabled;
			ImGui::Checkbox("Instancing", &instancingEnabled);
			nc::theApplication().renderingSettings().instancingEnabled = instancingEnabled;

	#ifndef __ANDROID__
			ImGui::SameLine();
			ImGui::Checkbox("V-Sync", &withVSync_);
			nc::theApplication().gfxDevice().setSwapInterval(withVSync_ ? 1 : 0);
	#endif

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

			if (ImGui::TreeNodeEx("Statistics", ImGuiTreeNodeFlags_DefaultOpen))
			{
				// If the tree node is closed, no statistics are collected
				const float frameTimeMs = frameTimer.lastFrameTime() * 1000;
				frameStats_.addValueWrap(frameTimeMs);

				static nc::TimeStamp timestamp = nc::TimeStamp::now();
				static bool sorted = false;
				ImGui::PlotHistogram("Frame time", frameStats_.values(sorted), frameStats_.size(), 0, nullptr, 0.0f, frameStats_.maximum() * 1.1f);

				ImGui::Checkbox("Sorted", &sorted);
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
				{
					frameStats_.clearValues();
					frameStats_.resetStats();
				}
				ImGui::SameLine();
				static float updateStats = 1.0f;
				ImGui::PushItemWidth(164.0f);
				ImGui::SliderFloat("Update##statistics", &updateStats, 0.05f, 2.0f, "%.2f s", ImGuiSliderFlags_AlwaysClamp);
				ImGui::PopItemWidth();
				if (timestamp.secondsSince() >= updateStats)
				{
					frameStats_.calculateStats();
					timestamp.toNow();
				}

				if (ImGui::BeginTable("FrameTimingsTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
				                      ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp))
				{
					ImGui::TableSetupScrollFreeze(0, 1);
					ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoHide);
					ImGui::TableSetupColumn("Timings");
					ImGui::TableHeadersRow();

					ImGui::TableNextRow();
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Mean");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", frameStats_.mean());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Median");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", frameStats_.median());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Mode");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", frameStats_.mode());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("P75");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", frameStats_.percentile(0.75f));
					ImGui::TableNextColumn(); ImGui::TextUnformatted("P90");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", frameStats_.percentile(0.9f));
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Sigma");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", frameStats_.sigma());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Rel. Sigma");
					ImGui::TableNextColumn(); ImGui::Text("%.3f%%", frameStats_.relativeSigma());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Min");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", frameStats_.minimum());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Max");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", frameStats_.maximum());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Range");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", frameStats_.range());

					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
#endif
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	#if NCINE_WITH_IMGUI
	if (passMouseEvents == false)
		return;
	#endif

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

	if (event.button == nc::MouseButton::LEFT)
		setNumBunnies(transforms_.size() + appendSize);
	else if (event.button == nc::MouseButton::RIGHT)
		setNumBunnies(InitialSize);
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::B)
	{
		const bool batchingEnabled = nc::theApplication().renderingSettings().batchingEnabled;
		nc::theApplication().renderingSettings().batchingEnabled = !batchingEnabled;
	}
	else if (event.sym == nc::KeySym::I)
	{
		const bool instancingEnabled = nc::theApplication().renderingSettings().instancingEnabled;
		nc::theApplication().renderingSettings().instancingEnabled = !instancingEnabled;
	}
#ifndef __ANDROID__
	else if (event.sym == nc::KeySym::V)
		withVSync_ = !withVSync_;
#endif
	else if (event.sym == nc::KeySym::SPACE)
		setNumBunnies(transforms_.size() + appendSize);
	else if (event.sym == nc::KeySym::R)
		setNumBunnies(InitialCapacity);
	else if (event.sym == nc::KeySym::F)
	{
		nc::IGfxDevice &gfxDevice = nc::theApplication().gfxDevice();
		gfxDevice.setFullscreen(!gfxDevice.isFullscreen());
		if (gfxDevice.isFullscreen() == false)
			gfxDevice.setWindowSize(nc::theApplication().appConfiguration().window.resolution);
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

	if (event.buttonName == nc::ButtonName::A)
		renderingSettings.batchingEnabled = !renderingSettings.batchingEnabled;
	else if (event.buttonName == nc::ButtonName::X)
		renderingSettings.instancingEnabled = !renderingSettings.instancingEnabled;
#ifndef __ANDROID__
	else if (event.buttonName == nc::ButtonName::B)
		withVSync_ = !withVSync_;
#endif
	else if (event.buttonName == nc::ButtonName::Y)
		setNumBunnies(InitialSize);
	else if (event.buttonName == nc::ButtonName::LBUMPER)
		setNumBunnies(transforms_.size() + appendSize);
	else if (event.buttonName == nc::ButtonName::RBUMPER)
		setNumBunnies(transforms_.size() - appendSize);
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

void MyEventHandler::updateBunny(unsigned int index, const nc::Vector2f &minBounds, const nc::Vector2f &maxBounds)
{
	Transform &transform = transforms_[index];
	nc::Sprite &sprite = *sprites_[index];

	transform.position += transform.velocity;
	transform.velocity += nc::Vector2f(0.0f, gravity);

	if (transform.position.x > maxBounds.x)
	{
		transform.velocity.x *= -1.0f;
		transform.position.x = maxBounds.x;
	}
	else if (transform.position.x < minBounds.x)
	{
		transform.velocity.x *= -1.0f;
		transform.position.x = minBounds.x;
	}

	if (transform.position.y < minBounds.y)
	{
		transform.velocity.y *= -0.8f;
		transform.position.y = minBounds.y;

		// Flip a coin
		if (nc::random().fastInteger(0, 2) > 0)
			transform.velocity += nc::Vector2f(0.0f, 3.0f + nc::random().fastReal() * 4.0f);
	}
	else if (transform.position.y > maxBounds.y)
	{
		transform.velocity.y = 0.0f;
		transform.position.y = maxBounds.y;
	}

	sprite.setPosition(transform.position);
}
