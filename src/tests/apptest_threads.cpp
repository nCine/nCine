#include "apptest_threads.h"
#include <ncine/common_macros.h>
#include <ncine/Application.h>
#include <ncine/Thread.h> // after Application.h to compile on MSVC
#include <ncine/TimeStamp.h>

namespace {

const int NumThreads = 2;
const int NumFloats = 100000000;
nctl::UniquePtr<float[]> globalArray;

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

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onInit()
{
	LOGI_X("APPTEST_THREADS: %d threads for %d numbers on %u processor(s)", NumThreads, NumFloats, nc::Thread::numProcessors());

	nc::Thread threads[NumThreads];
	int threadNums[NumThreads];

	globalArray = nctl::makeUnique<float[]>(NumFloats);

	nc::TimeStamp startTime;
	for (int i = 0; i < NumThreads; i++)
	{
		threadNums[i] = i;
		threads[i].run(threadFunction, &threadNums[i]);
	}

	for (int i = 0; i < NumThreads; i++)
		threads[i].join();

	LOGI_X("APPTEST_THREADS: total time %fms", startTime.millisecondsSince());

	globalArray.reset(nullptr);
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
}
