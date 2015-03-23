#include <cstdlib>
#include <android_native_app_glue.h>
#include "AndroidApplication.h"
#include "AndroidInputManager.h"
#include "EglGfxDevice.h"
#include "ServiceLocator.h"

namespace ncine {

class IAppEventHandler;

}

namespace nc = ncine;

nc::IAppEventHandler* createApphandler();

/// Process the next input event.
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
	return static_cast<int32_t>(nc::AndroidInputManager::parseEvent(event));
}

/// Process the next main command
static void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
	static nc::EglGfxDevice *eglGfxDevice = NULL;

	switch (cmd)
	{
		case APP_CMD_INPUT_CHANGED:
			LOGW("engine_handle_cmd - APP_CMD_INPUT_CHANGED event received (not handled)");
			break;

		case APP_CMD_INIT_WINDOW:
			LOGI("engine_handle_cmd - APP_CMD_INIT_WINDOW event received");
			if (app->window != NULL)
			{
				if (nc::theApplication().isInitialized() == false)
				{
					nc::theApplication().init(app);
					eglGfxDevice = &static_cast<nc::EglGfxDevice &>(nc::theApplication().gfxDevice());
					nc::theApplication().step();
				}
				else
				{
					eglGfxDevice->createSurface(app);
					eglGfxDevice->bindContext();
				}
			}
			break;
		case APP_CMD_TERM_WINDOW:
			LOGI("engine_handle_cmd - APP_CMD_TERM_WINDOW event received");
			eglGfxDevice->unbindContext();
			break;
		case APP_CMD_WINDOW_RESIZED:
			LOGI("engine_handle_cmd - APP_CMD_WINDOW_RESIZED event received");
			eglGfxDevice->querySurfaceSize();
			break;
		case APP_CMD_WINDOW_REDRAW_NEEDED:
			LOGI("engine_handle_cmd - APP_CMD_WINDOW_REDRAW_NEEDED event received");
			nc::theApplication().step();
			break;

		case APP_CMD_GAINED_FOCUS:
			LOGI("engine_handle_cmd - APP_CMD_GAINED_FOCUS event received");
			nc::AndroidInputManager::enableAccelerometerSensor();
			nc::theApplication().setFocus(true);
			break;
		case APP_CMD_LOST_FOCUS:
			LOGI("engine_handle_cmd - APP_CMD_LOST_FOCUS event received");
			nc::AndroidInputManager::disableAccelerometerSensor();
			nc::theApplication().setFocus(false);
			nc::theApplication().step();
			break;

		case APP_CMD_CONFIG_CHANGED:
			LOGW("engine_handle_cmd - APP_CMD_CONFIG_CHANGED event received (not handled)");
			break;

		case APP_CMD_LOW_MEMORY:
			LOGW("engine_handle_cmd - APP_CMD_LOW_MEMORY event received (not handled)");
			break;

		case APP_CMD_START:
			if (nc::theApplication().isInitialized() == false)
			{
				nc::theApplication().preInit(createApphandler);
				LOGI("engine_handle_cmd - APP_CMD_START event received (first start)");
			}
			LOGI("engine_handle_cmd - APP_CMD_START event received");
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
			LOGI("engine_handle_cmd - APP_CMD_DESTROY event received");
			nc::theApplication().quit();
			break;
	}
}

void android_main(struct android_app* state)
{
	// Make sure glue isn't stripped.
	app_dummy();

	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;

	while (nc::theApplication().shouldQuit() == false)
	{
		int ident;
		int events;
		struct android_poll_source* source;

		while ((ident = ALooper_pollAll(!nc::theApplication().isPaused() ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
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
				LOGI("android_main - android_app->destroyRequested not equal to zero");
				nc::theApplication().quit();
			}
		}

		if (nc::theApplication().isInitialized() &&
			nc::theApplication().hasFocus() &&
			!nc::theApplication().isPaused())
		{
			nc::AndroidInputManager::updateJoystickConnections();
			nc::theApplication().step();
		}
	}

	nc::theApplication().shutdown();
	ANativeActivity_finish(state->activity);
}
