#include "AndroidApplication.h"
#include "AppConfiguration.h"
#include "IAppEventHandler.h"
#include "ServiceLocator.h"
#include "FileLogger.h"
#include "EglGfxDevice.h"
#include "AndroidInputManager.h"
#include "AssetFile.h"

namespace ncine {

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
	ServiceLocator::registerLogger(new FileLogger(appCfg_.logFile_.data(), appCfg_.consoleLogLevel_, appCfg_.fileLogLevel_));
}

/// Must be called at start to init the application
void AndroidApplication::init(struct android_app* state)
{
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

/// Wrapper around EglGfxDevice::createSurface()
void AndroidApplication::createEglSurface(struct android_app* state)
{
	if (gfxDevice_)
	{
		EglGfxDevice *eglDevice = static_cast<EglGfxDevice *>(gfxDevice_);
		eglDevice->createSurface(state);
	}
}

/// Wrapper around EglGfxDevice::bindContext()
void AndroidApplication::bindEglContext()
{
	if (gfxDevice_)
	{
		EglGfxDevice *eglDevice = static_cast<EglGfxDevice *>(gfxDevice_);
		eglDevice->bindContext();
	}
}

/// Wrapper around EglGfxDevice::unbindContext()
void AndroidApplication::unbindEglContext()
{
	if (gfxDevice_)
	{
		EglGfxDevice *eglDevice = static_cast<EglGfxDevice *>(gfxDevice_);
		eglDevice->unbindContext();
	}
}

/// Wrapper around EglGfxDevice::querySurfaceSize()
void AndroidApplication::queryEglSurfaceSize()
{
	if (gfxDevice_)
	{
		EglGfxDevice *eglDevice = static_cast<EglGfxDevice *>(gfxDevice_);
		eglDevice->querySurfaceSize();
	}
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
			ServiceLocator::audioDevice().unfreezePlayers();
		}
		else
		{
			ServiceLocator::audioDevice().freezePlayers();
		}
	}

}

}
