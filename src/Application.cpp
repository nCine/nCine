#include "Application.h"
#include "Random.h"
#include "IAppEventHandler.h"
#include "FileSystem.h"
#include "IFile.h"
#include "ArrayIndexer.h"
#include "GfxCapabilities.h"
#include "RenderResources.h"
#include "RenderQueue.h"
#include "GLDebug.h"
#include "Timer.h" // for `sleep()`
#include "FrameTimer.h"
#include "SceneNode.h"
#include <nctl/String.h>
#include "IInputManager.h"
#include "JoyMapping.h"

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

#include "tracy_opengl.h"

#ifdef WITH_RENDERDOC
	#include "RenderDocCapture.h"
#endif

#include "version.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Application::Application()
    : isSuspended_(false), autoSuspension_(true), hasFocus_(true), shouldQuit_(false)
{
}

Application::~Application() = default;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned long int Application::numFrames() const
{
	return frameTimer_->totalNumberFrames();
}

float Application::interval() const
{
	return frameTimer_->lastFrameInterval();
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
	LOGI_X("nCine %s (%s) compiled on %s at %s", VersionStrings::Version, VersionStrings::GitBranch,
	       VersionStrings::CompilationDate, VersionStrings::CompilationTime);
#else
	LOGI_X("nCine compiled on %s at %s", VersionStrings::CompilationDate, VersionStrings::CompilationTime);
#endif

	theServiceLocator().registerIndexer(nctl::makeUnique<ArrayIndexer>());
#ifdef WITH_AUDIO
	if (appCfg_.withAudio)
		theServiceLocator().registerAudioDevice(nctl::makeUnique<ALAudioDevice>());
#endif
#ifdef WITH_THREADS
	if (appCfg_.withThreads)
		theServiceLocator().registerThreadPool(nctl::makeUnique<ThreadPool>());
#endif
	theServiceLocator().registerGfxCapabilities(nctl::makeUnique<GfxCapabilities>());
	GLDebug::init(theServiceLocator().gfxCapabilities());

	LOGI_X("Data path: \"%s\"", fs::dataPath().data());
	LOGI_X("Save path: \"%s\"", fs::savePath().data());

#ifdef WITH_RENDERDOC
	RenderDocCapture::init();
#endif

	// Swapping frame now for a cleaner API trace capture when debugging
	gfxDevice_->update();
	FrameMark;
	TracyGpuCollect;

	frameTimer_ = nctl::makeUnique<FrameTimer>(appCfg_.frameTimerLogInterval, appCfg_.profileTextUpdateTime());

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
		renderQueue_ = nctl::makeUnique<RenderQueue>();
		rootNode_ = nctl::makeUnique<SceneNode>();
	}
	else
		RenderResources::createMinimal(); // some resources are still required for rendering

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
	if (appCfg_.withScenegraph)
	{
		TracyGpuZone("Clear");
		gfxDevice_->clear();
	}

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

	{
		ZoneScopedN("onFrameStart");
		profileStartTime_ = TimeStamp::now();
		appEventHandler_->onFrameStart();
		timings_[Timings::FRAME_START] = profileStartTime_.secondsSince();
	}

	if (debugOverlay_)
		debugOverlay_->update();

	if (rootNode_ != nullptr && renderQueue_ != nullptr)
	{
		ZoneScopedN("SceneGraph");
		{
			ZoneScopedN("Update");
			profileStartTime_ = TimeStamp::now();
			rootNode_->update(frameTimer_->lastFrameInterval());
			timings_[Timings::UPDATE] = profileStartTime_.secondsSince();
		}

		{
			ZoneScopedN("Visit");
			profileStartTime_ = TimeStamp::now();
			rootNode_->visit(*renderQueue_);
			timings_[Timings::VISIT] = profileStartTime_.secondsSince();
		}

		{
			ZoneScopedN("Draw");
			profileStartTime_ = TimeStamp::now();
			renderQueue_->draw();
			timings_[Timings::DRAW] = profileStartTime_.secondsSince();
		}
	}

	{
		ZoneScopedN("Audio");
		theServiceLocator().audioDevice().updatePlayers();
	}

#ifdef WITH_IMGUI
	{
		ZoneScopedN("ImGui endFrame");
		profileStartTime_ = TimeStamp::now();
		if (appCfg_.withScenegraph)
			imguiDrawing_->endFrame(*renderQueue_);
		else
			imguiDrawing_->endFrame();
		timings_[Timings::IMGUI] += profileStartTime_.secondsSince();
	}
#endif

#ifdef WITH_NUKLEAR
	{
		ZoneScopedN("Nuklear endFrame");
		profileStartTime_ = TimeStamp::now();
		if (appCfg_.withScenegraph)
			nuklearDrawing_->endFrame(*renderQueue_);
		else
			nuklearDrawing_->endFrame();
		timings_[Timings::NUKLEAR] += profileStartTime_.secondsSince();
	}
#endif

	gfxDevice_->update();
	FrameMark;
	TracyGpuCollect;

	{
		ZoneScopedN("onFrameEnd");
		profileStartTime_ = TimeStamp::now();
		appEventHandler_->onFrameEnd();
		timings_[Timings::FRAME_END] = profileStartTime_.secondsSince();
	}

	if (debugOverlay_)
		debugOverlay_->updateFrameTimings();

	if (appCfg_.frameLimit > 0)
	{
		const float frameTimeDuration = 1.0f / static_cast<float>(appCfg_.frameLimit);
		while (frameTimer_->frameInterval() < frameTimeDuration)
			Timer::sleep(0.0f);
	}
}

void Application::shutdownCommon()
{
	ZoneScoped;
	{
		ZoneScopedN("onShutdown");
		appEventHandler_->onShutdown();
		LOGI("IAppEventHandler::OnShutdown() invoked");
		appEventHandler_.reset(nullptr);
	}

#ifdef WITH_NUKLEAR
	nuklearDrawing_.reset(nullptr);
#endif
#ifdef WITH_IMGUI
	imguiDrawing_.reset(nullptr);
#endif

#ifdef WITH_RENDERDOC
	RenderDocCapture::shutdown();
#endif

	debugOverlay_.reset(nullptr);
	rootNode_.reset(nullptr);
	renderQueue_.reset(nullptr);
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
	const TimeStamp suspensionDuration = frameTimer_->resume();
	LOGV_X("Suspended for %.3f seconds", suspensionDuration.seconds());
	profileStartTime_ += suspensionDuration;
	LOGI("IAppEventHandler::onResume() invoked");
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool Application::shouldSuspend()
{
	return (!hasFocus_ && autoSuspension_) || isSuspended_;
}

}
