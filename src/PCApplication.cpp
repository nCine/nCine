#include "PCApplication.h"
#include "Timer.h"
#include "IAppEventHandler.h"
#include "FileLogger.h"

#if defined(WITH_SDL)
	#include "SdlGfxDevice.h"
	#include "SdlInputManager.h"
#elif defined(WITH_GLFW)
	#include "GlfwGfxDevice.h"
	#include "GlfwInputManager.h"
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

void PCApplication::start(IAppEventHandler *(*createAppEventHandler)())
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
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void PCApplication::init(IAppEventHandler *(*createAppEventHandler)())
{
	ZoneScoped;
	profileTimer_ = nctl::makeUnique<Timer>();
	profileTimer_->start();

	appEventHandler_ = nctl::UniquePtr<IAppEventHandler>(createAppEventHandler());
	// Only `onPreInit()` can modify the application configuration
	AppConfiguration &modifiableAppCfg = const_cast<AppConfiguration &>(appCfg_);
	appEventHandler_->onPreInit(modifiableAppCfg);

	// Registering the logger as early as possible
	theServiceLocator().registerLogger(nctl::makeUnique<FileLogger>(appCfg_.logFile.data(), appCfg_.consoleLogLevel, appCfg_.fileLogLevel));
	// Graphics device should always be created before the input manager!
	IGfxDevice::GLContextInfo glContextInfo(appCfg_);
	const DisplayMode::VSync vSyncMode = appCfg_.withVSync ? DisplayMode::VSync::ENABLED : DisplayMode::VSync::DISABLED;
	DisplayMode displayMode(8, 8, 8, 8, 24, 8, DisplayMode::DoubleBuffering::ENABLED, vSyncMode);

	const IGfxDevice::WindowMode windowMode(appCfg_.xResolution, appCfg_.yResolution, appCfg_.inFullscreen, appCfg_.isResizable);
#if defined(WITH_SDL)
	gfxDevice_ = nctl::makeUnique<SdlGfxDevice>(windowMode, glContextInfo, displayMode);
	inputManager_ = nctl::makeUnique<SdlInputManager>();
#elif defined(WITH_GLFW)
	gfxDevice_ = nctl::makeUnique<GlfwGfxDevice>(windowMode, glContextInfo, displayMode);
	inputManager_ = nctl::makeUnique<GlfwInputManager>();
#endif
	gfxDevice_->setWindowTitle(appCfg_.windowTitle.data());
	nctl::String windowIconFilePath = IFile::dataPath() + appCfg_.windowIconFilename;
	if (IFile::access(windowIconFilePath.data(), IFile::AccessMode::EXISTS))
		gfxDevice_->setWindowIcon(windowIconFilePath.data());

	timings_[Timings::PRE_INIT] = profileTimer_->interval();

	initCommon();
}

void PCApplication::run()
{
#ifndef __EMSCRIPTEN__
	while (!shouldQuit_)
	{
#endif
		processEvents();

		if (shouldSuspend() == false)
			step();
#ifndef __EMSCRIPTEN__
	}
#endif
}

void PCApplication::processEvents()
{
	ZoneScopedN("Poll events");
#if defined(WITH_SDL)
	SDL_Event event;

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
#elif defined(WITH_GLFW)
	setFocus(GlfwInputManager::hasFocus());
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
