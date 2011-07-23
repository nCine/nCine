#include "ncApplication.h"

void test_scene(ncApplication::eCommand cmd);

int main(int argc, char **argv)
{
	ncApplication::Init(test_scene);
	ncApplication::Run();
	ncApplication::Shutdown();

	return 0;	
}
