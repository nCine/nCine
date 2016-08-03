#include <android_native_app_glue.h>
#include "AndroidApplication.h"

namespace ncine {

class IAppEventHandler;

}

ncine::IAppEventHandler* createAppEventHandler();

void android_main(struct android_app* state)
{
	// Make sure glue isn't stripped.
	app_dummy();

	ncine::AndroidApplication::start(state, createAppEventHandler);
}
