#include "apptest_threadpool.h"
#include "ncThreadPool.h"
#include "ncThreadCommands.h"
#include "ncApplication.h"
#include "ncServiceLocator.h"
#include "ncTimer.h"

ncIAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	ncIInputManager::setHandler(this);

	for (unsigned int i = 0; i < 4; i++)
	{
		ncServiceLocator::threadPool().enqueueCommand(new ncDummyCommand(i));
		ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"APPTEST_THREADPOOL: enqueued %u", i);
		ncTimer::sleep(1000);
	}
}

#ifndef __ANDROID__
void MyEventHandler::onKeyReleased(const ncKeyboardEvent &event)
{
	if (event.sym == NCKEY_ESCAPE || event.sym == NCKEY_Q)
	{
		ncApplication::quit();
	}
}
#endif
