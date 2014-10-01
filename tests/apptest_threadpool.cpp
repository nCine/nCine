#include "apptest_threadpool.h"
#include "ncThreadPool.h"
#include "ncThreadCommands.h"
#include "ncApplication.h"
#include "ncServiceLocator.h"
#include "ncTimer.h"

ncIAppEventHandler* create_apphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::OnInit()
{
	ncIInputManager::SetHandler(this);

	for (unsigned int i = 0; i < 4; i++)
	{
		ncServiceLocator::ThreadPool().EnqueueCommand(new ncDummyCommand(i));
		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"APPTEST_THREADPOOL: enqueued %u", i);
		ncTimer::Sleep(1000);
	}
}

#ifndef __ANDROID__
void MyEventHandler::OnKeyReleased(const ncKeyboardEvent &event)
{
	if (event.sym == NCKEY_ESCAPE || event.sym == NCKEY_Q)
	{
		ncApplication::Quit();
	}
}
#endif
