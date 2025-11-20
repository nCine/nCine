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
		__except (EXCEPTION_EXECUTE_HANDLER)
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

	Thread::Priority priorityImpl(HANDLE handle)
	{
		const int winPriority = GetThreadPriority(handle);
		switch (winPriority)
		{
			case THREAD_PRIORITY_LOWEST:
				return Thread::Priority::Lowest;
			case THREAD_PRIORITY_BELOW_NORMAL:
				return Thread::Priority::Low;
			default:
			case THREAD_PRIORITY_NORMAL:
				return Thread::Priority::Normal;
			case THREAD_PRIORITY_ABOVE_NORMAL:
				return Thread::Priority::High;
			case THREAD_PRIORITY_HIGHEST:
				return Thread::Priority::Highest;
		}
	}

	bool setPriorityImpl(HANDLE handle, Thread::Priority priority)
	{
		int winPriority = THREAD_PRIORITY_NORMAL;
		switch (priority)
		{
			case Thread::Priority::Lowest:
				winPriority = THREAD_PRIORITY_LOWEST;
				break;
			case Thread::Priority::Low:
				winPriority = THREAD_PRIORITY_BELOW_NORMAL;
				break;
			default:
			case Thread::Priority::Normal:
				winPriority = THREAD_PRIORITY_NORMAL;
				break;
			case Thread::Priority::High:
				winPriority = THREAD_PRIORITY_ABOVE_NORMAL;
				break;
			case Thread::Priority::Highest:
				winPriority = THREAD_PRIORITY_HIGHEST;
				break;
		}

		return SetThreadPriority(handle, winPriority) != 0;
	}

}

///////////////////////////////////////////////////////////
// `ThreadAffinityMask`
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

bool ThreadAffinityMask::isSet(int cpuNum) const
{
	return ((affinityMask_ >> cpuNum) & 1LL) != 0;
}

///////////////////////////////////////////////////////////
// `ThisThread`
///////////////////////////////////////////////////////////

unsigned long int ThisThread::threadId()
{
	return static_cast<unsigned long int>(GetCurrentThreadId());
}

void ThisThread::setName(const char *name)
{
#ifdef WITH_TRACY
	tracy::SetThreadName(name);
#else
	setThreadName(reinterpret_cast<HANDLE>(-1), name);
#endif
}

/*! \note This retrieves the affinity mask only within the processor group the calling thread currently belongs to. */
ThreadAffinityMask ThisThread::affinityMask()
{
	ThreadAffinityMask affinityMask;

	const HANDLE handle = GetCurrentThread();

	DWORD_PTR processAffinityMask = 0;
	DWORD_PTR systemAffinityMask = 0;

	if (GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask))
	{
		// Temporarily set affinity to current process mask (always valid for this thread), then restore it
		DWORD_PTR oldAffinity = SetThreadAffinityMask(handle, processAffinityMask);
		SetThreadAffinityMask(handle, oldAffinity);

		affinityMask.affinityMask_ = oldAffinity;
	}

	return affinityMask;
}

/*! \note This only controls affinity within the first 64 logical processors of the calling thread's group. */
void ThisThread::setAffinityMask(ThreadAffinityMask affinityMask)
{
	SetThreadAffinityMask(GetCurrentThread(), affinityMask.affinityMask_);
}

Thread::Priority ThisThread::priority()
{
	return priorityImpl(GetCurrentThread());
}

bool ThisThread::setPriority(Thread::Priority priority)
{
	return setPriorityImpl(GetCurrentThread(), priority);
}

void ThisThread::yield()
{
	SwitchToThread();
}

[[noreturn]] void ThisThread::exit()
{
	_endthreadex(0);
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Thread::Thread()
    : tid_(0), handle_(0), threadInfo_(nctl::makeUnique<ThreadInfo>())
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

void Thread::run(ThreadFunctionPtr threadFunction, void *threadArg)
{
	if (handle_ == 0)
	{
		threadInfo_->tid = &tid_;
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
	if (handle_ != 0)
		setThreadName(handle_, name);
	else
		LOGW("Cannot set the name for an invalid thread id");
}

/*! \note This retrieves the affinity mask only within the processor group the thread currently belongs to. */
ThreadAffinityMask Thread::affinityMask() const
{
	ThreadAffinityMask affinityMask;
	DWORD_PTR processAffinityMask = 0;
	DWORD_PTR systemAffinityMask = 0;

	if (handle_ != 0 && GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask))
	{
		// Temporarily set affinity to current process mask, that is always valid for this thread, and then restore it
		DWORD_PTR oldAffinity = SetThreadAffinityMask(handle_, processAffinityMask);
		SetThreadAffinityMask(handle_, oldAffinity);

		affinityMask.affinityMask_ = oldAffinity;
	}

	return affinityMask;
}

/*! \note This only controls affinity within the first 64 logical processors of the thread's group. */
void Thread::setAffinityMask(ThreadAffinityMask affinityMask)
{
	if (handle_ != 0)
		SetThreadAffinityMask(handle_, affinityMask.affinityMask_);
	else
		LOGW("Cannot set the affinity mask for an invalid thread id");
}

Thread::Priority Thread::priority() const
{
	if (handle_ == 0)
	{
		LOGW("Cannot get the priority for an invalid thread id");
		return Priority::Normal;
	}

	return priorityImpl(handle_);
}

bool Thread::setPriority(Priority priority)
{
	if (handle_ == 0)
	{
		LOGW("Cannot set the priority for an invalid thread id");
		return false;
	}

	return setPriorityImpl(handle_, priority);
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

unsigned int Thread::numProcessors()
{
	const DWORD count = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
	return (count > 0 ? static_cast<unsigned int>(count) : 1);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int Thread::wrapperFunction(void *arg)
{
	ThreadInfo *threadInfo = static_cast<ThreadInfo *>(arg);
	*threadInfo->tid = ThisThread::threadId();
	threadInfo->threadFunction(threadInfo->threadArg);

	return 0;
}

}
