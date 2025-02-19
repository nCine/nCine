#include "PCApplication.h"
#include "IAppEventHandler.h"
#include "FileLogger.h"
#include "FileSystem.h"

#if defined(WITH_SDL)
	#include "SdlGfxDevice.h"
	#include "SdlInputManager.h"
	#ifdef WITH_NUKLEAR
		#include "NuklearSdlInput.h"
	#endif
#elif defined(WITH_GLFW)
	#include "GlfwGfxDevice.h"
	#include "GlfwInputManager.h"
#elif defined(WITH_QT5)
	#include "Qt5GfxDevice.h"
	#include "Qt5InputManager.h"
#endif

#ifdef __EMSCRIPTEN__
	#include "emscripten.h"
#endif

#include "tracy.h"

namespace ncine {

Application &theApplication()
{
	static PCApplication instance;
	return instance;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

int PCApplication::start(nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)(), int argc, char **argv)
{
	ASSERT(createAppEventHandler);
	PCApplication &app = static_cast<PCApplication &>(theApplication());

	app.init(createAppEventHandler, argc, argv);
#ifndef __EMSCRIPTEN__
	while (app.shouldQuit_ == false)
		app.run();
#else
	emscripten_set_main_loop(PCApplication::emscriptenStep, 0, 1);
	emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
#endif
	app.shutdownCommon();

	return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void PCApplication::init(nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)(), int argc, char **argv)
{
	ZoneScoped;
	profileStartTime_ = TimeStamp::now();
	wasSuspended_ = shouldSuspend();

	// Registering the logger as early as possible
	theServiceLocator().registerLogger(nctl::makeUnique<FileLogger>(appCfg_.consoleLogLevel));

	appEventHandler_ = createAppEventHandler();
	// Only `onPreInit()` can modify the application configuration
	AppConfiguration &modifiableAppCfg = const_cast<AppConfiguration &>(appCfg_);
	modifiableAppCfg.argc_ = argc;
	modifiableAppCfg.argv_ = argv;
	appEventHandler_->onPreInit(modifiableAppCfg);

	// Setting log levels and filename based on application configuration
	FileLogger &fileLogger = static_cast<FileLogger &>(theServiceLocator().logger());
	fileLogger.setConsoleLevel(appCfg_.consoleLogLevel);
	fileLogger.setFileLevel(appCfg_.fileLogLevel);
	fileLogger.openLogFile(appCfg_.logFile.data());
	LOGI("IAppEventHandler::onPreInit() invoked"); // Logging delayed to set up the logger first

	// The application's `onPreInit()` might have requested to quit
	if (shouldQuit_)
		return;

	// Graphics device should always be created before the input manager!
	IGfxDevice::GLContextInfo glContextInfo(appCfg_);
	const DisplayMode::VSync vSyncMode = appCfg_.withVSync ? DisplayMode::VSync::ENABLED : DisplayMode::VSync::DISABLED;
	DisplayMode displayMode(8, 8, 8, 8, 24, 8, DisplayMode::DoubleBuffering::ENABLED, vSyncMode);

	const IGfxDevice::WindowMode windowMode(appCfg_);
#if defined(WITH_SDL)
	gfxDevice_ = nctl::makeUnique<SdlGfxDevice>(windowMode, glContextInfo, displayMode);
	inputManager_ = nctl::makeUnique<SdlInputManager>();
#elif defined(WITH_GLFW)
	gfxDevice_ = nctl::makeUnique<GlfwGfxDevice>(windowMode, glContextInfo, displayMode);
	inputManager_ = nctl::makeUnique<GlfwInputManager>();
#elif defined(WITH_QT5)
	FATAL_ASSERT_MSG(qt5Widget_, "The Qt5 widget has not been assigned");
	gfxDevice_ = nctl::makeUnique<Qt5GfxDevice>(windowMode, glContextInfo, displayMode, *qt5Widget_);
	inputManager_ = nctl::makeUnique<Qt5InputManager>(*qt5Widget_);
#endif
	gfxDevice_->setWindowTitle(appCfg_.windowTitle.data());
	nctl::String windowIconFilePath = fs::joinPath(fs::dataPath(), appCfg_.windowIconFilename);
	if (fs::isReadableFile(windowIconFilePath.data()))
		gfxDevice_->setWindowIcon(windowIconFilePath.data());

	timings_[Timings::PRE_INIT] = profileStartTime_.secondsSince();

#ifndef WITH_QT5
	// Common initialization on Qt5 is performed later, when OpenGL can be used
	initCommon();
#endif
}

void PCApplication::run()
{
#if !defined(WITH_QT5)
	processEvents();
#elif defined(WITH_QT5GAMEPAD)
	static_cast<Qt5InputManager &>(*inputManager_).updateJoystickStates();
#endif

	const bool suspended = shouldSuspend();
	if (wasSuspended_ != suspended)
	{
		if (suspended)
			suspend();
		else
			resume();
		wasSuspended_ = suspended;
	}

	if (suspended == false)
		step();

#if defined(WITH_QT5)
	static_cast<Qt5InputManager &>(*inputManager_).copyButtonStatesToPrev();
#endif
}

#if defined(WITH_SDL)
void PCApplication::processEvents()
{
	ZoneScoped;

	SdlInputManager::copyButtonStatesToPrev();

	SDL_Event event;
	#ifndef __EMSCRIPTEN__
	if (shouldSuspend())
	{
		SDL_WaitEvent(&event);
		SDL_PushEvent(&event);
		// Don't lose any events when resuming
		while (SDL_PollEvent(&event))
			SDL_PushEvent(&event);
	}
	#endif

	#ifdef WITH_NUKLEAR
	NuklearSdlInput::inputBegin();
	#endif

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				shouldQuit_ = SdlInputManager::shouldQuitOnRequest();
				break;
			case SDL_DISPLAYEVENT:
				gfxDevice_->updateMonitors();
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
					setFocus(true);
				else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
					setFocus(false);
				else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					SDL_Window *windowHandle = SDL_GetWindowFromID(event.window.windowID);
					SDL_GL_GetDrawableSize(windowHandle, &gfxDevice_->drawableWidth_, &gfxDevice_->drawableHeight_);
					// With `SDL_HINT_WINDOWS_DPI_SCALING`, the event data will contain pre-scaled width and height
	#ifndef __APPLE__
					const int newWidth = gfxDevice_->drawableWidth_;
					const int newHeight = gfxDevice_->drawableHeight_;
	#else
					const int newWidth = event.window.data1;
					const int newHeight = event.window.data2;
	#endif
					gfxDevice_->width_ = newWidth;
					gfxDevice_->height_ = newHeight;
					gfxDevice_->isFullScreen_ = SDL_GetWindowFlags(windowHandle) & SDL_WINDOW_FULLSCREEN;
					resizeScreenViewport(newWidth, newHeight);
				}
				else if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					if (gfxDevice_->backendScalesWindowSize_)
					{
						// Check if the scale factor has changed and the callback needs to be invoked
						updateScalingFactor();
					}
				}
				break;
			default:
				SdlInputManager::parseEvent(event);
				break;
		}
	}

	#ifdef WITH_NUKLEAR
	NuklearSdlInput::inputEnd();
	#endif
}
#elif defined(WITH_GLFW)
void PCApplication::processEvents()
{
	ZoneScoped;

	GlfwInputManager::copyButtonStatesToPrev();

	// GLFW does not seem to correctly handle Emscripten focus and blur events
	#ifndef __EMSCRIPTEN__
	setFocus(GlfwInputManager::hasFocus());
	#endif

	if (shouldSuspend())
		glfwWaitEvents();
	else
		glfwPollEvents();
	GlfwInputManager::updateJoystickStates();
}
#endif

#ifdef __EMSCRIPTEN__
void PCApplication::emscriptenStep()
{
	reinterpret_cast<PCApplication &>(theApplication()).run();
}
#endif

}
