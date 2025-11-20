#include "ThreadSync.h"

namespace ncine {

///////////////////////////////////////////////////////////
// Mutex CLASS
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Mutex::Mutex()
#ifdef WITH_TRACY
    : tracyCtx_(TRACY_SOURCE_LOCATION)
#endif
{
	InitializeCriticalSection(&handle_);
}

Mutex::~Mutex()
{
	DeleteCriticalSection(&handle_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Mutex::lock()
{
#ifdef WITH_TRACY
	const bool mark = tracyCtx_.BeforeLock();
#endif

	EnterCriticalSection(&handle_);

#ifdef WITH_TRACY
	if (mark)
		tracyCtx_.AfterLock();
#endif
}

void Mutex::unlock()
{
	LeaveCriticalSection(&handle_);

#ifdef WITH_TRACY
	tracyCtx_.AfterUnlock();
#endif
}

bool Mutex::tryLock()
{
	const bool acquired = (TryEnterCriticalSection(&handle_) != 0);

#ifdef WITH_TRACY
	tracyCtx_.AfterTryLock(acquired);
#endif

	return acquired;
}

///////////////////////////////////////////////////////////
// CondVariable CLASS
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

CondVariable::CondVariable()
{
	InitializeConditionVariable(&handle_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void CondVariable::wait(Mutex &mutex)
{
	SleepConditionVariableCS(&handle_, &mutex.handle_, INFINITE);
}

void CondVariable::signal()
{
	WakeConditionVariable(&handle_);
}

void CondVariable::broadcast()
{
	WakeAllConditionVariable(&handle_);
}

///////////////////////////////////////////////////////////
// Semaphore CLASS
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Semaphore::Semaphore()
    : Semaphore(0)
{
}

Semaphore::Semaphore(unsigned int initialCount)
{
	// Setting `LONG_MAX` as the maximum value, effectively unlimited for typical uses
	handle_ = CreateSemaphore(nullptr, initialCount, 0x7FFFFFFF, nullptr);
}

Semaphore::~Semaphore()
{
	CloseHandle(handle_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Semaphore::wait()
{
	WaitForSingleObject(handle_, INFINITE);
}

void Semaphore::signal()
{
	ReleaseSemaphore(handle_, 1, nullptr);
}

bool Semaphore::tryWait()
{
	const DWORD result = WaitForSingleObject(handle_, 0);
	return (result == WAIT_OBJECT_0);
}

void Semaphore::wait(unsigned int count)
{
	for (unsigned int i = 0; i < count; i++)
		WaitForSingleObject(handle_, INFINITE);
}

void Semaphore::signal(unsigned int count)
{
	ReleaseSemaphore(handle_, count, nullptr);
}

#if HAVE_USER_SEMAPHORE
///////////////////////////////////////////////////////////
// UserSemaphore CLASS
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

UserSemaphore::UserSemaphore()
    : UserSemaphore(0)
{
}

UserSemaphore::UserSemaphore(int initialCount)
    : count_(initialCount)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void UserSemaphore::wait()
{
	const int32_t c = count_.fetchSub(1, nctl::MemoryModel::ACQUIRE);
	if (c <= 0)
	{
		int32_t expected = c - 1;
		while (count_.load(nctl::MemoryModel::RELAXED) < 0)
		{
			WaitOnAddress(&count_, &expected, sizeof(expected), INFINITE);
			expected = count_.load(nctl::MemoryModel::RELAXED);
		}
	}
}

void UserSemaphore::signal()
{
	const int32_t c = count_.fetchAdd(1, nctl::MemoryModel::RELEASE);
	if (c < 0)
		WakeByAddressSingle(&count_);
}

bool UserSemaphore::tryWait()
{
	int32_t c = count_.load(nctl::MemoryModel::RELAXED);
	while (c > 0)
	{
		if (count_.cmpExchange(c, c - 1, nctl::MemoryModel::ACQUIRE))
			return true;
	}
	return false;
}

void UserSemaphore::wait(unsigned int count)
{
	const int32_t c = count_.fetchSub(count, nctl::MemoryModel::ACQUIRE);
	if (c < static_cast<int32_t>(count))
	{
		int32_t expected = count_.load(nctl::MemoryModel::RELAXED);
		while (count_.load(nctl::MemoryModel::RELAXED) < 0)
		{
			WaitOnAddress(&count_, &expected, sizeof(expected), INFINITE);
			expected = count_.load(nctl::MemoryModel::RELAXED);
		}
	}
}

void UserSemaphore::signal(unsigned int count)
{
	const int32_t c = count_.fetchAdd(count, nctl::MemoryModel::RELEASE);
	if (c < 0)
		WakeByAddressAll(&count_);
}
#endif

}
