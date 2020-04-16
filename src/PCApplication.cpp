#include "PCApplication.h"
#include "IAppEventHandler.h"
#include "FileLogger.h"
#include "FileSystem.h"

#if defined(WITH_SDL)
	#include "SdlGfxDevice.h"
	#include "SdlInputManager.h"
	#if WITH_NUKLEAR
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

int PCApplication::start(nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)())
{
	ASSERT(createAppEventHandler);
	PCApplication &app = static_cast<PCApplication &>(theApplication());

	app.init(createAppEventHandler);
#ifndef __EMSCRIPTEN__
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

void PCApplication::init(nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)())
{
	ZoneScoped;
	profileStartTime_ = TimeStamp::now();
	wasSuspended_ = shouldSuspend();

	// Registering the logger as early as possible
	theServiceLocator().registerLogger(nctl::makeUnique<FileLogger>(appCfg_.consoleLogLevel));

	appEventHandler_ = createAppEventHandler();
	// Only `onPreInit()` can modify the application configuration
	AppConfiguration &modifiableAppCfg = const_cast<AppConfiguration &>(appCfg_);
	appEventHandler_->onPreInit(modifiableAppCfg);
	LOGI("IAppEventHandler::onPreInit() invoked");

	// Setting log levels and filename based on application configuration
	FileLogger &fileLogger = static_cast<FileLogger &>(theServiceLocator().logger());
	fileLogger.setConsoleLevel(appCfg_.consoleLogLevel);
	fileLogger.setFileLevel(appCfg_.fileLogLevel);
	fileLogger.openLogFile(appCfg_.logFile.data());
	// Graphics device should always be created before the input manager!
	IGfxDevice::GLContextInfo glContextInfo(appCfg_);
	const DisplayMode::VSync vSyncMode = appCfg_.withVSync ? DisplayMode::VSync::ENABLED : DisplayMode::VSync::DISABLED;
	DisplayMode displayMode(8, 8, 8, 8, 24, 8, DisplayMode::DoubleBuffering::ENABLED, vSyncMode);

	const IGfxDevice::WindowMode windowMode(appCfg_.resolution.x, appCfg_.resolution.y, appCfg_.inFullscreen, appCfg_.isResizable);
#if defined(WITH_SDL)
	gfxDevice_ = nctl::makeUnique<SdlGfxDevice>(windowMode, glContextInfo, displayMode);
	inputManager_ = nctl::makeUnique<SdlInputManager>();
#elif defined(WITH_GLFW)
	gfxDevice_ = nctl::makeUnique<GlfwGfxDevice>(windowMode, glContextInfo, displayMode);
	inputManager_ = nctl::makeUnique<GlfwInputManager>();
#elif defined(WITH_QT5)
	FATAL_ASSERT(widget_);
	gfxDevice_ = nctl::makeUnique<Qt5GfxDevice>(windowMode, glContextInfo, displayMode, *widget_);
	inputManager_ = nctl::makeUnique<Qt5InputManager>(*widget_);
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
#if !defined(__EMSCRIPTEN__) && !defined(WITH_QT5)
	while (!shouldQuit_)
	{
#endif

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
#if !defined(__EMSCRIPTEN__) && !defined(WITH_QT5)
	}
#endif
}

void PCApplication::processEvents()
{
	ZoneScopedN("Poll events");
#if defined(WITH_SDL)
	SDL_Event event;

	#if WITH_NUKLEAR
	NuklearSdlInput::inputBegin();
	#endif
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				shouldQuit_ = true;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
					setFocus(true);
				else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
					setFocus(false);
				else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					gfxDevice_->width_ = event.window.data1;
					gfxDevice_->height_ = event.window.data2;
					gfxDevice_->setViewport(event.window.data1, event.window.data2);
				}
				break;
			default:
				SdlInputManager::parseEvent(event);
				break;
		}

	#ifndef __EMSCRIPTEN__
		if (shouldSuspend())
		{
			SDL_WaitEvent(&event);
			SDL_PushEvent(&event);
		}
	#endif
	}
	#if WITH_NUKLEAR
	NuklearSdlInput::inputEnd();
	#endif

#elif defined(WITH_GLFW)
	// GLFW does not seem to correctly handle Emscripten focus and blur events
	#ifndef __EMSCRIPTEN__
	setFocus(GlfwInputManager::hasFocus());
	#endif

	if (shouldSuspend())
		glfwWaitEvents();
	else
		glfwPollEvents();
	GlfwInputManager::updateJoystickStates();
#endif
}

#ifdef __EMSCRIPTEN__
void PCApplication::emscriptenStep()
{
	reinterpret_cast<PCApplication &>(theApplication()).run();
}
#endif

}
