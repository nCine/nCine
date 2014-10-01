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
	InitializeCriticalSection(&m_handle);
}

ncMutex::~ncMutex()
{
	DeleteCriticalSection(&m_handle);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncMutex::Lock()
{
	EnterCriticalSection(&m_handle);
}

void ncMutex::Unlock()
{
	LeaveCriticalSection(&m_handle);
}

int ncMutex::TryLock()
{
	return TryEnterCriticalSection(&m_handle);
}


///////////////////////////////////////////////////////////
// ncCondVariable CLASS
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncCondVariable::ncCondVariable()
	: m_uWaitersCount(0)
{
	m_events[0] = CreateEvent(NULL, FALSE, FALSE, NULL); // Signal
	m_events[1] = CreateEvent(NULL, TRUE, FALSE, NULL); // Broadcast
	InitializeCriticalSection(&m_WaitersCountLock);
}

ncCondVariable::~ncCondVariable()
{
	CloseHandle(m_events[0]); // Signal
	CloseHandle(m_events[1]); // Broadcast
	DeleteCriticalSection(&m_WaitersCountLock);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncCondVariable::Wait(ncMutex &rMutex)
{
	EnterCriticalSection(&m_WaitersCountLock);
	m_uWaitersCount++;
	LeaveCriticalSection(&m_WaitersCountLock);

	rMutex.Unlock();
	WaitEvents();
	rMutex.Lock();
}

void ncCondVariable::Signal()
{
	EnterCriticalSection(&m_WaitersCountLock);
	bool bHaveWaiters = (m_uWaitersCount > 0);
	LeaveCriticalSection(&m_WaitersCountLock);

	if (bHaveWaiters)
	{
		SetEvent(m_events[0]);    // Signal
	}
}

void ncCondVariable::Broadcast()
{
	EnterCriticalSection(&m_WaitersCountLock);
	bool bHaveWaiters = (m_uWaitersCount > 0);
	LeaveCriticalSection(&m_WaitersCountLock);

	if (bHaveWaiters)
	{
		SetEvent(m_events[1]);    // Broadcast
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncCondVariable::WaitEvents()
{
	int iResult = WaitForMultipleObjects(2, m_events, FALSE, INFINITE);

	EnterCriticalSection(&m_WaitersCountLock);
	m_uWaitersCount--;
	bool bLastWaiter = (iResult == (WAIT_OBJECT_0 + 1)) && (m_uWaitersCount == 0);
	LeaveCriticalSection(&m_WaitersCountLock);

	if (bLastWaiter)
	{
		ResetEvent(m_events[1]);    // Broadcast
	}
}
