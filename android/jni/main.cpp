#include <cstdlib>
#include <android_native_app_glue.h>
#include "Application.h"
#include "AndroidInputManager.h"
#include "ServiceLocator.h"

namespace ncine {

class IAppEventHandler;

}

namespace nc = ncine;

nc::IAppEventHandler* createApphandler();

/// Process the next input event.
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
	nc::AndroidInputManager::parseEvent(event);
	return 1;
}

/// Process the next main command
static void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
	switch (cmd)
	{
		case APP_CMD_INPUT_CHANGED:
			LOGW("engine_handle_cmd - APP_CMD_INPUT_CHANGED event received (not handled)");
			break;

		case APP_CMD_INIT_WINDOW:
			LOGW("engine_handle_cmd - APP_CMD_INIT_WINDOW event received");
			if (app->window != NULL)
			{
				nc::Application::init(app, createApphandler);
				nc::Application::step();
			}
			break;
		case APP_CMD_TERM_WINDOW:
			LOGW("engine_handle_cmd - APP_CMD_TERM_WINDOW event received");
			nc::Application::quit();
			break;

		case APP_CMD_GAINED_FOCUS:
			LOGW("engine_handle_cmd - APP_CMD_GAINED_FOCUS event received");
			nc::AndroidInputManager::enableAccelerometerSensor();
			nc::Application::setFocus(true);
			break;
		case APP_CMD_LOST_FOCUS:
			LOGW("engine_handle_cmd - APP_CMD_LOST_FOCUS event received");
			nc::AndroidInputManager::disableAccelerometerSensor();
			nc::Application::setFocus(false);
			nc::Application::step();
			break;

		case APP_CMD_CONFIG_CHANGED:
			LOGW("engine_handle_cmd - APP_CMD_CONFIG_CHANGED event received (not handled)");
			break;

		case APP_CMD_LOW_MEMORY:
			LOGW("engine_handle_cmd - APP_CMD_LOW_MEMORY event received (not handled)");
			break;

		case APP_CMD_START:
			LOGW("engine_handle_cmd - APP_CMD_START event received (not handled)");
			break;
		case APP_CMD_RESUME:
			LOGW("engine_handle_cmd - APP_CMD_RESUME event received (not handled)");
			break;
		case APP_CMD_SAVE_STATE:
			LOGW("engine_handle_cmd - APP_CMD_SAVE_STATE event received (not handled)");
			break;
		case APP_CMD_PAUSE:
			LOGW("engine_handle_cmd - APP_CMD_PAUSE event received (not handled)");
			break;
		case APP_CMD_STOP:
			LOGW("engine_handle_cmd - APP_CMD_STOP event received (not handled)");
			break;

		case APP_CMD_DESTROY:
			LOGW("engine_handle_cmd - APP_CMD_DESTROY event received (not handled)");
			break;
	}
}

void android_main(struct android_app* state)
{
	// Make sure glue isn't stripped.
	app_dummy();

	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;

	while (nc::Application::shouldQuit() == false)
	{
		int ident;
		int events;
		struct android_poll_source* source;

		while ((ident = ALooper_pollAll(!nc::Application::isPaused() ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
		{
			if (source != NULL)
			{
				source->process(state, source);
			}

			if (ident == LOOPER_ID_USER)
			{
				nc::AndroidInputManager::parseAccelerometerEvent();
			}

			if (state->destroyRequested)
			{
				nc::Application::quit();
			}
		}

		if (nc::Application::hasFocus() && !nc::Application::isPaused())
		{
			nc::AndroidInputManager::updateJoystickConnections();
			nc::Application::step();
		}
	}

	nc::Application::shutdown();
	ANativeActivity_finish(state->activity);
}
