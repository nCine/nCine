#include "apptest_threads.h"
#include "ncThread.h"
#include "ncApplication.h"
#include "ncServiceLocator.h"

const int NUM_THREADS = 2;
const int NUM_FLOATS = 100000000;
float *pArray = NULL;

void pThreadFunction(void *arg)
{
	int iThreadNum = *(static_cast<int*>(arg));
	int iStartIndex = iThreadNum * (NUM_FLOATS/NUM_THREADS);
	int iEndIndex = (iThreadNum+1) * (NUM_FLOATS/NUM_THREADS);
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"THREAD #%d: %d-%d", iThreadNum, iStartIndex, iEndIndex);

	for(int i = iStartIndex; i < iEndIndex; i++)
	{
		pArray[i] = 0.0f;

		pArray[i] += 100.0f;
		pArray[i] /= 2.0f;
		pArray[i] *= 2.0f;
		pArray[i] -= 100.0f;

		pArray[i] += 100.0f;
		pArray[i] /= 2.0f;
		pArray[i] *= 2.0f;
		pArray[i] -= 100.0f;
	}
}

ncIAppEventHandler* create_apphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::OnInit()
{
	ncIInputManager::SetHandler(this);

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"APPTEST_THREADS: %d threads for %d numbers on %u processor(s)", NUM_THREADS, NUM_FLOATS, ncThread::NumProcessors());

	ncThread threads[NUM_THREADS];
	int iThreadNum[NUM_THREADS];

	pArray = new float[NUM_FLOATS];

	float fStartTime = ncTimer::Now();
	for (int i = 0; i < NUM_THREADS; i++)
	{
		iThreadNum[i] = i;
		threads[i].Run(pThreadFunction, &iThreadNum[i]);
	}

	for (int i = 0; i < NUM_THREADS; i++)
		threads[i].Join();
	float fEndTime = ncTimer::Now();

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"APPTEST_THREADS: total time %fms", (fEndTime-fStartTime)*1000.0f);

	delete[] pArray;
}

#ifndef __ANDROID__
void MyEventHandler::OnKeyReleased(const ncKeyboardEvent &event)
{
	if (event.sym == NCKEY_ESCAPE || event.sym == NCKEY_Q)
		ncApplication::Quit();
}
#endif
