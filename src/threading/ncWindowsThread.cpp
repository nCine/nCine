#include "ncThread.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// A default constructor for a class without the associated function
ncThread::ncThread()
	: handle_(0)
{

}

/// Creates a thread around a function and runs it
ncThread::ncThread(ncThreadFunctionPtr startFunction, void* arg)
	: handle_(0)
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

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	numProcs = si.dwNumberOfProcessors;

	return numProcs;
}

/// Spawns a new thread if the class hasn't one already associated
void ncThread::run(ncThreadFunctionPtr startFunction, void* arg)
{
	if (handle_ == 0)
	{
		threadInfo_.startFunction = startFunction;
		threadInfo_.threadArg = arg;
		handle_ = (HANDLE)_beginthreadex(NULL, 0, wrapperFunction, &threadInfo_, 0, NULL);
		if (handle_ <= 0)
		{
			LOGE("_beginthreadex error");
			::exit(EXIT_FAILURE);
		}
	}
	else
	{
		LOGW_X("thread %u is already running", handle_);
	}
}

/// Joins the thread
void* ncThread::join()
{
	WaitForSingleObject(handle_, INFINITE);
	return NULL;
}

/// Returns the calling thread id
long int ncThread::self()
{
	return GetCurrentThreadId();
}

/// Terminates the calling thread
void ncThread::exit(void *retVal)
{
	_endthreadex(0);
	*static_cast<unsigned int *>(retVal) = 0;
}

/// Yields the calling thread in favour of another one with the same priority
void ncThread::yieldExecution()
{
	Sleep(0);
}

/// Asks the thread for termination
void ncThread::cancel()
{
	TerminateThread(handle_, 0);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// The wrapper start function for thread creation
unsigned int ncThread::wrapperFunction(void* arg)
{
	ncThreadInfo* threadInfo = static_cast<ncThreadInfo*>(arg);
	threadInfo->startFunction(threadInfo->threadArg);

	return 0;
}
