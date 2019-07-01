#include <unistd.h> // for sysconf()
#include <sched.h> // for sched_yield()

#ifdef __APPLE__
	#include <mach/thread_act.h>
	#include <mach/thread_policy.h>
#endif

#include "common_macros.h"
#include "Thread.h"

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#if !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)

void ThreadAffinityMask::zero()
{
	#ifdef __APPLE__
	affinityTag_ = THREAD_AFFINITY_TAG_NULL;
	#else
	CPU_ZERO(&cpuSet_);
	#endif
}

void ThreadAffinityMask::set(int cpuNum)
{
	#ifdef __APPLE__
	affinityTag_ |= 1 << cpuNum;
	#else
	CPU_SET(cpuNum, &cpuSet_);
	#endif
}

void ThreadAffinityMask::clear(int cpuNum)
{
	#ifdef __APPLE__
	affinityTag_ &= ~(1 << cpuNum);
	#else
	CPU_CLR(cpuNum, &cpuSet_);
	#endif
}

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

Thread::Thread()
    : tid_(0)
{
}

Thread::Thread(ThreadFunctionPtr startFunction, void *arg)
    : tid_(0)
{
	run(startFunction, arg);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

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
		numProcs = static_cast<unsigned int>(confRet);

	return numProcs;
}

void Thread::run(ThreadFunctionPtr startFunction, void *arg)
{
	if (tid_ == 0)
	{
		threadInfo_.startFunction = startFunction;
		threadInfo_.threadArg = arg;
		const int error = pthread_create(&tid_, nullptr, wrapperFunction, &threadInfo_);
		FATAL_ASSERT_MSG_X(!error, "Error in pthread_create(): %d", error);
	}
	else
		LOGW_X("Thread %u is already running", tid_);
}

void *Thread::join()
{
	void *pRetVal = nullptr;
	pthread_join(tid_, &pRetVal);
	return pRetVal;
}

long int Thread::self()
{
#if defined(__APPLE__)
	return reinterpret_cast<long int>(pthread_self());
#else
	return static_cast<long int>(pthread_self());
#endif
}

void Thread::exit(void *retVal)
{
	pthread_exit(retVal);
}

void Thread::yieldExecution()
{
	sched_yield();
}

#ifndef __ANDROID__
void Thread::cancel()
{
	pthread_cancel(tid_);
}

	#ifndef __EMSCRIPTEN__
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
		LOGW("Cannot get the affinity for a thread that has not been created yet");

	return affinityMask;
}

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
		LOGW("Cannot set the affinity mask for a not yet created thread");
}
	#endif
#endif

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void *Thread::wrapperFunction(void *arg)
{
	const ThreadInfo *pThreadInfo = static_cast<ThreadInfo *>(arg);
	pThreadInfo->startFunction(pThreadInfo->threadArg);

	return nullptr;
}

}
