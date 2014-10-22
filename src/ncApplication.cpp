#include <ctime>
#include "ncApplication.h"
#include "ncIAppEventHandler.h"
#include "ncServiceLocator.h"
#include "ncArrayIndexer.h"
#include "ncFileLogger.h"
#include "ncALAudioDevice.h"
#include "ncLinePlotter.h"
#include "ncStackedBarPlotter.h"
#include "ncFont.h"
#include "ncTextNode.h"

#ifdef WITH_THREADS
	#include "ncThreadPool.h"
#endif

#if defined(__ANDROID__)
	#include "ncEGLGfxDevice.h"
	#include "ncAndroidInputManager.h"
	#include "ncAssetFile.h"
#elif defined(WITH_SDL)
	#include "ncSDLGfxDevice.h"
	#include "ncSDLInputManager.h"
#elif defined(WITH_GLFW)
	#include "ncGLFWGfxDevice.h"
	#include "ncGLFWInputManager.h"
#endif

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool ncApplication::isPaused_ = true;
bool ncApplication::hasFocus_ = true;
bool ncApplication::shouldQuit_ = false;
ncFrameTimer *ncApplication::frameTimer_ = NULL;
ncIGfxDevice *ncApplication::gfxDevice_ = NULL;
ncSceneNode *ncApplication::rootNode_ = NULL;
ncRenderQueue *ncApplication::renderQueue_ = NULL;
ncTimer *ncApplication::profileTimer_ = NULL;
ncProfilePlotter *ncApplication::profilePlotter_ = NULL;
ncFont *ncApplication::font_ = NULL;
ncTextNode *ncApplication::textLines_ = NULL;
float ncApplication::textUpdateTime = 0.0f;
char ncApplication::textChars_[256] = "";
ncIInputManager *ncApplication::inputManager_ = NULL;
ncIAppEventHandler *ncApplication::appEventHandler_ = NULL;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Must be called at start to init the application
#ifdef __ANDROID__
void ncApplication::init(struct android_app* state, ncIAppEventHandler* (*createAppEventHandler)())
{
	// Registering the logger as early as possible
	ncServiceLocator::registerLogger(new ncFileLogger("/sdcard/ncine_log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_VERBOSE));
	if (ncEGLGfxDevice::isModeSupported(state, ncDisplayMode(8, 8, 8)))
	{
		gfxDevice_ = new ncEGLGfxDevice(state, ncDisplayMode(8, 8, 8));
	}
	else
	{
		gfxDevice_ = new ncEGLGfxDevice(state, ncDisplayMode(5, 6, 5));
	}
	ncAndroidJNIHelper::attachJVM(state);
	inputManager_ = new ncAndroidInputManager(state);
	ncAssetFile::initAssetManager(state);
#else
void ncApplication::init(ncIAppEventHandler* (*createAppEventHandler)())
{
	// Registering the logger as early as possible
	ncServiceLocator::registerLogger(new ncFileLogger("ncine_log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));
	// Graphics device should always be created before the input manager!
#if defined(WITH_SDL)
	gfxDevice_ = new ncSDLGfxDevice(960, 640);
	inputManager_ = new ncSDLInputManager();
#elif defined(WITH_GLFW)
	gfxDevice_ = new ncGLFWGfxDevice(960, 640);
	inputManager_ = new ncGLFWInputManager();
#endif
	gfxDevice_->setWindowTitle("nCine");
#endif

	LOGI("nCine compiled on " __DATE__ " at " __TIME__);
	ncServiceLocator::registerIndexer(new ncArrayIndexer());
	ncServiceLocator::registerAudioDevice(new ncALAudioDevice());
#ifdef WITH_THREADS
	ncServiceLocator::registerThreadPool(new ncThreadPool());
#endif
	LOGI_X("Data path: %s", ncIFile::dataPath());

	frameTimer_ = new ncFrameTimer(5.0f, 0.2f);
	rootNode_ = new ncSceneNode();
	renderQueue_ = new ncRenderQueue();
	profileTimer_ = new ncTimer();

	profilePlotter_ = new ncStackedBarPlotter(rootNode_, ncRect(width() * 0.1f, height() * 0.1f, width() * 0.8f, height() * 0.15f));
	profilePlotter_->setBackgroundColor(ncColor(0.35f, 0.35f, 0.45f, 0.5f));
	profilePlotter_->addVariable(50, 0.2f);
	profilePlotter_->variable(0).setGraphColor(ncColor(0.8f, 0.0f, 0.0f));
	profilePlotter_->variable(0).setMeanColor(ncColor(1.0f, 0.0f, 0.0f));
	profilePlotter_->addVariable(50, 0.2f);
	profilePlotter_->variable(1).setGraphColor(ncColor(0.0f, 0.8f, 0.0f));
	profilePlotter_->variable(1).setMeanColor(ncColor(0.0f, 1.0f, 0.0f));
	profilePlotter_->addVariable(50, 0.2f);
	profilePlotter_->variable(2).setGraphColor(ncColor(0.0f, 0.0f, 0.8f));
	profilePlotter_->variable(2).setMeanColor(ncColor(0.0f, 0.0f, 1.0f));

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
	if (ncIFile::access(fontTexFilename, ncIFile::MODE_EXISTS) &&
		ncIFile::access(fontFntFilename, ncIFile::MODE_EXISTS))
	{
		font_ = new ncFont(fontTexFilename, fontFntFilename);
		textLines_ = new ncTextNode(rootNode_, font_);
		textLines_->setPosition(0.0f, height());
	}
	else
	{
		LOGW("Cannot access font files for profiling text");
	}

	LOGI("ncApplication initialized");

	appEventHandler_ = createAppEventHandler();
	appEventHandler_->onInit();
	LOGI("ncIAppEventHandler::OnInit() invoked");

	// HACK: Init of the random seed
	// In the future there could be a random generator service
	srand(time(NULL));

	// HACK: Using the pause flag as a way to know if initialization is done (Android)
	isPaused_ = false;
}

/// The main game loop, handling events and rendering
void ncApplication::run()
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
					ncSDLInputManager::parseEvent(event);
					break;
			}
		}
#elif defined(WITH_GLFW)
		setFocus(ncGLFWInputManager::hasFocus());
		glfwPollEvents();
		ncGLFWInputManager::updateJoystickStates();
#endif

		if (hasFocus_ && !isPaused_)
		{
			step();
		}
	}
#endif
}

/// A single step of the game loop made to render a frame
void ncApplication::step()
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
	if (textLines_ && ncTimer::now() - textUpdateTime > 0.2f)
	{
		textUpdateTime = ncTimer::now();
		sprintf(textChars_, static_cast<const char *>("FPS: %.0f (%.2fms)\nSprites: %uV, %uDC\nParticles: %uV, %uDC\nText: %uV, %uDC\nPlotter: %uV, %uDC\nTotal: %uV, %uDC"),
				frameTimer_->averageFps(), frameTimer_->interval() * 1000.0f,
				renderQueue_->numVertices(ncRenderCommand::SPRITE_TYPE), renderQueue_->numCommands(ncRenderCommand::SPRITE_TYPE),
				renderQueue_->numVertices(ncRenderCommand::PARTICLE_TYPE), renderQueue_->numCommands(ncRenderCommand::PARTICLE_TYPE),
				renderQueue_->numVertices(ncRenderCommand::TEXT_TYPE), renderQueue_->numCommands(ncRenderCommand::TEXT_TYPE),
				renderQueue_->numVertices(ncRenderCommand::PLOTTER_TYPE), renderQueue_->numCommands(ncRenderCommand::PLOTTER_TYPE),
				renderQueue_->numVertices(), renderQueue_->numCommands());

		textLines_->setString(textChars_);
		textLines_->setAlignment(ncTextNode::ALIGN_RIGHT);
		textLines_->setPosition((width() - textLines_->width()), height());
	}

	ncServiceLocator::audioDevice().updatePlayers();
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
void ncApplication::shutdown()
{
	appEventHandler_->onShutdown();
	LOGI("ncIAppEventHandler::OnShutdown() invoked");

	delete inputManager_;
#ifdef __ANDROID__
	ncAndroidJNIHelper::detachJVM();
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

	if (ncServiceLocator::indexer().isEmpty() == false)
	{
		LOGW("The object indexer is not empty");
	}

	LOGI("ncApplication shutted down");

	ncServiceLocator::unregisterAll();
}

/// Sets the pause flag value
void ncApplication::setPause(bool paused)
{
	isPaused_ = paused;
	if (isPaused_ == false)
	{
		frameTimer_->start();
		profileTimer_->start();
	}
}

/// Toggles the pause flag on and off
void ncApplication::togglePause()
{
	bool paused = !isPaused_;
	setPause(paused);
}

/// Sets the focus flag
void ncApplication::setFocus(bool hasFocus)
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
void ncApplication::showProfileGraphs(bool shouldDraw)
{
	profilePlotter_->shouldDraw_ = shouldDraw;
}

/// Shows or hides profiling information text
void ncApplication::showProfileInfo(bool shouldDraw)
{
	textLines_->shouldDraw_ = shouldDraw;
}
