#include "apptest_threads.h"
#include "Application.h"
#include "Thread.h" // after Application.h to compile on MSVC
#include "ServiceLocator.h"
#include "Timer.h"

namespace {
	const int NumThreads = 2;
	const int NumFloats = 100000000;
	float *globalArray = NULL;
}

void threadFunction(void *arg)
{
	int threadNum = *(static_cast<int *>(arg));
	int startIndex = threadNum * (NumFloats / NumThreads);
	int endIndex = (threadNum + 1) * (NumFloats / NumThreads);
	LOGI_X("THREAD #%d: %d-%d", threadNum, startIndex, endIndex);

	for (int i = startIndex; i < endIndex; i++)
	{
		globalArray[i] = 0.0f;

		globalArray[i] += 100.0f;
		globalArray[i] /= 2.0f;
		globalArray[i] *= 2.0f;
		globalArray[i] -= 100.0f;

		globalArray[i] += 100.0f;
		globalArray[i] /= 2.0f;
		globalArray[i] *= 2.0f;
		globalArray[i] -= 100.0f;
	}
}

nc::IAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	nc::IInputManager::setHandler(this);

	LOGI_X("APPTEST_THREADS: %d threads for %d numbers on %u processor(s)", NumThreads, NumFloats, nc::Thread::numProcessors());

	nc::Thread threads[NumThreads];
	int threadNums[NumThreads];

	globalArray = new float[NumFloats];

	float startTime = nc::Timer::now();
	for (int i = 0; i < NumThreads; i++)
	{
		threadNums[i] = i;
		threads[i].run(threadFunction, &threadNums[i]);
	}

	for (int i = 0; i < NumThreads; i++)
	{
		threads[i].join();
	}
	float endTime = nc::Timer::now();

	LOGI_X("APPTEST_THREADS: total time %fms", (endTime - startTime) * 1000.0f);

	delete[] globalArray;
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
