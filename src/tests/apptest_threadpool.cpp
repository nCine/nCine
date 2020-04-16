#include "apptest_threadpool.h"
#include <ncine/IThreadPool.h>
#include <ncine/ThreadCommands.h>
#include <ncine/Application.h>
#include <ncine/AppConfiguration.h>
#include <ncine/Timer.h>

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	config.withThreads = true;
}

void MyEventHandler::onInit()
{
	for (unsigned int i = 0; i < 4; i++)
	{
		nc::theServiceLocator().threadPool().enqueueCommand(nctl::makeUnique<nc::DummyCommand>(i));
		LOGI_X("APPTEST_THREADPOOL: enqueued %u", i);
		nc::Timer::sleep(1.0f);
	}
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
}
