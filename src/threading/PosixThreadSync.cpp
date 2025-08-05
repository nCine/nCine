#include "ThreadSync.h"

#ifdef __linux__
	#include <linux/futex.h>
	#include <sys/syscall.h>
	#include <unistd.h>
#endif

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
	pthread_mutex_init(&mutex_, nullptr);
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&mutex_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Mutex::lock()
{
#ifdef WITH_TRACY
	const bool mark = tracyCtx_.BeforeLock();
#endif

	pthread_mutex_lock(&mutex_);

#ifdef WITH_TRACY
	if (mark)
		tracyCtx_.AfterLock();
#endif
}

void Mutex::unlock()
{
	pthread_mutex_unlock(&mutex_);

#ifdef WITH_TRACY
	tracyCtx_.AfterUnlock();
#endif
}

bool Mutex::tryLock()
{
	const bool acquired = (pthread_mutex_trylock(&mutex_) == 0);

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
	pthread_cond_init(&cond_, nullptr);
}

CondVariable::~CondVariable()
{
	pthread_cond_destroy(&cond_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void CondVariable::wait(Mutex &mutex)
{
	pthread_cond_wait(&cond_, &(mutex.mutex_));
}

void CondVariable::signal()
{
	pthread_cond_signal(&cond_);
}

void CondVariable::broadcast()
{
	pthread_cond_broadcast(&cond_);
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
	sem_init(&sem_, 0, initialCount);
}

Semaphore::~Semaphore()
{
	sem_destroy(&sem_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Semaphore::wait()
{
	sem_wait(&sem_);
}

void Semaphore::signal()
{
	sem_post(&sem_);
}

bool Semaphore::tryWait()
{
	return (sem_trywait(&sem_) == 0);
}

void Semaphore::wait(unsigned int count)
{
	for (unsigned int i = 0; i < count; i++)
		sem_wait(&sem_);
}

void Semaphore::signal(unsigned int count)
{
	for (unsigned int i = 0; i < count; i++)
		sem_post(&sem_);
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

	#if !defined(__APPLE__)
UserSemaphore::UserSemaphore(int initialCount)
    : count_(initialCount)
{
}
	#else
UserSemaphore::UserSemaphore(int initialCount)
{
	sem_ = dispatch_semaphore_create(initialCount);
}

UserSemaphore::~UserSemaphore()
{
	dispatch_release(sem_);
}
	#endif

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

	#if !defined(__APPLE__)
namespace {
	inline int futexWait(int *uaddr, int val)
	{
		return syscall(SYS_futex, uaddr, FUTEX_WAIT, val, nullptr, nullptr, 0);
	}

	inline int futexWake(int *uaddr, int n)
	{
		return syscall(SYS_futex, uaddr, FUTEX_WAKE, n, nullptr, nullptr, 0);
	}
}

void UserSemaphore::wait()
{
	const int32_t c = count_.fetchSub(1, nctl::MemoryModel::ACQUIRE);
	if (c <= 0)
	{
		while (true)
		{
			const int32_t expected = count_.load(nctl::MemoryModel::RELAXED);
			if (expected >= 0)
				break;
			futexWait(reinterpret_cast<int *>(&count_), expected);
		}
	}
}

void UserSemaphore::signal()
{
	const int32_t c = count_.fetchAdd(1, nctl::MemoryModel::RELEASE);
	if (c < 0)
		futexWake(reinterpret_cast<int *>(&count_), 1);
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
		while (true)
		{
			const int32_t expected = count_.load(nctl::MemoryModel::RELAXED);
			if (expected >= 0)
				break;
			futexWait(reinterpret_cast<int *>(&count_), expected);
		}
	}
}

void UserSemaphore::signal(unsigned int count)
{
	const int32_t c = count_.fetchAdd(count, nctl::MemoryModel::RELEASE);
	if (c < 0)
		futexWake(reinterpret_cast<int *>(&count_), count);
}
	#else
void UserSemaphore::wait()
{
	dispatch_semaphore_wait(sem_, DISPATCH_TIME_FOREVER);
}

void UserSemaphore::signal()
{
	dispatch_semaphore_signal(sem_);
}

bool UserSemaphore::tryWait()
{
	return (dispatch_semaphore_wait(sem_, DISPATCH_TIME_NOW) == 0);
}

void UserSemaphore::wait(unsigned int count)
{
	for (unsigned int i = 0; i < count; i++)
		dispatch_semaphore_wait(sem_, DISPATCH_TIME_FOREVER);
}

void UserSemaphore::signal(unsigned int count)
{
	for (unsigned int i = 0; i < count; i++)
		dispatch_semaphore_signal(sem_);
}
	#endif

#endif

///////////////////////////////////////////////////////////
// RWLock CLASS
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RWLock::RWLock()
{
	pthread_rwlock_init(&rwlock_, nullptr);
}

RWLock::~RWLock()
{
	pthread_rwlock_destroy(&rwlock_);
}

///////////////////////////////////////////////////////////
// Barrier CLASS
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

#if !defined(__ANDROID__) && !defined(__APPLE__)

Barrier::Barrier(unsigned int count)
{
	pthread_barrier_init(&barrier_, nullptr, count);
}

Barrier::~Barrier()
{
	pthread_barrier_destroy(&barrier_);
}

#endif

}
