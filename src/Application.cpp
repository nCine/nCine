#include "Application.h"
#include "Random.h"
#include "IAppEventHandler.h"
#include "FileSystem.h"
#include "ArrayIndexer.h"
#include "GfxCapabilities.h"
#include "RenderResources.h"
#include "RenderQueue.h"
#include "ScreenViewport.h"
#include "GLDebug.h"
#include "Timer.h" // for `sleep()`
#include "FrameTimer.h"
#include "SceneNode.h"
#include <nctl/StaticString.h>
#include "IInputManager.h"

#ifdef WITH_AUDIO
	#include "ALAudioDevice.h"
#endif

#ifdef WITH_THREADS
	#include "ThreadPool.h"
#endif

#ifdef WITH_LUA
	#include "LuaStatistics.h"
#endif

#ifdef WITH_IMGUI
	#include "ImGuiDrawing.h"
	#include "ImGuiDebugOverlay.h"
#endif

#ifdef WITH_NUKLEAR
	#include "NuklearDrawing.h"
#endif

#include "tracy.h"
#include "tracy_opengl.h"

#ifdef WITH_RENDERDOC
	#include "RenderDocCapture.h"
#endif

#include "version.h"

namespace ncine {

namespace {
	static nctl::StaticString<256> appInfoString;
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Application::Application()
    : isSuspended_(false), autoSuspension_(true), hasFocus_(true), shouldQuit_(false)
{
}

Application::~Application() = default;

Application::GuiSettings::GuiSettings()
    : imguiLayer(0xffff - 1024),
      nuklearLayer(0xffff - 512),
      imguiViewport(nullptr), nuklearViewport(nullptr)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

Viewport &Application::screenViewport()
{
	return *screenViewport_;
}

IFrameTimer &Application::frameTimer()
{
	return *frameTimer_;
}

unsigned long int Application::numFrames() const
{
	return frameTimer_->totalNumberFrames();
}

float Application::interval() const
{
	return frameTimer_->lastFrameDuration();
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

void Application::initCommon()
{
	TracyGpuContext;
	ZoneScoped;
	profileStartTime_ = TimeStamp::now();

#ifdef WITH_GIT_VERSION
	appInfoString.format("nCine %s (%s) compiled on %s at %s", VersionStrings::Version, VersionStrings::GitBranch,
	                     VersionStrings::CompilationDate, VersionStrings::CompilationTime);
#else
	appInfoString.format("nCine compiled on %s at %s", VersionStrings::CompilationDate, VersionStrings::CompilationTime);
#endif
	LOGI_X("%s", appInfoString.data());
#ifdef WITH_TRACY
	TracyAppInfo(appInfoString.data(), appInfoString.length());
#endif

	theServiceLocator().registerIndexer(nctl::makeUnique<ArrayIndexer>());
#ifdef WITH_AUDIO
	if (appCfg_.withAudio)
		theServiceLocator().registerAudioDevice(nctl::makeUnique<ALAudioDevice>(appCfg_));
#endif
#ifdef WITH_THREADS
	if (appCfg_.withThreads)
		theServiceLocator().registerThreadPool(nctl::makeUnique<ThreadPool>());
#endif
	theServiceLocator().registerGfxCapabilities(nctl::makeUnique<GfxCapabilities>());
	GLDebug::init(theServiceLocator().gfxCapabilities());

	LOGI_X("Data path: \"%s\"", fs::dataPath().data());
	LOGI_X("Save path: \"%s\"", fs::savePath().data());
	LOGI_X("Cache path: \"%s\"", fs::cachePath().data());

#ifdef WITH_RENDERDOC
	RenderDocCapture::init();
#endif

	// Swapping frame now for a cleaner API trace capture when debugging
	gfxDevice_->update();
	FrameMark;
	TracyGpuCollect;

	frameTimer_ = nctl::makeUnique<FrameTimer>(appCfg_.profileTextUpdateTime(), appCfg_.frameTimerLogInterval);

	// Create a minimal set of render resources before compiling the first shader
	RenderResources::createMinimal(); // they are required for rendering even without a scenegraph

#ifdef WITH_IMGUI
	imguiDrawing_ = nctl::makeUnique<ImGuiDrawing>(appCfg_.withScenegraph);
#endif
#ifdef WITH_NUKLEAR
	nuklearDrawing_ = nctl::makeUnique<NuklearDrawing>(appCfg_.withScenegraph);
#endif

	if (appCfg_.withScenegraph)
	{
		gfxDevice_->setupGL();
		RenderResources::create();
		rootNode_ = nctl::makeUnique<SceneNode>();
		screenViewport_ = nctl::makeUnique<ScreenViewport>();
		screenViewport_->setRootNode(rootNode_.get());
	}

#ifdef WITH_IMGUI
	// Debug overlay is available even when scenegraph is not
	if (appCfg_.withDebugOverlay)
		debugOverlay_ = nctl::makeUnique<ImGuiDebugOverlay>(appCfg_.profileTextUpdateTime());
#endif

	// Initialization of the static random generator seeds
	random().init(static_cast<uint64_t>(TimeStamp::now().ticks()), static_cast<uint64_t>(profileStartTime_.ticks()));

	LOGI("Application initialized");

	timings_[Timings::INIT_COMMON] = profileStartTime_.secondsSince();

	{
		ZoneScopedN("onInit");
		profileStartTime_ = TimeStamp::now();
		appEventHandler_->onInit();
		timings_[Timings::APP_INIT] = profileStartTime_.secondsSince();
		LOGI("IAppEventHandler::onInit() invoked");
	}

	// Give user code a chance to add custom GUI fonts
#ifdef WITH_IMGUI
	imguiDrawing_->buildFonts();
#endif
#ifdef WITH_NUKLEAR
	nuklearDrawing_->bakeFonts();
#endif

	// Swapping frame now for a cleaner API trace capture when debugging
	gfxDevice_->update();
	FrameMark;
	TracyGpuCollect;
}

void Application::step()
{
	ZoneScoped;
	frameTimer_->addFrame();

#ifdef WITH_IMGUI
	{
		ZoneScopedN("ImGui newFrame");
		profileStartTime_ = TimeStamp::now();
		imguiDrawing_->newFrame();
		timings_[Timings::IMGUI] = profileStartTime_.secondsSince();
	}
#endif

#ifdef WITH_NUKLEAR
	{
		ZoneScopedN("Nuklear newFrame");
		profileStartTime_ = TimeStamp::now();
		nuklearDrawing_->newFrame();
		timings_[Timings::NUKLEAR] = profileStartTime_.secondsSince();
	}
#endif

#ifdef WITH_LUA
	LuaStatistics::update();
#endif

#if !defined(__EMSCRIPTEN__) && !defined(WITH_GLFW)
	if (gfxDevice_->backendScalesWindowSize_ == false)
	{
		// The backend does not scale the window size, the factor needs to be checked every frame
		updateScalingFactor();
	}
#endif

	{
		ZoneScopedN("onFrameStart");
		profileStartTime_ = TimeStamp::now();
		appEventHandler_->onFrameStart();
		timings_[Timings::FRAME_START] = profileStartTime_.secondsSince();
	}

	if (debugOverlay_)
		debugOverlay_->update();

	if (appCfg_.withScenegraph)
	{
		ZoneScopedN("SceneGraph");
		{
			ZoneScopedN("Update");
			profileStartTime_ = TimeStamp::now();
			screenViewport_->update();
			timings_[Timings::UPDATE] = profileStartTime_.secondsSince();
		}

		{
			ZoneScopedN("onPostUpdate");
			profileStartTime_ = TimeStamp::now();
			appEventHandler_->onPostUpdate();
			timings_[Timings::POST_UPDATE] = profileStartTime_.secondsSince();
		}

		{
			ZoneScopedN("Visit");
			profileStartTime_ = TimeStamp::now();
			screenViewport_->visit();
			timings_[Timings::VISIT] = profileStartTime_.secondsSince();
		}

#ifdef WITH_IMGUI
		{
			ZoneScopedN("ImGui endFrame");
			profileStartTime_ = TimeStamp::now();
			RenderQueue *imguiRenderQueue = (guiSettings_.imguiViewport) ?
			            guiSettings_.imguiViewport->renderQueue_.get() :
			            screenViewport_->renderQueue_.get();
			imguiDrawing_->endFrame(*imguiRenderQueue);
			timings_[Timings::IMGUI] += profileStartTime_.secondsSince();
		}
#endif

#ifdef WITH_NUKLEAR
		{
			ZoneScopedN("Nuklear endFrame");
			profileStartTime_ = TimeStamp::now();
			RenderQueue *nuklearRenderQueue = (guiSettings_.nuklearViewport) ?
			            guiSettings_.nuklearViewport->renderQueue_.get() :
			            screenViewport_->renderQueue_.get();
			nuklearDrawing_->endFrame(*nuklearRenderQueue);
			timings_[Timings::NUKLEAR] += profileStartTime_.secondsSince();
		}
#endif

		{
			ZoneScopedN("Draw");
			profileStartTime_ = TimeStamp::now();
			screenViewport_->sortAndCommitQueue();
			screenViewport_->draw();
			timings_[Timings::DRAW] = profileStartTime_.secondsSince();
		}
	}
	else
	{
#ifdef WITH_IMGUI
		{
			ZoneScopedN("ImGui endFrame");
			profileStartTime_ = TimeStamp::now();
			imguiDrawing_->endFrame();
			timings_[Timings::IMGUI] += profileStartTime_.secondsSince();
		}
#endif

#ifdef WITH_NUKLEAR
		{
			ZoneScopedN("Nuklear endFrame");
			profileStartTime_ = TimeStamp::now();
			nuklearDrawing_->endFrame();
			timings_[Timings::NUKLEAR] += profileStartTime_.secondsSince();
		}
#endif
	}

	{
		ZoneScopedN("Audio");
		theServiceLocator().audioDevice().updatePlayers();
	}

	{
		ZoneScopedN("onFrameEnd");
		profileStartTime_ = TimeStamp::now();
		appEventHandler_->onFrameEnd();
		timings_[Timings::FRAME_END] = profileStartTime_.secondsSince();
	}

	if (debugOverlay_)
		debugOverlay_->updateFrameTimings();

	gfxDevice_->update();
	FrameMark;
	TracyGpuCollect;

	if (appCfg_.frameLimit > 0)
	{
		const float frameTimeDuration = 1.0f / static_cast<float>(appCfg_.frameLimit);
		while (frameTimer_->currentFrameDuration() < frameTimeDuration)
			Timer::sleep(0);
	}
}

void Application::shutdownCommon()
{
	ZoneScoped;
	{
		ZoneScopedN("onShutdown");
		appEventHandler_->onShutdown();
		LOGI("IAppEventHandler::onShutdown() invoked");
		appEventHandler_.reset(nullptr);
	}

#ifdef WITH_NUKLEAR
	nuklearDrawing_.reset(nullptr);
#endif
#ifdef WITH_IMGUI
	imguiDrawing_.reset(nullptr);
#endif

#ifdef WITH_RENDERDOC
	RenderDocCapture::removeHooks();
#endif

	debugOverlay_.reset(nullptr);
	rootNode_.reset(nullptr);
	RenderResources::dispose();
	frameTimer_.reset(nullptr);
	inputManager_.reset(nullptr);
	gfxDevice_.reset(nullptr);

	if (theServiceLocator().indexer().isEmpty() == false)
	{
		LOGW_X("The object indexer is not empty, %u object(s) left", theServiceLocator().indexer().size());
		theServiceLocator().indexer().logReport();
	}

	LOGI("Application shut down");

	theServiceLocator().unregisterAll();
}

void Application::setFocus(bool hasFocus)
{
#if defined(WITH_TRACY) && !defined(__ANDROID__)
	hasFocus = true;
#endif

	hasFocus_ = hasFocus;
}

void Application::suspend()
{
	ZoneScopedN("onSuspend");
	theServiceLocator().audioDevice().pauseDevice();
	frameTimer_->suspend();
	if (appEventHandler_)
		appEventHandler_->onSuspend();
	LOGI("IAppEventHandler::onSuspend() invoked");
}

void Application::resume()
{
	ZoneScopedN("onResume");
	if (appEventHandler_)
		appEventHandler_->onResume();
	theServiceLocator().audioDevice().resumeDevice();
	const TimeStamp suspensionDuration = frameTimer_->resume();
	LOGV_X("Suspended for %.3f seconds", suspensionDuration.seconds());
	profileStartTime_ += suspensionDuration;
	LOGI("IAppEventHandler::onResume() invoked");
}

/*! \note It will also call the `onResizeWindow()` callback if the size has really changed */
bool Application::resizeScreenViewport(int width, int height)
{
	if (screenViewport_ != nullptr)
	{
		const bool shouldResize = (width != screenViewport_->width_ || height != screenViewport_->height_);
		if (width > 0 && height > 0 && shouldResize)
		{
			screenViewport_->resize(width, height);
			appEventHandler_->onResizeWindow(width, height);
		}
		return shouldResize;
	}
	else
	{
		appEventHandler_->onResizeWindow(width, height);
		return false;
	}
}

/*! \note It will also call the `onChangeScalingFactor()` callback if the factor has really changed */
bool Application::updateScalingFactor()
{
	const bool scalingChanged = gfxDevice_->scaleWindowSize(appCfg_.windowScaling);
	if (scalingChanged)
		appEventHandler_->onChangeScalingFactor(gfxDevice_->windowScalingFactor());
	return scalingChanged;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool Application::shouldSuspend()
{
	return (!hasFocus_ && autoSuspension_) || isSuspended_;
}

}
