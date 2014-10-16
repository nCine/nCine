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

	void lock();
	void unlock();
	int tryLock();

  private:
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	CRITICAL_SECTION handle_;
#else
	pthread_mutex_t mutex_;
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

	void wait(ncMutex &mutex);
	void signal();
	void broadcast();

  private:
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	HANDLE events_[2];
	unsigned int waitersCount_;
	CRITICAL_SECTION waitersCountLock_;

	void waitEvents();
#else
	pthread_cond_t cond_;
#endif
};

#if !defined(_WIN32) && !defined(__WIN32__) && !defined(__WINDOWS__)

/// Read/write lock class (threads synchronization)
class ncRWLock
{
  public:
	ncRWLock();
	~ncRWLock();

	inline void readLock() { pthread_rwlock_rdlock(&rwlock_); }
	inline void writeLock() { pthread_rwlock_wrlock(&rwlock_); }
	inline int tryReadLock() { return pthread_rwlock_tryrdlock(&rwlock_); }
	inline int tryWriteLock() { return pthread_rwlock_trywrlock(&rwlock_); }
	inline void unlock() { pthread_rwlock_unlock(&rwlock_); }

  private:
	pthread_rwlock_t rwlock_;
};

#if !defined (__ANDROID__) && !defined(__APPLE__)

/// Barrier class (threads synchronization)
class ncBarrier
{
  public:
	// Creates a barrier for the specified amount of waiting threads
	ncBarrier(unsigned int count);
	~ncBarrier();

	/// The calling thread waits at the barrier
	inline int wait() { return pthread_barrier_wait(&barrier_); }

  private:
	pthread_barrier_t barrier_;
};

#endif
#endif

#endif
