#include "common_macros.h"
#include "Thread.h"

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ThreadAffinityMask::zero()
{
	affinityMask_ = 0LL;
}

void ThreadAffinityMask::set(int cpuNum)
{
	affinityMask_ |= 1LL << cpuNum;
}

void ThreadAffinityMask::clear(int cpuNum)
{
	affinityMask_ &= ~(1LL << cpuNum);
}

bool ThreadAffinityMask::isSet(int cpuNum)
{
	return ((affinityMask_ >> cpuNum) & 1LL) != 0;
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Thread::Thread()
	: handle_(0)
{

}

Thread::Thread(ThreadFunctionPtr startFunction, void *arg)
	: handle_(0)
{
	run(startFunction, arg);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int Thread::numProcessors()
{
	unsigned int numProcs = 0;

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	numProcs = si.dwNumberOfProcessors;

	return numProcs;
}

void Thread::run(ThreadFunctionPtr startFunction, void *arg)
{
	if (handle_ == 0)
	{
		threadInfo_.startFunction = startFunction;
		threadInfo_.threadArg = arg;
		handle_ = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, wrapperFunction, &threadInfo_, 0, nullptr));
		FATAL_ASSERT_MSG(handle_, "Error in _beginthreadex()");
	}
	else
		LOGW_X("Thread %u is already running", handle_);
}

void *Thread::join()
{
	WaitForSingleObject(handle_, INFINITE);
	return nullptr;
}

long int Thread::self()
{
	return GetCurrentThreadId();
}

void Thread::exit(void *retVal)
{
	_endthreadex(0);
	*static_cast<unsigned int *>(retVal) = 0;
}

void Thread::yieldExecution()
{
	Sleep(0);
}

void Thread::cancel()
{
	TerminateThread(handle_, 0);
}

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
		LOGW("Cannot get the affinity for a thread that has not been created yet");

	return affinityMask;
}

void Thread::setAffinityMask(ThreadAffinityMask affinityMask)
{
	if (handle_ != 0)
		SetThreadAffinityMask(handle_, affinityMask.affinityMask_);
	else
		LOGW("Cannot set the affinity mask for a not yet created thread");
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int Thread::wrapperFunction(void *arg)
{
	ThreadInfo *threadInfo = static_cast<ThreadInfo *>(arg);
	threadInfo->startFunction(threadInfo->threadArg);

	return 0;
}

}
