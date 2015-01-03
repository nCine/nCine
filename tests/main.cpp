#include <cstdlib> // for EXIT_SUCCESS
#include "Application.h"

namespace ncine{

class IAppEventHandler;

}

ncine::IAppEventHandler* createApphandler();

int main(int argc, char **argv)
{
	ncine::Application::init(createApphandler);
	ncine::Application::run();
	ncine::Application::shutdown();

	return EXIT_SUCCESS;
}
