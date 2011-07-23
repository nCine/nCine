#include <android_native_app_glue.h>
#include "ncApplication.h"
void test_scene(ncApplication::eCommand cmd);
bool bIsPaused = true;

/// Process the next input event.
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		bIsPaused = false;
		return 1;
	}

	return 0;
}

/// Process the next main command
static void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
	switch (cmd)
	{
		case APP_CMD_SAVE_STATE:
			break;
		case APP_CMD_INIT_WINDOW:
			if (app->window != NULL)
			{
				ncApplication::Init(app, test_scene);
				ncApplication::Step();
			}
			break;
		case APP_CMD_TERM_WINDOW:
			ncApplication::Shutdown();
			break;
		case APP_CMD_GAINED_FOCUS:
			bIsPaused = false;
			break;
		case APP_CMD_LOST_FOCUS:
			bIsPaused = true;
			ncApplication::Step();
			break;
	}
}

void android_main(struct android_app* state)
{
	// Make sure glue isn't stripped.
	app_dummy();

	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;

	while (1)
	{
		int ident;
		int events;
		struct android_poll_source* source;

		while ((ident=ALooper_pollAll(!bIsPaused ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
		{
			if (source != NULL)
				source->process(state, source);

			if (state->destroyRequested)
			{
				ncApplication::Shutdown();
				return;
			}
		}

		if (bIsPaused == false)
			ncApplication::Step();
	}
}
