#include "ncApplication.h"
class ncIAppEventHandler;

ncIAppEventHandler* create_apphandler();

int main(int argc, char **argv)
{
	ncApplication::Init(create_apphandler);
	ncApplication::Run();
	ncApplication::Shutdown();

	return 0;
}
