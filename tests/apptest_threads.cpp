#include "apptest_threads.h"
#include "ncThread.h"
#include "ncApplication.h"
#include "ncServiceLocator.h"

const int NumThreads = 2;
const int NumFloats = 100000000;
float *globalArray = NULL;

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

ncIAppEventHandler* createApphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onInit()
{
	ncIInputManager::setHandler(this);

	LOGI_X("APPTEST_THREADS: %d threads for %d numbers on %u processor(s)", NumThreads, NumFloats, ncThread::numProcessors());

	ncThread threads[NumThreads];
	int threadNums[NumThreads];

	globalArray = new float[NumFloats];

	float startTime = ncTimer::now();
	for (int i = 0; i < NumThreads; i++)
	{
		threadNums[i] = i;
		threads[i].run(threadFunction, &threadNums[i]);
	}

	for (int i = 0; i < NumThreads; i++)
	{
		threads[i].join();
	}
	float endTime = ncTimer::now();

	LOGI_X("APPTEST_THREADS: total time %fms", (endTime - startTime) * 1000.0f);

	delete[] globalArray;
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
