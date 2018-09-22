#include <ctime>
#include "Application.h"
#include "IAppEventHandler.h"
#include "IFile.h"
#include "ArrayIndexer.h"
#include "GfxCapabilities.h"
#include "RenderResources.h"
#include "RenderQueue.h"
#include "GLDebug.h"
#include "FrameTimer.h"
#include "SceneNode.h"
#include "nctl/String.h"
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
#else
	#include "DebugOverlay.h"
#endif

#include "version.h"

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

float Application::interval() const
{
	return frameTimer_->interval();
}

void Application::setPaused(bool paused)
{
	isPaused_ = paused;
	if (isPaused_ == false)
	{
		frameTimer_->start();
		profileTimer_->start();
	}
}

void Application::togglePause()
{
	const bool paused = !isPaused_;
	setPaused(paused);
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

void Application::initCommon()
{
	profileTimer_->start();

#ifdef WITH_GIT_VERSION
	LOGI_X("nCine %s (%s) compiled on %s at %s" , VersionStrings::Version, VersionStrings::GitBranch,
		VersionStrings::CompilationDate, VersionStrings::CompilationTime);
#else
	LOGI_X("nCine compiled on %s at %s", VersionStrings::CompilationDate, VersionStrings::CompilationTime);
#endif

	theServiceLocator().registerIndexer(nctl::makeUnique<ArrayIndexer>());
#ifdef WITH_AUDIO
	if (appCfg_.withAudio())
		theServiceLocator().registerAudioDevice(nctl::makeUnique<ALAudioDevice>());
#endif
#ifdef WITH_THREADS
	if (appCfg_.withThreads())
		theServiceLocator().registerThreadPool(nctl::makeUnique<ThreadPool>());
#endif
	theServiceLocator().registerGfxCapabilities(nctl::makeUnique<GfxCapabilities>());
	GLDebug::init(theServiceLocator().gfxCapabilities());

	LOGI_X("Data path: \"%s\"", IFile::dataPath().data());
	LOGI_X("Save path: \"%s\"", IFile::savePath().data());

	// Swapping frame now for a cleaner API trace capture when debugging
	gfxDevice_->update();

	frameTimer_ = nctl::makeUnique<FrameTimer>(appCfg_.frameTimerLogInterval(), appCfg_.profileTextUpdateTime());

#ifdef WITH_IMGUI
	imguiDrawing_ = nctl::makeUnique<ImGuiDrawing>(appCfg_.withScenegraph());
#endif

	if (appCfg_.withScenegraph())
	{
		gfxDevice_->setupGL();
		RenderResources::create();
		renderQueue_ = nctl::makeUnique<RenderQueue>();
		rootNode_ = nctl::makeUnique<SceneNode>();

		if (appCfg_.withProfilerGraphs() || appCfg_.withInfoText())
		{
#ifdef WITH_IMGUI
			debugOverlay_ = nctl::makeUnique<ImGuiDebugOverlay>(appCfg_);
#else
			debugOverlay_ = nctl::makeUnique<DebugOverlay>(appCfg_);
#endif
		}
	}
	else
		RenderResources::createMinimal(); // some resources are still required for rendering

	// HACK: Init of the random seed
	// In the future there could be a random generator service
	srand(static_cast<unsigned int>(time(nullptr)));

	LOGI("Application initialized");

	timings_[Timings::INIT_COMMON] = profileTimer_->interval();

	profileTimer_->start();
	appEventHandler_->onInit();
	timings_[Timings::APP_INIT] = profileTimer_->interval();
	LOGI("IAppEventHandler::OnInit() invoked");

	// Swapping frame now for a cleaner API trace capture when debugging
	gfxDevice_->update();
}

void Application::step()
{
	frameTimer_->addFrame();
	if (appCfg_.withScenegraph())
		gfxDevice_->clear();

#ifdef WITH_IMGUI
	profileTimer_->start();
	imguiDrawing_->newFrame();
	timings_[Timings::IMGUI] = profileTimer_->interval();
#endif

#ifdef WITH_LUA
	LuaStatistics::update();
#endif

	profileTimer_->start();
	appEventHandler_->onFrameStart();
	timings_[Timings::FRAME_START] = profileTimer_->interval();

	if (debugOverlay_)
		debugOverlay_->update();

	if (rootNode_ != nullptr && renderQueue_ != nullptr)
	{
		profileTimer_->start();
		rootNode_->update(frameTimer_->interval());
		timings_[Timings::UPDATE] = profileTimer_->interval();

		profileTimer_->start();
		rootNode_->visit(*renderQueue_);
		timings_[Timings::VISIT] = profileTimer_->interval();

		profileTimer_->start();
		renderQueue_->draw();
		timings_[Timings::DRAW] = profileTimer_->interval();
	}

	theServiceLocator().audioDevice().updatePlayers();

#ifdef WITH_IMGUI
	profileTimer_->start();
	if (appCfg_.withScenegraph())
		imguiDrawing_->endFrame(*renderQueue_);
	else
		imguiDrawing_->endFrame();
	timings_[Timings::IMGUI] += profileTimer_->interval();
#endif

	gfxDevice_->update();

	profileTimer_->start();
	appEventHandler_->onFrameEnd();
	timings_[Timings::FRAME_END] = profileTimer_->interval();

	if (debugOverlay_ && appCfg_.withProfilerGraphs())
		debugOverlay_->updateFrameTimings();
}

void Application::shutdownCommon()
{
	appEventHandler_->onShutdown();
	LOGI("IAppEventHandler::OnShutdown() invoked");
	appEventHandler_.reset(nullptr);

#ifdef WITH_IMGUI
	imguiDrawing_.reset(nullptr);
#endif

	debugOverlay_.reset(nullptr);
	rootNode_.reset(nullptr);
	renderQueue_.reset(nullptr);
	RenderResources::dispose();
	frameTimer_.reset(nullptr);
	inputManager_.reset(nullptr);
	gfxDevice_.reset(nullptr);
	profileTimer_.reset(nullptr);

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
	// Check if a focus event has occurred
	if (hasFocus_ != hasFocus)
	{
		hasFocus_ = hasFocus;
		// Check if focus has been gained
		if (hasFocus == true)
		{
			frameTimer_->start();
			profileTimer_->start();
		}
	}
}

}
