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

void PCApplication::start(IAppEventHandler * (*createAppEventHandler)())
{
	ASSERT(createAppEventHandler);
	PCApplication &app = static_cast<PCApplication &>(theApplication());

	app.init(createAppEventHandler);
	app.run();
	app.shutdownCommon();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void PCApplication::init(IAppEventHandler * (*createAppEventHandler)())
{
	ZoneScoped;
	profileTimer_ = nctl::makeUnique<Timer>();
	profileTimer_->start();

	appEventHandler_ = nctl::UniquePtr<IAppEventHandler>(createAppEventHandler());
	appEventHandler_->onPreInit(appCfg_);

	// Registering the logger as early as possible
	theServiceLocator().registerLogger(nctl::makeUnique<FileLogger>(appCfg_.logFile().data(), appCfg_.consoleLogLevel(), appCfg_.fileLogLevel()));
	// Graphics device should always be created before the input manager!
	IGfxDevice::GLContextInfo glContextInfo(appCfg_);
	const DisplayMode::VSync vSyncMode = appCfg_.withVSync() ? DisplayMode::VSync::ENABLED : DisplayMode::VSync::DISABLED;
	DisplayMode displayMode(8, 8, 8, 8, 24, 8, DisplayMode::DoubleBuffering::ENABLED, vSyncMode);

	const IGfxDevice::WindowMode windowMode(appCfg_.xResolution(), appCfg_.yResolution(), appCfg_.inFullscreen(), appCfg_.isResizable());
#if defined(WITH_SDL)
	gfxDevice_ = nctl::makeUnique<SdlGfxDevice>(windowMode, glContextInfo, displayMode);
	inputManager_ = nctl::makeUnique<SdlInputManager>();
#elif defined(WITH_GLFW)
	gfxDevice_ = nctl::makeUnique<GlfwGfxDevice>(windowMode, glContextInfo, displayMode);
	inputManager_ = nctl::makeUnique<GlfwInputManager>();
#endif
	gfxDevice_->setWindowTitle(appCfg_.windowTitle().data());
	nctl::String windowIconFilePath = IFile::dataPath() + appCfg_.windowIconFilename();
	if (IFile::access(windowIconFilePath.data(), IFile::AccessMode::EXISTS))
		gfxDevice_->setWindowIcon(windowIconFilePath.data());

	timings_[Timings::PRE_INIT] = profileTimer_->interval();

	initCommon();
}

void PCApplication::run()
{
	while (!shouldQuit_)
	{
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

				if (shouldSuspend())
				{
					SDL_WaitEvent(&event);
					SDL_PushEvent(&event);
				}
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

		if (shouldSuspend() == false)
			step();
	}
}

}
