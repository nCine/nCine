#include <ctime>
#include "Application.h"
#include "AppConfiguration.h"
#include "IAppEventHandler.h"
#include "ServiceLocator.h"
#include "ArrayIndexer.h"
#include "GfxCapabilities.h"
#include "RenderResources.h"
#include "FrameTimer.h"
#include "FileLogger.h"
#include "LinePlotter.h"
#include "StackedBarPlotter.h"
#include "Font.h"
#include "TextNode.h"
#include "ncString.h"

#ifdef WITH_AUDIO
	#include "ALAudioDevice.h"
#endif

#ifdef WITH_THREADS
	#include "ThreadPool.h"
#endif

#include "IInputManager.h"
#if defined(WITH_SDL)
	#include "SdlGfxDevice.h"
	#include "SdlInputManager.h"
#elif defined(WITH_GLFW)
	#include "GlfwGfxDevice.h"
	#include "GlfwInputManager.h"
#endif

namespace ncine {

#ifndef __ANDROID__
/// Meyers' Singleton
Application& theApplication()
{
	static Application instance;
	return instance;
}
#endif

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

/// Must be called at start to init the application
void Application::init(IAppEventHandler* (*createAppEventHandler)())
{
	appEventHandler_ = createAppEventHandler();
	appEventHandler_->onPreInit(appCfg_);

	// Registering the logger as early as possible
	String logFilePath = IFile::dataPath() + appCfg_.logFile_;
	theServiceLocator().registerLogger(new FileLogger(logFilePath.data(), appCfg_.consoleLogLevel_, appCfg_.fileLogLevel_));
	// Graphics device should always be created before the input manager!
	DisplayMode displayMode(8, 8, 8, 8, 24, 8, true, false);
#if defined(WITH_SDL)
	gfxDevice_ = new SdlGfxDevice(appCfg_.xResolution_, appCfg_.yResolution_, displayMode, appCfg_.inFullscreen_);
	inputManager_ = new SdlInputManager();
#elif defined(WITH_GLFW)
	gfxDevice_ = new GlfwGfxDevice(appCfg_.xResolution_, appCfg_.yResolution_, displayMode, appCfg_.inFullscreen_);
	inputManager_ = new GlfwInputManager();
#endif
	gfxDevice_->setWindowTitle(appCfg_.windowTitle_.data());

	initCommon();
}

/// The main game loop, handling events and rendering
void Application::run()
{
#ifndef __ANDROID__
	while (!shouldQuit_)
	{
#if defined(WITH_SDL)
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					shouldQuit_ = true;
					break;
				case SDL_ACTIVEEVENT:
					if (event.active.state != SDL_APPMOUSEFOCUS)
					{
						setFocus(event.active.gain);
					}
					break;
				default:
					SdlInputManager::parseEvent(event);
					break;
			}
		}
#elif defined(WITH_GLFW)
		setFocus(GlfwInputManager::hasFocus());
		glfwPollEvents();
		GlfwInputManager::updateJoystickStates();
#endif

		if (hasFocus_ && !isPaused_)
		{
			step();
		}
	}
#endif
}

/// A single step of the game loop made to render a frame
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

	if (textLines_ && Timer::now() - textUpdateTime_ > appCfg_.profileTextUpdateTime_)
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
		textLines_->setPosition((width() - textLines_->width()), height());
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

/// Must be called before exiting to shut down the application
void Application::shutdown()
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
		LOGW("The object indexer is not empty");
	}

	LOGI("Application shutted down");

	theServiceLocator().unregisterAll();
}

/// Returns the elapsed time since the end of the previous frame in milliseconds
float Application::interval() const
{
	return frameTimer_->interval();
}

/// Sets the pause flag value
void Application::setPause(bool paused)
{
	isPaused_ = paused;
	if (isPaused_ == false)
	{
		frameTimer_->start();
		profileTimer_->start();
	}
}

/// Toggles the pause flag on and off
void Application::togglePause()
{
	bool paused = !isPaused_;
	setPause(paused);
}

/// Sets the focus flag
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

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

void Application::initCommon()
{
	LOGI("nCine compiled on " __DATE__ " at " __TIME__);
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
	LOGI_X("Data path: %s", IFile::dataPath().data());
	LOGI_X("Save path: %s", IFile::savePath().data());

	frameTimer_ = new FrameTimer(appCfg_.frameTimerLogInterval_, appCfg_.profileTextUpdateTime_);
	profileTimer_ = new Timer();

	GfxCapabilities& gfxCaps = const_cast<GfxCapabilities&>(theServiceLocator().gfxCapabilities());
	gfxCaps.init();

	if (appCfg_.withScenegraph_)
	{
		gfxDevice_->setupGL();
		RenderResources::create();
		renderQueue_ = new RenderQueue();
		rootNode_ = new SceneNode();

		if (appCfg_.withProfilerGraphs_)
		{
			profilePlotter_ = new StackedBarPlotter(rootNode_, Rect(width() * 0.1f, height() * 0.1f, width() * 0.8f, height() * 0.15f));
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
			if (IFile::access(fontTexFilePath.data(), IFile::MODE_EXISTS) && IFile::access(fontFntFilePath.data(), IFile::MODE_EXISTS))
			{
				font_ = new Font(fontTexFilePath.data(), fontFntFilePath.data());
				textLines_ = new TextNode(rootNode_, font_);
				textLines_->setPosition(0.0f, height());
			}
			else
			{
				LOGW("Cannot access font files for profiling text");
			}
		}
	}

	LOGI("Application initialized");

	appEventHandler_->onInit();
	LOGI("IAppEventHandler::OnInit() invoked");

	// HACK: Init of the random seed
	// In the future there could be a random generator service
	srand(time(NULL));
}

}
