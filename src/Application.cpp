#include <ctime>
#include "Application.h"
#include "IAppEventHandler.h"
#include "ServiceLocator.h"
#include "ArrayIndexer.h"
#include "FrameTimer.h"
#include "FileLogger.h"
#include "LinePlotter.h"
#include "StackedBarPlotter.h"
#include "Font.h"
#include "TextNode.h"

#ifdef WITH_AUDIO
	#include "ALAudioDevice.h"
#endif

#ifdef WITH_THREADS
	#include "ThreadPool.h"
#endif

#if defined(__ANDROID__)
	#include "EglGfxDevice.h"
	#include "AndroidInputManager.h"
	#include "AssetFile.h"
#elif defined(WITH_SDL)
	#include "SdlGfxDevice.h"
	#include "SdlInputManager.h"
#elif defined(WITH_GLFW)
	#include "GlfwGfxDevice.h"
	#include "GlfwInputManager.h"
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool Application::isPaused_ = true;
bool Application::hasFocus_ = true;
bool Application::shouldQuit_ = false;
FrameTimer *Application::frameTimer_ = NULL;
IGfxDevice *Application::gfxDevice_ = NULL;
SceneNode *Application::rootNode_ = NULL;
RenderQueue *Application::renderQueue_ = NULL;
Timer *Application::profileTimer_ = NULL;
ProfilePlotter *Application::profilePlotter_ = NULL;
Font *Application::font_ = NULL;
TextNode *Application::textLines_ = NULL;
float Application::textUpdateTime = 0.0f;
char Application::textChars_[256] = "";
IInputManager *Application::inputManager_ = NULL;
IAppEventHandler *Application::appEventHandler_ = NULL;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Must be called at start to init the application
#ifdef __ANDROID__
void Application::init(struct android_app* state, IAppEventHandler* (*createAppEventHandler)())
{
	// Registering the logger as early as possible
	ServiceLocator::registerLogger(new FileLogger("/sdcard/ncine_log.txt", ILogger::LOG_VERBOSE, ILogger::LOG_VERBOSE));
	if (EglGfxDevice::isModeSupported(state, DisplayMode(8, 8, 8)))
	{
		gfxDevice_ = new EglGfxDevice(state, DisplayMode(8, 8, 8));
	}
	else
	{
		gfxDevice_ = new EglGfxDevice(state, DisplayMode(5, 6, 5));
	}
	AndroidJniHelper::attachJVM(state);
	inputManager_ = new AndroidInputManager(state);
	AssetFile::initAssetManager(state);
#else
void Application::init(IAppEventHandler* (*createAppEventHandler)())
{
	// Registering the logger as early as possible
	ServiceLocator::registerLogger(new FileLogger("ncine_log.txt", ILogger::LOG_VERBOSE, ILogger::LOG_OFF));
	// Graphics device should always be created before the input manager!
#if defined(WITH_SDL)
	gfxDevice_ = new SdlGfxDevice(960, 640);
	inputManager_ = new SdlInputManager();
#elif defined(WITH_GLFW)
	gfxDevice_ = new GlfwGfxDevice(960, 640);
	inputManager_ = new GlfwInputManager();
#endif
	gfxDevice_->setWindowTitle("nCine");
#endif

	LOGI("nCine compiled on " __DATE__ " at " __TIME__);
	ServiceLocator::registerIndexer(new ArrayIndexer());
#ifdef WITH_AUDIO
	ServiceLocator::registerAudioDevice(new ALAudioDevice());
#endif
#ifdef WITH_THREADS
	ServiceLocator::registerThreadPool(new ThreadPool());
#endif
	LOGI_X("Data path: %s", IFile::dataPath());

	frameTimer_ = new FrameTimer(5.0f, 0.2f);
	rootNode_ = new SceneNode();
	renderQueue_ = new RenderQueue();
	profileTimer_ = new Timer();

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

#if defined(__ANDROID__)
//	const char *fontTexFilename = "asset::trebuchet16_128.dds.mp3";
//	const char *fontFntFilename = "asset::trebuchet16_128.fnt";
	const char *fontTexFilename = "/sdcard/ncine/trebuchet32_256_4444.pvr";
	const char *fontFntFilename = "/sdcard/ncine/trebuchet32_256.fnt";
//	const char *fontTexFilename = "/sdcard/ncine/trebuchet16_128.dds";
//	const char *fontFntFilename = "/sdcard/ncine/trebuchet16_128.fnt";
#else
	const char *fontTexFilename = "fonts/trebuchet32_256.png";
	const char *fontFntFilename = "fonts/trebuchet32_256.fnt";
#endif
	if (IFile::access(fontTexFilename, IFile::MODE_EXISTS) &&
		IFile::access(fontFntFilename, IFile::MODE_EXISTS))
	{
		font_ = new Font(fontTexFilename, fontFntFilename);
		textLines_ = new TextNode(rootNode_, font_);
		textLines_->setPosition(0.0f, height());
	}
	else
	{
		LOGW("Cannot access font files for profiling text");
	}

	LOGI("Application initialized");

	appEventHandler_ = createAppEventHandler();
	appEventHandler_->onInit();
	LOGI("IAppEventHandler::OnInit() invoked");

	// HACK: Init of the random seed
	// In the future there could be a random generator service
	srand(time(NULL));

	// HACK: Using the pause flag as a way to know if initialization is done (Android)
	isPaused_ = false;
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
	gfxDevice_->clear();
	appEventHandler_->onFrameStart();
	// Measuring OnFrameStart() + OnFrameEnd() time
	profilePlotter_->addValue(0, profileTimer_->interval());

	profileTimer_->start();
	rootNode_->update(frameTimer_->interval());
	rootNode_->visit(*renderQueue_);
	renderQueue_->draw();

	// TODO: hard-coded 200ms update time
	if (textLines_ && Timer::now() - textUpdateTime > 0.2f)
	{
		textUpdateTime = Timer::now();
		sprintf(textChars_, static_cast<const char *>("FPS: %.0f (%.2fms)\nSprites: %uV, %uDC\nParticles: %uV, %uDC\nText: %uV, %uDC\nPlotter: %uV, %uDC\nTotal: %uV, %uDC"),
				frameTimer_->averageFps(), frameTimer_->interval() * 1000.0f,
				renderQueue_->numVertices(RenderCommand::SPRITE_TYPE), renderQueue_->numCommands(RenderCommand::SPRITE_TYPE),
				renderQueue_->numVertices(RenderCommand::PARTICLE_TYPE), renderQueue_->numCommands(RenderCommand::PARTICLE_TYPE),
				renderQueue_->numVertices(RenderCommand::TEXT_TYPE), renderQueue_->numCommands(RenderCommand::TEXT_TYPE),
				renderQueue_->numVertices(RenderCommand::PLOTTER_TYPE), renderQueue_->numCommands(RenderCommand::PLOTTER_TYPE),
				renderQueue_->numVertices(), renderQueue_->numCommands());

		textLines_->setString(textChars_);
		textLines_->setAlignment(TextNode::ALIGN_RIGHT);
		textLines_->setPosition((width() - textLines_->width()), height());
	}

	ServiceLocator::audioDevice().updatePlayers();
	// Measuring scenegraph update and visit + draw + audio update
	profilePlotter_->addValue(1, profileTimer_->interval());

	profileTimer_->start();
	gfxDevice_->update();
	// Measuring swap buffers time
	profilePlotter_->addValue(2, profileTimer_->interval());

	profileTimer_->start();
	appEventHandler_->onFrameEnd();
}

/// Must be called before exiting to shut down the application
void Application::shutdown()
{
	appEventHandler_->onShutdown();
	LOGI("IAppEventHandler::OnShutdown() invoked");

	delete inputManager_;
#ifdef __ANDROID__
	AndroidJniHelper::detachJVM();
#endif
	if (appEventHandler_)
	{
		delete appEventHandler_;
	}

	delete textLines_;
	delete profilePlotter_;
	delete rootNode_; // deletes every child too
	delete renderQueue_;
	delete font_;
	delete gfxDevice_;
	delete frameTimer_;

	if (ServiceLocator::indexer().isEmpty() == false)
	{
		LOGW("The object indexer is not empty");
	}

	LOGI("Application shutted down");

	ServiceLocator::unregisterAll();
}

/// Returns the elapsed time since the end of the previous frame in milliseconds
float Application::interval()
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

/// Shows or hides profiling graphs
void Application::showProfileGraphs(bool shouldDraw)
{
	profilePlotter_->shouldDraw_ = shouldDraw;
}

/// Shows or hides profiling information text
void Application::showProfileInfo(bool shouldDraw)
{
	textLines_->shouldDraw_ = shouldDraw;
}

}
