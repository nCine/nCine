#include "common_macros.h"
#include "Thread.h"

#ifdef WITH_TRACY
	#include "common/TracySystem.hpp"
#endif

namespace ncine {

namespace {

#ifdef WITH_TRACY
	void setThreadName(HANDLE handle, const char *name)
	{
	#if !defined(__MINGW32__)
		tracy::SetThreadName(handle, name);
	#else
		tracy::SetThreadName(reinterpret_cast<unsigned long long>(handle), name);
	#endif
	}
#elif !defined PTW32_VERSION && !defined __WINPTHREADS_VERSION
	const unsigned int MaxThreadNameLength = 256;

	void setThreadName(HANDLE handle, const char *name)
	{
		if (handle == 0)
			return;
	#if defined(NTDDI_WIN10_RS2) && NTDDI_VERSION >= NTDDI_WIN10_RS2
		wchar_t buffer[MaxThreadNameLength];
		mbstowcs(buffer, name, MaxThreadNameLength);
		const HANDLE threadHandle = (handle != reinterpret_cast<HANDLE>(-1)) ? handle : GetCurrentThread();
		SetThreadDescription(threadHandle, buffer);
	#elif !defined(__MINGW32__)
		const DWORD MS_VC_EXCEPTION = 0x406D1388;

		#pragma pack(push, 8)
		struct THREADNAME_INFO
		{
			DWORD dwType;
			LPCSTR szName;
			DWORD dwThreadID;
			DWORD dwFlags;
		};
		#pragma pack(pop)

		const DWORD threadId = (handle != reinterpret_cast<HANDLE>(-1)) ? GetThreadId(handle) : GetCurrentThreadId();
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = name;
		info.dwThreadID = threadId;
		info.dwFlags = 0;

		__try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR *>(&info));
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	#endif
	}
#endif

}

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
	handle_ = 0;
	return nullptr;
}

void Thread::setName(const char *name)
{
	setThreadName(handle_, name);
}

void Thread::setSelfName(const char *name)
{
	setThreadName(reinterpret_cast<HANDLE>(-1), name);
}

int Thread::priority() const
{
	return (handle_ != 0) ? GetThreadPriority(handle_) : 0;
}

void Thread::setPriority(int priority)
{
	if (handle_ != 0)
		SetThreadPriority(handle_, priority);
}

long int Thread::self()
{
	return GetCurrentThreadId();
}

[[noreturn]] void Thread::exit(void *retVal)
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
	handle_ = 0;
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
