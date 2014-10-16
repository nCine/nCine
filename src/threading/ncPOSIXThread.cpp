#include <cstdlib> // for exit()
#include <unistd.h> // for sysconf()
#include <sched.h> // for sched_yield()
#include "ncThread.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// A default constructor for a class without the associated function
ncThread::ncThread()
	: tid_(0)
{

}

/// Creates a thread around a function and runs it
ncThread::ncThread(ncThreadFunctionPtr startFunction, void *arg)
	: tid_(0)
{
	run(startFunction, arg);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

// Gets the number of processors in the machine
unsigned int ncThread::numProcessors()
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
void ncThread::run(ncThreadFunctionPtr startFunction, void *arg)
{
	if (tid_ == 0)
	{
		threadInfo_.startFunction = startFunction;
		threadInfo_.threadArg = arg;
		if (int error = pthread_create(&tid_, NULL, wrapperFunction, &threadInfo_))
		{
			ncServiceLocator::logger().write(ncILogger::LOG_ERROR, (const char *)"ncThread::run - pthread_create error: %d", error);
			::exit(EXIT_FAILURE);
		}
	}
	else
	{
		ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"ncThread::run - thread %u is already running", tid_);
	}
}

/// Joins the thread
void* ncThread::join()
{
	void* pRetVal = NULL;
	pthread_join(tid_, &pRetVal);
	return pRetVal;
}

/// Returns the calling thread id
long int ncThread::self()
{
#if defined(__APPLE__)
	return reinterpret_cast<long int>(pthread_self());
#else
	return static_cast<long int>(pthread_self());
#endif
}

/// Terminates the calling thread
void ncThread::exit(void *retVal)
{
	pthread_exit(retVal);
}

/// Yields the calling thread in favour of another one with the same priority
void ncThread::yieldExecution()
{
	sched_yield();
}

#ifndef __ANDROID__
/// Asks the thread for termination
void ncThread::cancel()
{
	pthread_cancel(tid_);
}
#endif

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// The wrapper start function for thread creation
void *ncThread::wrapperFunction(void *arg)
{
	ncThreadInfo* pThreadInfo = static_cast<ncThreadInfo*>(arg);
	pThreadInfo->startFunction(pThreadInfo->threadArg);

	return NULL;
}
