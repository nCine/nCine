#include <cstdlib>
#include <android_native_app_glue.h>
#include "ncApplication.h"
#include "ncAndroidInputManager.h"
#include "ncServiceLocator.h"
class ncIAppEventHandler;

ncIAppEventHandler* create_apphandler();

/// Process the next input event.
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
	ncAndroidInputManager::ParseEvent(event);
	return 1;
}

/// Process the next main command
static void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
	switch (cmd)
	{
		case APP_CMD_INPUT_CHANGED:
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_INPUT_CHANGED event received (not handled)");
			break;

		case APP_CMD_INIT_WINDOW:
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_INIT_WINDOW event received");
			if (app->window != NULL)
			{
				ncApplication::Init(app, create_apphandler);
				ncApplication::Step();
			}
			break;
		case APP_CMD_TERM_WINDOW:
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_TERM_WINDOW event received");
			ncApplication::Quit();
			break;

		case APP_CMD_GAINED_FOCUS:
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_GAINED_FOCUS event received");
			ncAndroidInputManager::EnableAccelerometerSensor();
			ncApplication::SetFocus(true);
			break;
		case APP_CMD_LOST_FOCUS:
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_LOST_FOCUS event received");
			ncAndroidInputManager::DisableAccelerometerSensor();
			ncApplication::SetFocus(false);
			ncApplication::Step();
			break;

		case APP_CMD_CONFIG_CHANGED:
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_CONFIG_CHANGED event received (not handled)");
			break;

		case APP_CMD_LOW_MEMORY:
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_LOW_MEMORY event received (not handled)");
			break;

		case APP_CMD_START:
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_START event received (not handled)");
			break;
		case APP_CMD_RESUME:
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_RESUME event received (not handled)");
			break;
		case APP_CMD_SAVE_STATE:
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_SAVE_STATE event received (not handled)");
			break;
		case APP_CMD_PAUSE:
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_PAUSE event received (not handled)");
			break;
		case APP_CMD_STOP:
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_STOP event received (not handled)");
			break;

		case APP_CMD_DESTROY:
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_DESTROY event received (not handled)");
			break;
	}
}

void android_main(struct android_app* state)
{
	// Make sure glue isn't stripped.
	app_dummy();

	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;

	while (ncApplication::ShouldQuit() == false)
	{
		int ident;
		int events;
		struct android_poll_source* source;

		while ((ident = ALooper_pollAll(!ncApplication::IsPaused() ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
		{
			if (source != NULL)
				source->process(state, source);

			if (ident == LOOPER_ID_USER)
				ncAndroidInputManager::ParseAccelerometerEvent();

			if (state->destroyRequested)
				ncApplication::Quit();
		}

		if (ncApplication::HasFocus() && !ncApplication::IsPaused())
		{
			ncAndroidInputManager::UpdateJoystickConnections();
			ncApplication::Step();
		}
	}

	ncApplication::Shutdown();
	ANativeActivity_finish(state->activity);
}
