#include <ctime>
#include "Application.h"
#include "IAppEventHandler.h"
#include "IFile.h"
#include "ArrayIndexer.h"
#include "GfxCapabilities.h"
#include "RenderStatistics.h"
#include "RenderResources.h"
#include "GLDebug.h"
#include "FrameTimer.h"
#include "LinePlotter.h"
#include "StackedBarPlotter.h"
#include "Font.h"
#include "TextNode.h"
#include "nctl/String.h"
#include "IInputManager.h"
#include "JoyMapping.h"

#ifdef WITH_AUDIO
	#include "ALAudioDevice.h"
#endif

#ifdef WITH_THREADS
	#include "ThreadPool.h"
#endif

#ifdef WITH_GIT_VERSION
	#include "version.h"
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
///////////////////////////////////////////////////////////

Application::Application()
	: isPaused_(false), hasFocus_(true), shouldQuit_(false),
	  textUpdateTime_(0.0f), textString_(MaxTextLength), textString2_(MaxTextLength)
{

}

Application::~Application()
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

float Application::interval() const
{
	return frameTimer_->interval();
}

void Application::setPause(bool paused)
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
	setPause(paused);
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

void Application::initCommon()
{
#ifdef WITH_GIT_VERSION
	LOGI_X("nCine %s compiled on " __DATE__ " at " __TIME__, VersionStrings::Version);
#else
	LOGI("nCine compiled on " __DATE__ " at " __TIME__);
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
	profileTimer_ = nctl::makeUnique<Timer>();

	if (appCfg_.withScenegraph())
	{
		gfxDevice_->setupGL();
		RenderResources::create();
		renderQueue_ = nctl::makeUnique<RenderQueue>();
		rootNode_ = nctl::makeUnique<SceneNode>();

		if (appCfg_.withProfilerGraphs())
		{
			profilePlotter_ = nctl::makeUnique<StackedBarPlotter>(rootNode_.get(), Rectf(width() * 0.1f, height() * 0.1f, width() * 0.8f, height() * 0.15f));
			profilePlotter_->setBackgroundColor(Color(0.35f, 0.35f, 0.45f, 0.5f));
			profilePlotter_->addVariable(50, 0.2f);
			profilePlotter_->variable(0).setGraphColor(Color(0.8f, 0.0f, 0.0f));
			profilePlotter_->variable(0).setMeanColor(Color(1.0f, 0.0f, 0.0f));
			profilePlotter_->addVariable(50, 0.2f);
			profilePlotter_->variable(1).setGraphColor(Color(0.0f, 0.8f, 0.0f));
			profilePlotter_->variable(1).setMeanColor(Color(0.0f, 1.0f, 0.0f));
			profilePlotter_->addVariable(50, 0.2f);
			profilePlotter_->variable(2).setGraphColor(Color(0.0f, 0.0f, 0.8f));
			profilePlotter_->variable(2).setMeanColor(Color(0.0f, 0.0f, 1.0f));

			profilePlotter_->variable(0).setPlotMean(false);
			profilePlotter_->variable(1).setPlotMean(false);
			profilePlotter_->variable(2).setPlotMean(false);
			profilePlotter_->setPlotRefValue(true);
			profilePlotter_->setRefValue(1.0f / 60.0f); // 60 FPS
		}

		if (appCfg_.withProfilerText())
		{
			nctl::String fontTexFilePath = IFile::dataPath() + appCfg_.fontTexFilename();
			nctl::String fontFntFilePath = IFile::dataPath() + appCfg_.fontFntFilename();
			if (IFile::access(fontTexFilePath.data(), IFile::AccessMode::EXISTS) == false)
				LOGW_X("Cannot access font texture file \"%s\" to enable profiling text", fontTexFilePath.data());
			else if (IFile::access(fontFntFilePath.data(), IFile::AccessMode::EXISTS) == false)
				LOGW_X("Cannot access font FNT file \"%s\" to enable profiling text", fontFntFilePath.data());
			else
			{
				font_ = nctl::makeUnique<Font>(fontTexFilePath.data(), fontFntFilePath.data());
				textLines_ = nctl::makeUnique<TextNode>(rootNode_.get(), font_.get());
				textLines_->setAlignment(TextNode::Alignment::RIGHT);
				textLines2_ = nctl::makeUnique<TextNode>(rootNode_.get(), font_.get());
				textLines2_->setAlignment(TextNode::Alignment::LEFT);
			}
		}
	}
	else
		RenderResources::createMinimal(); // some resources are still required for rendering

	LOGI("Application initialized");

	appEventHandler_->onInit();
	LOGI("IAppEventHandler::OnInit() invoked");

	// HACK: Init of the random seed
	// In the future there could be a random generator service
	srand(static_cast<unsigned int>(time(nullptr)));

	// Swapping frame now for a cleaner API trace capture when debugging
	gfxDevice_->update();
}

void Application::step()
{
	frameTimer_->addFrame();
	if (appCfg_.withScenegraph())
		gfxDevice_->clear();
	appEventHandler_->onFrameStart();
	// Measuring OnFrameEnd() + OnFrameStart() time
	if (profilePlotter_)
		profilePlotter_->addValue(0, profileTimer_->interval());

	if (profilePlotter_)
	{
		profilePlotter_->enableUpdate(renderingSettings_.showProfilerGraphs);
		profilePlotter_->enableDraw(renderingSettings_.showProfilerGraphs);
	}

	if (textLines_ && textLines2_)
	{
		textLines_->enableUpdate(renderingSettings_.showProfilerText);
		textLines_->enableDraw(renderingSettings_.showProfilerText);
		textLines2_->enableUpdate(renderingSettings_.showProfilerText);
		textLines2_->enableDraw(renderingSettings_.showProfilerText);
	}

	profileTimer_->start();
	if (rootNode_ != nullptr && renderQueue_ != nullptr)
	{
		rootNode_->update(frameTimer_->interval());
		rootNode_->visit(*renderQueue_);
		renderQueue_->draw();
	}

	if (renderQueue_ && textLines_ && textLines2_ && Timer::now() - textUpdateTime_ > appCfg_.profileTextUpdateTime())
	{
		textUpdateTime_ = Timer::now();

		textString_.format("FPS: %.0f (%.2fms)\n", frameTimer_->averageFps(), frameTimer_->interval() * 1000.0f);
		RenderStatistics::appendCommandsStatistics(textString_);
		textLines_->setString(textString_);
		textLines_->setPosition(width() - textLines_->width() * 0.5f, height() - textLines_->height() * 0.5f);

		textString2_.clear();
		RenderStatistics::appendMoreStatistics(textString2_);
		textLines2_->setString(textString2_);
		textLines2_->setPosition(textLines2_->width() * 0.5f, height() - textLines2_->height() * 0.5f);
	}

	theServiceLocator().audioDevice().updatePlayers();
	// Measuring scenegraph update and visit + draw + audio update
	if (profilePlotter_)
		profilePlotter_->addValue(1, profileTimer_->interval());

	profileTimer_->start();
	gfxDevice_->update();
	// Measuring swap buffers time
	if (profilePlotter_)
		profilePlotter_->addValue(2, profileTimer_->interval());

	profileTimer_->start();
	appEventHandler_->onFrameEnd();
}

void Application::shutdownCommon()
{
	appEventHandler_->onShutdown();
	LOGI("IAppEventHandler::OnShutdown() invoked");
	appEventHandler_.reset(nullptr);

	textLines2_.reset(nullptr);
	textLines_.reset(nullptr);
	font_.reset(nullptr);
	profilePlotter_.reset(nullptr);
	profileTimer_.reset(nullptr);
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
