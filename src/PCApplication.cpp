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
	appEventHandler_ = createAppEventHandler();
	appEventHandler_->onPreInit(appCfg_);

	// Registering the logger as early as possible
	theServiceLocator().registerLogger(new FileLogger(appCfg_.logFile_.data(), appCfg_.consoleLogLevel_, appCfg_.fileLogLevel_));
	// Graphics device should always be created before the input manager!
	IGfxDevice::GLContextInfo contextInfo(appCfg_.glMajorVersion_, appCfg_.glMinorVersion_, appCfg_.glDebugContext_);
	DisplayMode::VSyncMode vSyncMode = appCfg_.withVSync_ ? DisplayMode::WITH_VSYNC : DisplayMode::NO_VSYNC;
	DisplayMode displayMode(8, 8, 8, 8, 24, 8, DisplayMode::DOUBLE_BUFFERED, vSyncMode);
#if defined(WITH_SDL)
	gfxDevice_ = new SdlGfxDevice(appCfg_.xResolution_, appCfg_.yResolution_, contextInfo, displayMode, appCfg_.inFullscreen_);
	inputManager_ = new SdlInputManager();
#elif defined(WITH_GLFW)
	gfxDevice_ = new GlfwGfxDevice(appCfg_.xResolution_, appCfg_.yResolution_, contextInfo, displayMode, appCfg_.inFullscreen_);
	inputManager_ = new GlfwInputManager();
#endif
	gfxDevice_->setWindowTitle(appCfg_.windowTitle_.data());
	String windowIconFilePath = IFile::dataPath() + appCfg_.windowIconFilename_;
	if (IFile::access(windowIconFilePath.data(), IFile::MODE_EXISTS))
	{
		gfxDevice_->setWindowIcon(windowIconFilePath.data());
	}

	initCommon();
}

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
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) { setFocus(true); }
					else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) { setFocus(false); }
					break;
				default:
					SdlInputManager::parseEvent(event);
					break;
			}

			if (!hasFocus_ || isPaused_)
			{
				SDL_WaitEvent(&event);
				SDL_PushEvent(&event);
			}
		}
#elif defined(WITH_GLFW)
		setFocus(GlfwInputManager::hasFocus());
		if (!hasFocus_ || isPaused_) { glfwWaitEvents(); } else { glfwPollEvents(); }
		GlfwInputManager::updateJoystickStates();
#endif

		if (hasFocus_ && !isPaused_)
		{
			step();
		}
	}
}

}
