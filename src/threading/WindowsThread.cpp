#include "Thread.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Clears the CPU set
void ThreadAffinityMask::zero()
{
	affinityMask_ = 0LL;
}

/// Sets the specified CPU number to be included in the set
void ThreadAffinityMask::set(int cpuNum)
{
	affinityMask_ |= 1LL << cpuNum;
}

/// Sets the specified CPU number to be excluded by the set
void ThreadAffinityMask::clear(int cpuNum)
{
	affinityMask_ &= ~(1LL << cpuNum);
}

/// Returns true if the specified CPU number belongs to the set
bool ThreadAffinityMask::isSet(int cpuNum)
{
	return ((affinityMask_ >> cpuNum) & 1LL) != 0;
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// A default constructor for a class without the associated function
Thread::Thread()
	: handle_(0)
{

}

/// Creates a thread around a function and runs it
Thread::Thread(ThreadFunctionPtr startFunction, void* arg)
	: handle_(0)
{
	run(startFunction, arg);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Gets the number of processors in the machine
unsigned int Thread::numProcessors()
{
	unsigned int numProcs = 0;

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	numProcs = si.dwNumberOfProcessors;

	return numProcs;
}

/// Spawns a new thread if the class hasn't one already associated
void Thread::run(ThreadFunctionPtr startFunction, void* arg)
{
	if (handle_ == 0)
	{
		threadInfo_.startFunction = startFunction;
		threadInfo_.threadArg = arg;
		handle_ = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, wrapperFunction, &threadInfo_, 0, NULL));
		if (handle_ == 0)
		{
			LOGE("Error in _beginthreadex()");
			::exit(EXIT_FAILURE);
		}
	}
	else
	{
		LOGW_X("Thread %u is already running", handle_);
	}
}

/// Joins the thread
void* Thread::join()
{
	WaitForSingleObject(handle_, INFINITE);
	return NULL;
}

/// Returns the calling thread id
long int Thread::self()
{
	return GetCurrentThreadId();
}

/// Terminates the calling thread
void Thread::exit(void *retVal)
{
	_endthreadex(0);
	*static_cast<unsigned int *>(retVal) = 0;
}

/// Yields the calling thread in favour of another one with the same priority
void Thread::yieldExecution()
{
	Sleep(0);
}

/// Asks the thread for termination
void Thread::cancel()
{
	TerminateThread(handle_, 0);
}

/// Gets the thread affinity mask
ThreadAffinityMask Thread::affinityMask() const
{
	ThreadAffinityMask affinityMask;

	if (handle_ != 0)
	{
		// A neutral value for the temporary mask
		DWORD_PTR allCpus = ~(allCpus & 0);

		affinityMask.affinityMask_ = SetThreadAffinityMask(handle_, allCpus);
		SetThreadAffinityMask(handle_, affinityMask.affinityMask_);
	}
	else
	{
		LOGW("Cannot get the affinity for a thread that has not been created yet");
	}

	return affinityMask;
}

/// Sets the thread affinity mask
void Thread::setAffinityMask(ThreadAffinityMask affinityMask)
{
	if (handle_ != 0)
	{
		SetThreadAffinityMask(handle_, affinityMask.affinityMask_);
	}
	else
	{
		LOGW("Cannot set the affinity mask for a not yet created thread");
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// The wrapper start function for thread creation
unsigned int Thread::wrapperFunction(void* arg)
{
	ThreadInfo* threadInfo = static_cast<ThreadInfo*>(arg);
	threadInfo->startFunction(threadInfo->threadArg);

	return 0;
}

}
