#ifndef CLASS_NCTHREADSYNC
#define CLASS_NCTHREADSYNC

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	#include <windows.h>
	#include <process.h>
#else
	#include <pthread.h>
#endif

/// Mutex class (threads synchronization)
class ncMutex
{
private:
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	CRITICAL_SECTION m_handle;
#else
	pthread_mutex_t m_mutex;
#endif
public:
	ncMutex();
	~ncMutex();

	void Lock();
	void Unlock();
	int TryLock();

	friend class ncCondVariable;
};

/// Condition variable class (threads synchronization)
/*! Windows version based on the TinyThread++ library implementation 
 * More info at http://www.cs.wustl.edu/~schmidt/win32-cv-1.html */
class ncCondVariable
{
private:
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	HANDLE m_events[2];
	unsigned int m_uWaitersCount;
	CRITICAL_SECTION m_WaitersCountLock;

	void WaitEvents();
#else
	pthread_cond_t m_cond;
#endif
public:
	ncCondVariable();
	~ncCondVariable();

	void Wait(ncMutex &rMutex);
	void Signal();
	void Broadcast();
};

#if !defined(_WIN32) && !defined(__WIN32__) && !defined(__WINDOWS__)

/// Read/write lock class (threads synchronization)
class ncRWLock
{
private:
	pthread_rwlock_t m_rwlock;
public:
	ncRWLock() { pthread_rwlock_init(&m_rwlock, NULL); }
	~ncRWLock() { pthread_rwlock_destroy(&m_rwlock); }

	void ReadLock() { pthread_rwlock_rdlock(&m_rwlock); }
	void WriteLock() { pthread_rwlock_wrlock(&m_rwlock); }
	int TryReadLock() { return pthread_rwlock_tryrdlock(&m_rwlock); }
	int TryWriteLock() { return pthread_rwlock_trywrlock(&m_rwlock); }
	void Unlock() { pthread_rwlock_unlock(&m_rwlock); }
};

#if !defined (__ANDROID__) && !defined(__APPLE__)

/// Barrier class (threads synchronization)
class ncBarrier
{
private:

	pthread_barrier_t m_barrier;
public:
	/// Creates a barrier for the specified amount of waiting threads
	ncBarrier(unsigned int uCount) { pthread_barrier_init(&m_barrier, NULL, uCount); }
	~ncBarrier() { pthread_barrier_destroy(&m_barrier); }

	/// The calling thread waits at the barrier
	int Wait() { return pthread_barrier_wait(&m_barrier); }
};

#endif
#endif

#endif
