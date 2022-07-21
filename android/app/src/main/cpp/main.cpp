#include <android_native_app_glue.h>
#include <ncine/AndroidApplication.h>

namespace ncine {

class IAppEventHandler;

}

nctl::UniquePtr<ncine::IAppEventHandler> createAppEventHandler();

void android_main(struct android_app *state)
{
	ncine::AndroidApplication::start(state, createAppEventHandler);
}
