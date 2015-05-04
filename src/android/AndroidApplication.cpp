#include "AndroidApplication.h"
#include "AppConfiguration.h"
#include "IAppEventHandler.h"
#include "ServiceLocator.h"
#include "FileLogger.h"
#include "EglGfxDevice.h"
#include "AndroidInputManager.h"
#include "AssetFile.h"
#include "AndroidJniHelper.h"

namespace ncine {

/// Meyers' Singleton
AndroidApplication& theApplication()
{
	static AndroidApplication instance;
	return instance;
}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool AndroidApplication::isInitialized_ = false;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AndroidApplication::preInit(IAppEventHandler* (*createAppEventHandler)())
{
	appEventHandler_ = createAppEventHandler();
	appEventHandler_->onPreInit(appCfg_);

	// Registering the logger as early as possible
	String logFilePath = IFile::dataPath() + appCfg_.logFile_;
	theServiceLocator().registerLogger(new FileLogger(logFilePath.data(), appCfg_.consoleLogLevel_, appCfg_.fileLogLevel_));
}

/// Must be called at start to init the application
void AndroidApplication::init(struct android_app* state)
{
	// Graphics device should always be created before the input manager!
	DisplayMode displayMode32(8, 8, 8, 8, 24, 8, true, false);
	DisplayMode displayMode16(5, 6, 5, 0, 16, 0, true, false);
	if (EglGfxDevice::isModeSupported(state, displayMode32))
	{
		gfxDevice_ = new EglGfxDevice(state, displayMode32);
	}
	else if (EglGfxDevice::isModeSupported(state, displayMode16))
	{
		gfxDevice_ = new EglGfxDevice(state, displayMode16);
	}
	else
	{
		LOGF("Cannot find a suitable EGL configuration, graphics device not created");
		exit(EXIT_FAILURE);
	}
	AndroidJniHelper::attachJVM(state);
	inputManager_ = new AndroidInputManager(state);
	AssetFile::initAssetManager(state);

	Application::initCommon();

	isInitialized_ = true;
}

/// Must be called before exiting to shut down the application
void AndroidApplication::shutdown()
{
	AndroidJniHelper::detachJVM();
	Application::shutdown();
	isInitialized_ = false;
}

/// Wrapper around AndroidJniHelper::sdkVersion()
unsigned int AndroidApplication::sdkVersion() const
{
	unsigned int sdkVersion = 0;

	if (isInitialized_)
	{
		sdkVersion = AndroidJniHelper::sdkVersion();
	}

	return sdkVersion;
}

void AndroidApplication::setFocus(bool hasFocus)
{
	Application::setFocus(hasFocus);

	// Check if a focus event has occurred
	if (hasFocus_ != hasFocus)
	{
		hasFocus_ = hasFocus;
		// Check if focus has been gained
		if (hasFocus == true)
		{
			theServiceLocator().audioDevice().unfreezePlayers();
		}
		else
		{
			theServiceLocator().audioDevice().freezePlayers();
		}
	}

}

}
