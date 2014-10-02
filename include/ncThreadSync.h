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
  public:
	ncMutex();
	~ncMutex();

	void Lock();
	void Unlock();
	int TryLock();

  private:
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	CRITICAL_SECTION m_handle;
#else
	pthread_mutex_t m_mutex;
#endif

	friend class ncCondVariable;
};

/// Condition variable class (threads synchronization)
/*! Windows version based on the TinyThread++ library implementation
 * More info at http://www.cs.wustl.edu/~schmidt/win32-cv-1.html */
class ncCondVariable
{
  public:
	ncCondVariable();
	~ncCondVariable();

	void Wait(ncMutex &rMutex);
	void Signal();
	void Broadcast();

  private:
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	HANDLE m_events[2];
	unsigned int m_uWaitersCount;
	CRITICAL_SECTION m_WaitersCountLock;

	void WaitEvents();
#else
	pthread_cond_t m_cond;
#endif
};

#if !defined(_WIN32) && !defined(__WIN32__) && !defined(__WINDOWS__)

/// Read/write lock class (threads synchronization)
class ncRWLock
{
  public:
	ncRWLock();
	~ncRWLock();

	inline void ReadLock() { pthread_rwlock_rdlock(&m_rwlock); }
	inline void WriteLock() { pthread_rwlock_wrlock(&m_rwlock); }
	inline int TryReadLock() { return pthread_rwlock_tryrdlock(&m_rwlock); }
	inline int TryWriteLock() { return pthread_rwlock_trywrlock(&m_rwlock); }
	inline void Unlock() { pthread_rwlock_unlock(&m_rwlock); }

  private:
	pthread_rwlock_t m_rwlock;
};

#if !defined (__ANDROID__) && !defined(__APPLE__)

/// Barrier class (threads synchronization)
class ncBarrier
{
  public:
	// Creates a barrier for the specified amount of waiting threads
	ncBarrier(unsigned int uCount);
	~ncBarrier();

	/// The calling thread waits at the barrier
	inline int Wait() { return pthread_barrier_wait(&m_barrier); }

  private:
	pthread_barrier_t m_barrier;
};

#endif
#endif

#endif
