#include <cstdlib> // for EXIT_SUCCESS
#include "PCApplication.h"

#if defined(_WIN32) && defined(NO_INTEGRATED_GPU)
extern "C"
{
	_declspec(dllexport) unsigned long int NvOptimusEnablement = 0x00000001;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
};
#endif

namespace ncine{

class IAppEventHandler;

}

ncine::IAppEventHandler* createApphandler();

int main(int argc, char **argv)
{
	ncine::PCApplication::start(createApphandler);
	return EXIT_SUCCESS;
}
