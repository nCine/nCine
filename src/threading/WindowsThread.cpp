#include "common_macros.h"
#include "Thread.h"

#ifdef WITH_TRACY
	#include "common/TracySystem.hpp"
#endif

namespace ncine {

namespace {

#if !defined PTW32_VERSION && !defined __WINPTHREADS_VERSION
	extern "C" typedef HRESULT(WINAPI *t_SetThreadDescription)(HANDLE, PCWSTR);
	const unsigned int MaxThreadNameLength = 256;

	#if !defined(__MINGW32__)

	#pragma pack(push, 8)
	struct THREADNAME_INFO
	{
		DWORD dwType;
		LPCSTR szName;
		DWORD dwThreadID;
		DWORD dwFlags;
	};
	#pragma pack(pop)

	void ThreadNameMsvcMagic(const THREADNAME_INFO &info)
	{
		__try
		{
			RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR *)&info);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
	#endif

	void setThreadName(HANDLE handle, const char *name)
	{
		if (handle == 0)
			return;

	#if !defined(__MINGW32__)
		static auto _SetThreadDescription = reinterpret_cast<t_SetThreadDescription>(GetProcAddress(GetModuleHandleA("kernel32.dll"), "SetThreadDescription"));
		if (_SetThreadDescription)
		{
			wchar_t buffer[MaxThreadNameLength];
			mbstowcs(buffer, name, MaxThreadNameLength);
			const HANDLE threadHandle = (handle != reinterpret_cast<HANDLE>(-1)) ? handle : GetCurrentThread();
			_SetThreadDescription(threadHandle, buffer);
		}
		else
		{
			const DWORD threadId = (handle != reinterpret_cast<HANDLE>(-1)) ? GetThreadId(handle) : GetCurrentThreadId();
			THREADNAME_INFO info;
			info.dwType = 0x1000;
			info.szName = name;
			info.dwThreadID = threadId;
			info.dwFlags = 0;
			ThreadNameMsvcMagic(info);
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
    : handle_(0), threadInfo_(nctl::makeUnique<ThreadInfo>())
{
}

Thread::Thread(ThreadFunctionPtr threadFunction, void *threadArg)
    : Thread()
{
	run(threadFunction, threadArg);
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

void Thread::run(ThreadFunctionPtr threadFunction, void *threadArg)
{
	if (handle_ == 0)
	{
		threadInfo_->threadFunction = threadFunction;
		threadInfo_->threadArg = threadArg;
		handle_ = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, wrapperFunction, threadInfo_.get(), 0, nullptr));
		FATAL_ASSERT_MSG(handle_, "Error in _beginthreadex()");
	}
	else
		LOGW_X("Thread %u is already running", handle_);
}

bool Thread::join()
{
	bool success = false;
	if (handle_ != 0)
	{
		DWORD retValue = WaitForSingleObject(handle_, INFINITE);
		if (retValue != WAIT_FAILED)
		{
			retValue = CloseHandle(handle_);
			success = (retValue != 0);
			handle_ = 0;
		}
	}
	return success;
}

bool Thread::detach()
{
	bool success = false;
	if (handle_ != 0)
	{
		const BOOL retValue = CloseHandle(handle_);
		success = (retValue != 0);
		handle_ = 0;
	}
	return success;
}

void Thread::setName(const char *name)
{
	setThreadName(handle_, name);
}

void Thread::setSelfName(const char *name)
{
#ifdef WITH_TRACY
	tracy::SetThreadName(name);
#else
	setThreadName(reinterpret_cast<HANDLE>(-1), name);
#endif
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

[[noreturn]] void Thread::exit()
{
	_endthreadex(0);
}

void Thread::yieldExecution()
{
	Sleep(0);
}

bool Thread::cancel()
{
	bool success = false;
	if (handle_ != 0)
	{
		BOOL retValue = TerminateThread(handle_, 0);
		if (retValue != 0)
		{
			retValue = CloseHandle(handle_);
			success = (retValue != 0);
			handle_ = 0;
		}
	}
	return success;
}

ThreadAffinityMask Thread::affinityMask() const
{
	ThreadAffinityMask affinityMask;

	if (handle_ != 0)
	{
		// A neutral value for the temporary mask
		const DWORD_PTR allCpus = ~(allCpus & 0);

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
	threadInfo->threadFunction(threadInfo->threadArg);

	return 0;
}

}
