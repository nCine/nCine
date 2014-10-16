#include "ncThreadSync.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// ncMutex CLASS
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncMutex::ncMutex()
{
	InitializeCriticalSection(&handle_);
}

ncMutex::~ncMutex()
{
	DeleteCriticalSection(&handle_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncMutex::lock()
{
	EnterCriticalSection(&handle_);
}

void ncMutex::unlock()
{
	LeaveCriticalSection(&handle_);
}

int ncMutex::tryLock()
{
	return TryEnterCriticalSection(&handle_);
}


///////////////////////////////////////////////////////////
// ncCondVariable CLASS
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncCondVariable::ncCondVariable()
	: waitersCount_(0)
{
	events_[0] = CreateEvent(NULL, FALSE, FALSE, NULL); // Signal
	events_[1] = CreateEvent(NULL, TRUE, FALSE, NULL); // Broadcast
	InitializeCriticalSection(&waitersCountLock_);
}

ncCondVariable::~ncCondVariable()
{
	CloseHandle(events_[0]); // Signal
	CloseHandle(events_[1]); // Broadcast
	DeleteCriticalSection(&waitersCountLock_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncCondVariable::wait(ncMutex &mutex)
{
	EnterCriticalSection(&waitersCountLock_);
	waitersCount_++;
	LeaveCriticalSection(&waitersCountLock_);

	mutex.unlock();
	waitEvents();
	mutex.lock();
}

void ncCondVariable::signal()
{
	EnterCriticalSection(&waitersCountLock_);
	bool haveWaiters = (waitersCount_ > 0);
	LeaveCriticalSection(&waitersCountLock_);

	if (haveWaiters)
	{
		SetEvent(events_[0]);    // Signal
	}
}

void ncCondVariable::broadcast()
{
	EnterCriticalSection(&waitersCountLock_);
	bool haveWaiters = (waitersCount_ > 0);
	LeaveCriticalSection(&waitersCountLock_);

	if (haveWaiters)
	{
		SetEvent(events_[1]);    // Broadcast
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncCondVariable::waitEvents()
{
	int result = WaitForMultipleObjects(2, events_, FALSE, INFINITE);

	EnterCriticalSection(&waitersCountLock_);
	waitersCount_--;
	bool isLastWaiter = (result == (WAIT_OBJECT_0 + 1)) && (waitersCount_ == 0);
	LeaveCriticalSection(&waitersCountLock_);

	if (isLastWaiter)
	{
		ResetEvent(events_[1]);    // Broadcast
	}
}
