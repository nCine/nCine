#include <cstdlib> // for exit()
#include <unistd.h> // for sysconf()
#include <sched.h> // for sched_yield()
#include "Thread.h"
#include "ServiceLocator.h"

#ifdef __APPLE__
	#include <mach/thread_act.h>
	#include <mach/thread_policy.h>
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#ifndef __ANDROID__

/// Clears the CPU set
void ThreadAffinityMask::zero()
{
#ifdef __APPLE__
	affinityTag_ = THREAD_AFFINITY_TAG_NULL;
#else
	CPU_ZERO(&cpuSet_);
#endif
}

/// Sets the specified CPU number to be included in the set
void ThreadAffinityMask::set(int cpuNum)
{
#ifdef __APPLE__
	affinityTag_ |= 1 << cpuNum;
#else
	CPU_SET(cpuNum, &cpuSet_);
#endif
}

/// Sets the specified CPU number to be excluded by the set
void ThreadAffinityMask::clear(int cpuNum)
{
#ifdef __APPLE__
	affinityTag_ &= ~(1 << cpuNum);
#else
	CPU_CLR(cpuNum, &cpuSet_);
#endif
}

/// Returns true if the specified CPU number belongs to the set
bool ThreadAffinityMask::isSet(int cpuNum)
{
#ifdef __APPLE__
	return ((affinityTag_ >> cpuNum) & 1) != 0;
#else
	return CPU_ISSET(cpuNum, &cpuSet_) != 0;
#endif
}

#endif

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// A default constructor for a class without the associated function
Thread::Thread()
	: tid_(0)
{

}

/// Creates a thread around a function and runs it
Thread::Thread(ThreadFunctionPtr startFunction, void *arg)
	: tid_(0)
{
	run(startFunction, arg);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Gets the number of processors in the machine
unsigned int Thread::numProcessors()
{
	unsigned int numProcs = 0;

	long int confRet = -1;
#if defined(_SC_NPROCESSORS_ONLN)
	confRet = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_SC_NPROC_ONLN)
	confRet = sysconf(_SC_NPROC_ONLN);
#endif

	if (confRet > 0)
	{
		numProcs = static_cast<unsigned int>(confRet);
	}

	return numProcs;
}

/// Spawns a new thread if the class hasn't one already associated
void Thread::run(ThreadFunctionPtr startFunction, void *arg)
{
	if (tid_ == 0)
	{
		threadInfo_.startFunction = startFunction;
		threadInfo_.threadArg = arg;
		if (int error = pthread_create(&tid_, NULL, wrapperFunction, &threadInfo_))
		{
			LOGE_X("Error in pthread_create(): %d", error);
			::exit(EXIT_FAILURE);
		}
	}
	else
	{
		LOGW_X("Thread %u is already running", tid_);
	}
}

/// Joins the thread
void* Thread::join()
{
	void* pRetVal = NULL;
	pthread_join(tid_, &pRetVal);
	return pRetVal;
}

/// Returns the calling thread id
long int Thread::self()
{
#if defined(__APPLE__)
	return reinterpret_cast<long int>(pthread_self());
#else
	return static_cast<long int>(pthread_self());
#endif
}

/// Terminates the calling thread
void Thread::exit(void *retVal)
{
	pthread_exit(retVal);
}

/// Yields the calling thread in favour of another one with the same priority
void Thread::yieldExecution()
{
	sched_yield();
}

#ifndef __ANDROID__
/// Asks the thread for termination
void Thread::cancel()
{
	pthread_cancel(tid_);
}

/// Gets the thread affinity mask
ThreadAffinityMask Thread::affinityMask() const
{
	ThreadAffinityMask affinityMask;

	if (tid_ != 0)
	{
#ifdef __APPLE__
		thread_affinity_policy_data_t threadAffinityPolicy;
		thread_port_t threadPort = pthread_mach_thread_np(tid_);
		mach_msg_type_number_t policyCount = THREAD_AFFINITY_POLICY_COUNT;
		boolean_t getDefault = FALSE;
		thread_policy_get(threadPort, THREAD_AFFINITY_POLICY, reinterpret_cast<thread_policy_t>(&threadAffinityPolicy), &policyCount, &getDefault);
		affinityMask.affinityTag_ = threadAffinityPolicy.affinity_tag;
#else
		pthread_getaffinity_np(tid_, sizeof(cpu_set_t), &affinityMask.cpuSet_);
#endif
	}
	else
	{
		LOGW("Cannot get the affinity for a thread that has not been created yet");
	}

	return affinityMask;
}

/// Sets the thread affinity mask
void Thread::setAffinityMask(ThreadAffinityMask affinityMask)
{
	if (tid_ != 0)
	{
#ifdef __APPLE__
		thread_affinity_policy_data_t threadAffinityPolicy = { affinityMask.affinityTag_ };
		thread_port_t threadPort = pthread_mach_thread_np(tid_);
		thread_policy_set(threadPort, THREAD_AFFINITY_POLICY, reinterpret_cast<thread_policy_t>(&threadAffinityPolicy), THREAD_AFFINITY_POLICY_COUNT);
#else
		pthread_setaffinity_np(tid_, sizeof(cpu_set_t), &affinityMask.cpuSet_);
#endif
	}
	else
	{
		LOGW("Cannot set the affinity mask for a not yet created thread");
	}
}
#endif

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// The wrapper start function for thread creation
void *Thread::wrapperFunction(void *arg)
{
	ThreadInfo* pThreadInfo = static_cast<ThreadInfo*>(arg);
	pThreadInfo->startFunction(pThreadInfo->threadArg);

	return NULL;
}

}
