#include <cstdlib>
#include <android_native_app_glue.h>
#include "ncApplication.h"
#include "ncAndroidInputManager.h"
#include "ncServiceLocator.h"
class ncIAppEventHandler;

ncIAppEventHandler* createApphandler();

/// Process the next input event.
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
	ncAndroidInputManager::parseEvent(event);
	return 1;
}

/// Process the next main command
static void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
	switch (cmd)
	{
		case APP_CMD_INPUT_CHANGED:
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_INPUT_CHANGED event received (not handled)");
			break;

		case APP_CMD_INIT_WINDOW:
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_INIT_WINDOW event received");
			if (app->window != NULL)
			{
				ncApplication::init(app, createApphandler);
				ncApplication::step();
			}
			break;
		case APP_CMD_TERM_WINDOW:
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_TERM_WINDOW event received");
			ncApplication::quit();
			break;

		case APP_CMD_GAINED_FOCUS:
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_GAINED_FOCUS event received");
			ncAndroidInputManager::enableAccelerometerSensor();
			ncApplication::setFocus(true);
			break;
		case APP_CMD_LOST_FOCUS:
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_LOST_FOCUS event received");
			ncAndroidInputManager::disableAccelerometerSensor();
			ncApplication::setFocus(false);
			ncApplication::step();
			break;

		case APP_CMD_CONFIG_CHANGED:
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_CONFIG_CHANGED event received (not handled)");
			break;

		case APP_CMD_LOW_MEMORY:
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_LOW_MEMORY event received (not handled)");
			break;

		case APP_CMD_START:
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_START event received (not handled)");
			break;
		case APP_CMD_RESUME:
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_RESUME event received (not handled)");
			break;
		case APP_CMD_SAVE_STATE:
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_SAVE_STATE event received (not handled)");
			break;
		case APP_CMD_PAUSE:
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_PAUSE event received (not handled)");
			break;
		case APP_CMD_STOP:
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_STOP event received (not handled)");
			break;

		case APP_CMD_DESTROY:
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"engine_handle_cmd - APP_CMD_DESTROY event received (not handled)");
			break;
	}
}

void android_main(struct android_app* state)
{
	// Make sure glue isn't stripped.
	app_dummy();

	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;

	while (ncApplication::shouldQuit() == false)
	{
		int ident;
		int events;
		struct android_poll_source* source;

		while ((ident = ALooper_pollAll(!ncApplication::isPaused() ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
		{
			if (source != NULL)
			{
				source->process(state, source);
			}

			if (ident == LOOPER_ID_USER)
			{
				ncAndroidInputManager::parseAccelerometerEvent();
			}

			if (state->destroyRequested)
			{
				ncApplication::quit();
			}
		}

		if (ncApplication::hasFocus() && !ncApplication::isPaused())
		{
			ncAndroidInputManager::updateJoystickConnections();
			ncApplication::step();
		}
	}

	ncApplication::shutdown();
	ANativeActivity_finish(state->activity);
}
