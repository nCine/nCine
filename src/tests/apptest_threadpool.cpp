#include "apptest_threadpool.h"
#include "IThreadPool.h"
#include "ThreadCommands.h"
#include "Application.h"
#include "ServiceLocator.h"
#include "Timer.h"

nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	for (unsigned int i = 0; i < 4; i++)
	{
		nc::theServiceLocator().threadPool().enqueueCommand(new nc::DummyCommand(i));
		LOGI_X("APPTEST_THREADPOOL: enqueued %u", i);
		nc::Timer::sleep(1000);
	}
}

#ifndef __ANDROID__
void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KEY_ESCAPE || event.sym == nc::KEY_Q)
	{
		nc::theApplication().quit();
	}
}
#endif
