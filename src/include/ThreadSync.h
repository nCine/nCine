#ifndef CLASS_NCINE_THREADSYNC
#define CLASS_NCINE_THREADSYNC

#if defined(_WIN32)
	#include "common_windefines.h"
	#include <windef.h>
	#include <winbase.h>
#else
	#include <pthread.h>
#endif

#include "tracy.h"

#ifdef WITH_TRACY
	#define TRACY_SOURCE_LOCATION \
		([]() -> const tracy::SourceLocationData * { \
			static constexpr tracy::SourceLocationData srcLoc { nullptr, __func__, __FILE__, __LINE__, 0 }; \
			return &srcLoc; \
		})()
#endif

namespace ncine {

/// Mutex class (threads synchronization)
class Mutex
{
  public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();
	bool tryLock();

#ifdef WITH_TRACY
	inline bool try_lock() { return tryLock(); }
	inline void setTracyName(const char *name) { tracyCtx_.CustomName(name, strlen(name)); }
#endif

  private:
#if defined(_WIN32)
	CRITICAL_SECTION handle_;
#else
	pthread_mutex_t mutex_;
#endif

#ifdef WITH_TRACY
	tracy::LockableCtx tracyCtx_;
#endif

	/// Deleted copy constructor
	Mutex(const Mutex &) = delete;
	/// Deleted assignment operator
	Mutex &operator=(const Mutex &) = delete;

	friend class CondVariable;
};

/// Condition variable class (threads synchronization)
class CondVariable
{
  public:
	CondVariable();
#if defined(_WIN32)
	~CondVariable() = default;
#else
	~CondVariable();
#endif

	void wait(Mutex &mutex);
	void signal();
	void broadcast();

  private:
#if defined(_WIN32)
	CONDITION_VARIABLE handle_;
#else
	pthread_cond_t cond_;
#endif

	/// Deleted copy constructor
	CondVariable(const CondVariable &) = delete;
	/// Deleted assignment operator
	CondVariable &operator=(const CondVariable &) = delete;
};

#if !defined(_WIN32)

/// Read/write lock class (threads synchronization)
class RWLock
{
  public:
	RWLock();
	~RWLock();

	inline void readLock() { pthread_rwlock_rdlock(&rwlock_); }
	inline void writeLock() { pthread_rwlock_wrlock(&rwlock_); }
	inline int tryReadLock() { return pthread_rwlock_tryrdlock(&rwlock_); }
	inline int tryWriteLock() { return pthread_rwlock_trywrlock(&rwlock_); }
	inline void unlock() { pthread_rwlock_unlock(&rwlock_); }

  private:
	pthread_rwlock_t rwlock_;

	/// Deleted copy constructor
	RWLock(const RWLock &) = delete;
	/// Deleted assignment operator
	RWLock &operator=(const RWLock &) = delete;
};

	#if !defined(__ANDROID__) && !defined(__APPLE__)

/// Barrier class (threads synchronization)
class Barrier
{
  public:
	/// Creates a barrier for the specified amount of waiting threads
	explicit Barrier(unsigned int count);
	~Barrier();

	/// The calling thread waits at the barrier
	inline int wait() { return pthread_barrier_wait(&barrier_); }

  private:
	pthread_barrier_t barrier_;

	/// Deleted copy constructor
	Barrier(const Barrier &) = delete;
	/// Deleted assignment operator
	Barrier &operator=(const Barrier &) = delete;
};

	#endif
#endif

}

#endif
