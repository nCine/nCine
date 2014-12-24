#include <cstdlib> // for exit()
#include <unistd.h> // for sysconf()
#include <sched.h> // for sched_yield()
#include "Thread.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// A default constructor for a class without the associated function
Thread::Thread()
	: tid_(0)
{

}

/// Creates a thread around a function and runs it
Thread::Thread(ThreadFunctionPtr startFunction, void *arg)
	: tid_(0)
{
	run(startFunction, arg);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

// Gets the number of processors in the machine
unsigned int Thread::numProcessors()
{
	unsigned int numProcs = 0;

	long int confRet = -1;
#if defined(_SC_NPROCESSORS_ONLN)
	confRet = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_SC_NPROC_ONLN)
	confRet = sysconf(_SC_NPROC_ONLN);
#endif

	if (confRet > 0)
	{
		numProcs = static_cast<unsigned int>(confRet);
	}

	return numProcs;
}

/// Spawns a new thread if the class hasn't one already associated
void Thread::run(ThreadFunctionPtr startFunction, void *arg)
{
	if (tid_ == 0)
	{
		threadInfo_.startFunction = startFunction;
		threadInfo_.threadArg = arg;
		if (int error = pthread_create(&tid_, NULL, wrapperFunction, &threadInfo_))
		{
			LOGE_X("pthread_create error: %d", error);
			::exit(EXIT_FAILURE);
		}
	}
	else
	{
		LOGW_X("thread %u is already running", tid_);
	}
}

/// Joins the thread
void* Thread::join()
{
	void* pRetVal = NULL;
	pthread_join(tid_, &pRetVal);
	return pRetVal;
}

/// Returns the calling thread id
long int Thread::self()
{
#if defined(__APPLE__)
	return reinterpret_cast<long int>(pthread_self());
#else
	return static_cast<long int>(pthread_self());
#endif
}

/// Terminates the calling thread
void Thread::exit(void *retVal)
{
	pthread_exit(retVal);
}

/// Yields the calling thread in favour of another one with the same priority
void Thread::yieldExecution()
{
	sched_yield();
}

#ifndef __ANDROID__
/// Asks the thread for termination
void Thread::cancel()
{
	pthread_cancel(tid_);
}
#endif

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// The wrapper start function for thread creation
void *Thread::wrapperFunction(void *arg)
{
	ThreadInfo* pThreadInfo = static_cast<ThreadInfo*>(arg);
	pThreadInfo->startFunction(pThreadInfo->threadArg);

	return NULL;
}

}
