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

}
