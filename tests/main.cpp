#ifdef WITH_QT5
	#include <QApplication>
	#include <ncine/Qt5Widget.h>
#endif

#include <ncine/PCApplication.h>

#if defined(_WIN32) && !defined(WITH_QT5)
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

nctl::UniquePtr<ncine::IAppEventHandler> createAppEventHandler();

#if defined(_WIN32) && !defined(WITH_QT5)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char **argv)
#endif
{
#ifdef WITH_QT5
	QApplication app(argc, argv);
	ncine::Qt5Widget *ncWidget = new ncine::Qt5Widget(createAppEventHandler);
	ncWidget->show();
	return app.exec();
#else
	return ncine::PCApplication::start(createAppEventHandler);
#endif
}
