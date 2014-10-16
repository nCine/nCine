#include "ncApplication.h"
class ncIAppEventHandler;

ncIAppEventHandler* createApphandler();

int main(int argc, char **argv)
{
	ncApplication::init(createApphandler);
	ncApplication::run();
	ncApplication::shutdown();

	return 0;
}
