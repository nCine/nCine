#include "AndroidApplication.h"
#include "IAppEventHandler.h"
#include "ServiceLocator.h"
#include "FileLogger.h"
#include "EglGfxDevice.h"
#include "AndroidInputManager.h"
#include "AssetFile.h"
#include "AndroidJniHelper.h"

/// Processes the next application command
void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
	nc::AndroidApplication::processCommand(app, cmd);
}

/// Parses the next input event
int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
	return static_cast<int32_t>(nc::AndroidInputManager::parseEvent(event));
}

namespace ncine {

/// Meyers' Singleton
Application& theApplication()
{
	static AndroidApplication instance;
	return instance;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Entry point method to be called in the `android_main()` function
void AndroidApplication::start(struct android_app* state, IAppEventHandler* (*createAppEventHandler)())
{
	theAndroidApplication().state_ = state;
	theAndroidApplication().createAppEventHandler_ = createAppEventHandler;

	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;

	while (theApplication().shouldQuit() == false)
	{
		int ident;
		int events;
		struct android_poll_source* source;

		while ((ident = ALooper_pollAll(!theApplication().isPaused() ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
		{
			if (source != NULL)
			{
				source->process(state, source);
			}

			if (ident == LOOPER_ID_USER)
			{
				AndroidInputManager::parseAccelerometerEvent();
			}

			if (state->destroyRequested)
			{
				LOGI("android_app->destroyRequested not equal to zero");
				theApplication().quit();
			}
		}

		if (theAndroidApplication().isInitialized() &&
			theApplication().hasFocus() &&
			!theApplication().isPaused())
		{
			AndroidInputManager::updateJoystickConnections();
			theApplication().step();
		}
	}

	theAndroidApplication().shutdown();
	ANativeActivity_finish(state->activity);
}

/// Processes an Android application command
void AndroidApplication::processCommand(struct android_app* app, int32_t cmd)
{
	static EglGfxDevice *eglGfxDevice = NULL;

	switch (cmd)
	{
		case APP_CMD_INPUT_CHANGED:
			LOGW("APP_CMD_INPUT_CHANGED event received (not handled)");
			break;

		case APP_CMD_INIT_WINDOW:
			LOGI("APP_CMD_INIT_WINDOW event received");
			if (app->window != NULL)
			{
				if (theAndroidApplication().isInitialized() == false)
				{
					theAndroidApplication().init();
					eglGfxDevice = &static_cast<EglGfxDevice &>(theApplication().gfxDevice());
					theApplication().step();
				}
				else
				{
					eglGfxDevice->createSurface(app);
					eglGfxDevice->bindContext();
				}
			}
			break;
		case APP_CMD_TERM_WINDOW:
			LOGI("APP_CMD_TERM_WINDOW event received");
			eglGfxDevice->unbindContext();
			break;
		case APP_CMD_WINDOW_RESIZED:
			LOGI("APP_CMD_WINDOW_RESIZED event received");
			eglGfxDevice->querySurfaceSize();
			break;
		case APP_CMD_WINDOW_REDRAW_NEEDED:
			LOGI("APP_CMD_WINDOW_REDRAW_NEEDED event received");
			theApplication().step();
			break;

		case APP_CMD_GAINED_FOCUS:
			LOGI("APP_CMD_GAINED_FOCUS event received");
			AndroidInputManager::enableAccelerometerSensor();
			theApplication().setFocus(true);
			break;
		case APP_CMD_LOST_FOCUS:
			LOGI("APP_CMD_LOST_FOCUS event received");
			AndroidInputManager::disableAccelerometerSensor();
			theApplication().setFocus(false);
			theApplication().step();
			break;

		case APP_CMD_CONFIG_CHANGED:
			LOGW("APP_CMD_CONFIG_CHANGED event received (not handled)");
			break;

		case APP_CMD_LOW_MEMORY:
			LOGW("APP_CMD_LOW_MEMORY event received (not handled)");
			break;

		case APP_CMD_START:
			if (theAndroidApplication().isInitialized() == false)
			{
				theAndroidApplication().preInit();
				LOGI("APP_CMD_START event received (first start)");
			}
			LOGI("APP_CMD_START event received");
			break;
		case APP_CMD_RESUME:
			LOGW("APP_CMD_RESUME event received (not handled)");
			break;
		case APP_CMD_SAVE_STATE:
			LOGW("APP_CMD_SAVE_STATE event received (not handled)");
			break;
		case APP_CMD_PAUSE:
			LOGW("APP_CMD_PAUSE event received (not handled)");
			break;
		case APP_CMD_STOP:
			LOGW("APP_CMD_STOP event received (not handled)");
			break;

		case APP_CMD_DESTROY:
			LOGI("APP_CMD_DESTROY event received");
			theApplication().quit();
			break;
	}
}

/// Wrapper around `AndroidJniHelper::sdkVersion()`
unsigned int AndroidApplication::sdkVersion() const
{
	unsigned int sdkVersion = 0;

	if (isInitialized_)
	{
		sdkVersion = AndroidJniHelper::sdkVersion();
	}

	return sdkVersion;
}

/// Wrapper around `AndroidInputManager::enableAccelerometer()`
void AndroidApplication::enableAccelerometer(bool enabled)
{
	if (isInitialized_)
	{
		AndroidInputManager::enableAccelerometer(enabled);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void AndroidApplication::preInit()
{
	appEventHandler_ = createAppEventHandler_();
	appEventHandler_->onPreInit(appCfg_);

	// Registering the logger as early as possible
	String logFilePath = IFile::dataPath() + appCfg_.logFile_;
	theServiceLocator().registerLogger(new FileLogger(logFilePath.data(), appCfg_.consoleLogLevel_, appCfg_.fileLogLevel_));
}

/// Must be called at start to init the application
void AndroidApplication::init()
{
	// Graphics device should always be created before the input manager!
	DisplayMode displayMode32(8, 8, 8, 8, 24, 8, DisplayMode::DOUBLE_BUFFERED, DisplayMode::NO_VSYNC);
	DisplayMode displayMode16(5, 6, 5, 0, 16, 0, DisplayMode::DOUBLE_BUFFERED, DisplayMode::NO_VSYNC);
	if (EglGfxDevice::isModeSupported(state_, displayMode32))
	{
		gfxDevice_ = new EglGfxDevice(state_, displayMode32);
	}
	else if (EglGfxDevice::isModeSupported(state_, displayMode16))
	{
		gfxDevice_ = new EglGfxDevice(state_, displayMode16);
	}
	else
	{
		LOGF("Cannot find a suitable EGL configuration, graphics device not created");
		exit(EXIT_FAILURE);
	}
	AndroidJniHelper::attachJVM(state_);
	inputManager_ = new AndroidInputManager(state_);
	AssetFile::initAssetManager(state_);

	Application::initCommon();

	isInitialized_ = true;
}

/// Must be called before exiting to shut down the application
void AndroidApplication::shutdown()
{
	AndroidJniHelper::detachJVM();
	Application::shutdownCommon();
	isInitialized_ = false;
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
