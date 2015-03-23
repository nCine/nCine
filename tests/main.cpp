#include <cstdlib> // for EXIT_SUCCESS
#include "Application.h"

namespace ncine{

class IAppEventHandler;

}

ncine::IAppEventHandler* createApphandler();

int main(int argc, char **argv)
{
	ncine::theApplication().init(createApphandler);
	ncine::theApplication().run();
	ncine::theApplication().shutdown();

	return EXIT_SUCCESS;
}
