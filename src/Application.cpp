#include <ctime>
#include "Application.h"
#include "IAppEventHandler.h"
#include "ServiceLocator.h"
#include "IFile.h"
#include "ArrayIndexer.h"
#include "GfxCapabilities.h"
#include "RenderResources.h"
#include "FrameTimer.h"
#include "LinePlotter.h"
#include "StackedBarPlotter.h"
#include "Font.h"
#include "TextNode.h"
#include "ncString.h"
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
	  frameTimer_(NULL), gfxDevice_(NULL),
	  renderQueue_(NULL), rootNode_(NULL),
	  profileTimer_(NULL), profilePlotter_(NULL),
	  font_(NULL), textLines_(NULL), textUpdateTime_(0.0f),
	  textString_(MaxTextLength), inputManager_(NULL), appEventHandler_(NULL)
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
	bool paused = !isPaused_;
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

	theServiceLocator().registerIndexer(new ArrayIndexer());
#ifdef WITH_AUDIO
	if (appCfg_.withAudio_)
	{
		theServiceLocator().registerAudioDevice(new ALAudioDevice());
	}
#endif
#ifdef WITH_THREADS
	if (appCfg_.withThreads_)
	{
		theServiceLocator().registerThreadPool(new ThreadPool());
	}
#endif
	theServiceLocator().registerGfxCapabilities(new GfxCapabilities());

	LOGI_X("Data path: \"%s\"", IFile::dataPath().data());
	LOGI_X("Save path: \"%s\"", IFile::savePath().data());

	frameTimer_ = new FrameTimer(appCfg_.frameTimerLogInterval_, appCfg_.profileTextUpdateTime_);
	profileTimer_ = new Timer();

	if (appCfg_.withScenegraph_)
	{
		gfxDevice_->setupGL();
		RenderResources::create();
		renderQueue_ = new RenderQueue();
		rootNode_ = new SceneNode();

		if (appCfg_.withProfilerGraphs_)
		{
			profilePlotter_ = new StackedBarPlotter(rootNode_, Rectf(width() * 0.1f, height() * 0.1f, width() * 0.8f, height() * 0.15f));
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

		if (appCfg_.withProfilerText_)
		{
			String fontTexFilePath = IFile::dataPath() + appCfg_.fontTexFilename_;
			String fontFntFilePath = IFile::dataPath() + appCfg_.fontFntFilename_;
			if (IFile::access(fontTexFilePath.data(), IFile::MODE_EXISTS) == false)
			{
				LOGW_X("Cannot access font texture file \"%s\" to enable profiling text", fontTexFilePath.data());
			}
			else if (IFile::access(fontFntFilePath.data(), IFile::MODE_EXISTS) == false)
			{
				LOGW_X("Cannot access font FNT file \"%s\" to enable profiling text", fontFntFilePath.data());
			}
			else
			{
				font_ = new Font(fontTexFilePath.data(), fontFntFilePath.data());
				textLines_ = new TextNode(rootNode_, font_);
				textLines_->setPosition(0.0f, height());
			}
		}
	}

	LOGI("Application initialized");

	appEventHandler_->onInit();
	LOGI("IAppEventHandler::OnInit() invoked");

	// HACK: Init of the random seed
	// In the future there could be a random generator service
	srand(static_cast<unsigned int>(time(NULL)));
}

void Application::step()
{
	frameTimer_->addFrame();
	if (appCfg_.withScenegraph_)
	{
		gfxDevice_->clear();
	}
	appEventHandler_->onFrameStart();
	if (profilePlotter_)
	{
		// Measuring OnFrameEnd() + OnFrameStart() time
		profilePlotter_->addValue(0, profileTimer_->interval());
	}

	profileTimer_->start();
	if (rootNode_ != NULL && renderQueue_ != NULL)
	{
		rootNode_->update(frameTimer_->interval());
		rootNode_->visit(*renderQueue_);
		renderQueue_->draw();
	}

	if (renderQueue_ && textLines_ && Timer::now() - textUpdateTime_ > appCfg_.profileTextUpdateTime_)
	{
		textUpdateTime_ = Timer::now();
		textString_.format(static_cast<const char *>("FPS: %.0f (%.2fms)\nSprites: %uV, %uDC\nParticles: %uV, %uDC\nText: %uV, %uDC\nPlotter: %uV, %uDC\nTotal: %uV, %uDC"),
		                   frameTimer_->averageFps(), frameTimer_->interval() * 1000.0f,
		                   renderQueue_->numVertices(RenderCommand::SPRITE_TYPE), renderQueue_->numCommands(RenderCommand::SPRITE_TYPE),
		                   renderQueue_->numVertices(RenderCommand::PARTICLE_TYPE), renderQueue_->numCommands(RenderCommand::PARTICLE_TYPE),
		                   renderQueue_->numVertices(RenderCommand::TEXT_TYPE), renderQueue_->numCommands(RenderCommand::TEXT_TYPE),
		                   renderQueue_->numVertices(RenderCommand::PLOTTER_TYPE), renderQueue_->numCommands(RenderCommand::PLOTTER_TYPE),
		                   renderQueue_->numVertices(), renderQueue_->numCommands());

		textLines_->setString(textString_);
		textLines_->setAlignment(TextNode::ALIGN_RIGHT);
		textLines_->setPosition(width() - textLines_->width() * 0.5f, height() - textLines_->height() * 0.5f);
	}

	theServiceLocator().audioDevice().updatePlayers();
	if (profilePlotter_)
	{
		// Measuring scenegraph update and visit + draw + audio update
		profilePlotter_->addValue(1, profileTimer_->interval());
	}

	profileTimer_->start();
	gfxDevice_->update();
	if (profilePlotter_)
	{
		// Measuring swap buffers time
		profilePlotter_->addValue(2, profileTimer_->interval());
	}

	profileTimer_->start();
	appEventHandler_->onFrameEnd();
}

void Application::shutdownCommon()
{
	appEventHandler_->onShutdown();
	LOGI("IAppEventHandler::OnShutdown() invoked");

	if (appEventHandler_)
	{
		delete appEventHandler_;
	}

	delete textLines_;
	delete font_;
	delete profilePlotter_;
	delete profileTimer_;
	delete rootNode_; // deletes every child too
	delete renderQueue_;
	RenderResources::dispose();
	delete frameTimer_;
	delete inputManager_;
	delete gfxDevice_;

	if (theServiceLocator().indexer().isEmpty() == false)
	{
		LOGW_X("The object indexer is not empty, %u object(s) left", theServiceLocator().indexer().size());
		theServiceLocator().indexer().logReport();
	}

	LOGI("Application shutted down");

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
