#include "common_macros.h"
#include "AndroidApplication.h"
#include "Timer.h"
#include "IAppEventHandler.h"
#include "FileLogger.h"
#include "EglGfxDevice.h"
#include "AndroidInputManager.h"
#include "FileSystem.h"
#include "AssetFile.h"
#include "AndroidJniHelper.h"
#include "tracy.h"

#if WITH_NUKLEAR
	#include "NuklearAndroidInput.h"
#endif

namespace nc = ncine;

/// Processes the next application command
void engine_handle_cmd(struct android_app *state, int32_t cmd)
{
	nc::AndroidApplication::processCommand(state, cmd);
}

/// Parses the next input event
int32_t engine_handle_input(struct android_app *state, AInputEvent *event)
{
	return static_cast<int32_t>(nc::AndroidInputManager::parseEvent(event));
}

namespace ncine {

Application &theApplication()
{
	static AndroidApplication instance;
	return instance;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AndroidApplication::start(struct android_app *state, nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)())
{
	ASSERT(state);
	ASSERT(createAppEventHandler);
	theAndroidApplication().state_ = state;
	theAndroidApplication().createAppEventHandler_ = createAppEventHandler;

	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;

	while (theApplication().shouldQuit() == false)
	{
		int ident;
		int events;
		struct android_poll_source *source;

#if WITH_NUKLEAR
		NuklearAndroidInput::inputBegin();
#endif

		while ((ident = ALooper_pollAll(!theApplication().isSuspended() ? 0 : -1, nullptr, &events, reinterpret_cast<void **>(&source))) >= 0)
		{
			if (source != nullptr)
				source->process(state, source);

			if (ident == LOOPER_ID_USER)
				AndroidInputManager::parseAccelerometerEvent();

			if (state->destroyRequested)
			{
				LOGI("android_app->destroyRequested not equal to zero");
				theApplication().quit();
			}
		}

#if WITH_NUKLEAR
		NuklearAndroidInput::inputEnd();
#endif

		if (theAndroidApplication().isInitialized() &&
		    theApplication().shouldSuspend() == false)
		{
			AndroidInputManager::updateJoystickConnections();
			theApplication().step();
		}
	}

	theAndroidApplication().shutdown();
	ANativeActivity_finish(state->activity);
}

void AndroidApplication::processCommand(struct android_app *state, int32_t cmd)
{
	static EglGfxDevice *eglGfxDevice = nullptr;
	// A flag to avoid resuming if the application has not been suspended first
	static bool isPaused = false;

	switch (cmd)
	{
		case APP_CMD_INPUT_CHANGED:
			LOGW("APP_CMD_INPUT_CHANGED event received (not handled)");
			break;

		case APP_CMD_INIT_WINDOW:
			LOGI("APP_CMD_INIT_WINDOW event received");
			if (state->window != nullptr)
			{
				if (theAndroidApplication().isInitialized() == false)
				{
					theAndroidApplication().init();
					eglGfxDevice = &static_cast<EglGfxDevice &>(theApplication().gfxDevice());
					theApplication().step();
				}
				else
				{
					eglGfxDevice->createSurface(state);
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
			LOGW("APP_CMD_RESUME event received");
			if (isPaused)
				theAndroidApplication().resume();
			isPaused = false;
			break;
		case APP_CMD_SAVE_STATE:
			LOGW("APP_CMD_SAVE_STATE event received (not handled)");
			break;
		case APP_CMD_PAUSE:
			LOGW("APP_CMD_PAUSE event received");
			theAndroidApplication().suspend();
			isPaused = true;
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

unsigned int AndroidApplication::sdkVersion() const
{
	unsigned int sdkVersion = 0;

	if (isInitialized_)
		sdkVersion = AndroidJniHelper::sdkVersion();

	return sdkVersion;
}

void AndroidApplication::enableAccelerometer(bool enabled)
{
	if (isInitialized_)
		AndroidInputManager::enableAccelerometer(enabled);
}

const char *AndroidApplication::internalDataPath() const
{
	return state_->activity->internalDataPath;
}

const char *AndroidApplication::externalDataPath() const
{
	return state_->activity->externalDataPath;
}

const char *AndroidApplication::obbPath() const
{
	return state_->activity->obbPath;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void AndroidApplication::preInit()
{
	profileStartTime_ = TimeStamp::now();

	// Registering the logger as early as possible
	theServiceLocator().registerLogger(nctl::makeUnique<FileLogger>(appCfg_.consoleLogLevel));

	AndroidJniHelper::attachJVM(state_);
	AssetFile::initAssetManager(state_);

	appEventHandler_ = nctl::UniquePtr<IAppEventHandler>(createAppEventHandler_());
	// Only `onPreInit()` can modify the application configuration
	AppConfiguration &modifiableAppCfg = const_cast<AppConfiguration &>(appCfg_);
	appEventHandler_->onPreInit(modifiableAppCfg);
	LOGI("IAppEventHandler::onPreInit() invoked");

	// Setting log levels and filename based on application configuration
	const nctl::String logFilePath = fs::joinPath(fs::dataPath(), appCfg_.logFile);
	FileLogger &fileLogger = static_cast<FileLogger &>(theServiceLocator().logger());
	fileLogger.setConsoleLevel(appCfg_.consoleLogLevel);
	fileLogger.setFileLevel(appCfg_.fileLogLevel);
	fileLogger.openLogFile(logFilePath.data());
}

void AndroidApplication::init()
{
	ZoneScoped;
	// Graphics device should always be created before the input manager!
	const DisplayMode displayMode32(8, 8, 8, 8, 24, 8, DisplayMode::DoubleBuffering::ENABLED, DisplayMode::VSync::DISABLED);
	const DisplayMode displayMode16(5, 6, 5, 0, 16, 0, DisplayMode::DoubleBuffering::ENABLED, DisplayMode::VSync::DISABLED);
	IGfxDevice::GLContextInfo contextInfo(appCfg_);

	if (EglGfxDevice::isModeSupported(state_, contextInfo, displayMode32))
		gfxDevice_ = nctl::makeUnique<EglGfxDevice>(state_, contextInfo, displayMode32);
	else if (EglGfxDevice::isModeSupported(state_, contextInfo, displayMode16))
		gfxDevice_ = nctl::makeUnique<EglGfxDevice>(state_, contextInfo, displayMode16);
	else
	{
		LOGF("Cannot find a suitable EGL configuration, graphics device not created");
		exit(EXIT_FAILURE);
	}
	inputManager_ = nctl::makeUnique<AndroidInputManager>(state_);

	timings_[Timings::PRE_INIT] = profileStartTime_.secondsSince();

	Application::initCommon();

	isInitialized_ = true;
}

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
			theServiceLocator().audioDevice().unfreezePlayers();
		else
			theServiceLocator().audioDevice().freezePlayers();
	}
}

}
