#include <ncine/PCApplication.h>

#ifdef _WIN32
	#include <ncine/common_windefines.h>
	#include <windef.h>
#endif

#if defined(_WIN32) && defined(NO_INTEGRATED_GPU)
extern "C"
{
	_declspec(dllexport) unsigned long int NvOptimusEnablement = 0x00000001;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
};
#endif

namespace ncine {

class IAppEventHandler;

}

ncine::IAppEventHandler *createAppEventHandler();

int main(int argc, char **argv)
{
	return ncine::PCApplication::start(createAppEventHandler);
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	return ncine::PCApplication::start(createAppEventHandler);
}
#endif
