#include "PCApplication.h"
#include "IAppEventHandler.h"
#include "ServiceLocator.h"
#include "FileLogger.h"

#if defined(WITH_SDL)
	#include "SdlGfxDevice.h"
	#include "SdlInputManager.h"
#elif defined(WITH_GLFW)
	#include "GlfwGfxDevice.h"
	#include "GlfwInputManager.h"
#endif

namespace ncine {

/// Meyers' Singleton
Application& theApplication()
{
	static PCApplication instance;
	return instance;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void PCApplication::main(IAppEventHandler* (*createAppEventHandler)())
{
	PCApplication &app = static_cast<PCApplication &>(theApplication());

	app.init(createAppEventHandler);
	app.run();
	app.shutdownCommon();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Must be called at start to init the application
void PCApplication::init(IAppEventHandler* (*createAppEventHandler)())
{
	appEventHandler_ = createAppEventHandler();
	appEventHandler_->onPreInit(appCfg_);

	// Registering the logger as early as possible
	String logFilePath = IFile::dataPath() + appCfg_.logFile_;
	theServiceLocator().registerLogger(new FileLogger(logFilePath.data(), appCfg_.consoleLogLevel_, appCfg_.fileLogLevel_));
	// Graphics device should always be created before the input manager!
	DisplayMode displayMode(8, 8, 8, 8, 24, 8, DisplayMode::DOUBLE_BUFFERED, DisplayMode::NO_VSYNC);
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
void PCApplication::run()
{
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
}

}
