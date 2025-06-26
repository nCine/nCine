#include <unistd.h> // for sysconf()
#include <sched.h> // for sched_yield()
#include <cstring>

#ifdef __APPLE__
	#include <mach/thread_act.h>
#else
	#include <sys/resource.h>
#endif

#include "common_macros.h"
#include "Thread.h"

#ifdef WITH_TRACY
	#include "common/TracySystem.hpp"
#endif

namespace ncine {

namespace {
	const unsigned int MaxThreadNameLength = 16;

#ifndef __APPLE__
	Thread::Priority priorityImpl(unsigned long int tid)
	{
		const int niceVal = getpriority(PRIO_PROCESS, tid);
		if (niceVal <= -15)
			return Thread::Priority::Highest;
		if (niceVal <= -5)
			return Thread::Priority::High;
		if (niceVal <= 5)
			return Thread::Priority::Normal;
		if (niceVal <= 15)
			return Thread::Priority::Low;
		return Thread::Priority::Lowest;
	}

	bool setPriorityImpl(unsigned long int tid, Thread::Priority priority)
	{
		int niceVal = 0;
		switch (priority)
		{
			case Thread::Priority::Lowest:
				niceVal = 19;
				break;
			case Thread::Priority::Low:
				niceVal = 10;
				break;
			default:
			case Thread::Priority::Normal:
				niceVal = 0;
				break;
			case Thread::Priority::High:
				niceVal = -10;
				break;
			case Thread::Priority::Highest:
				niceVal = -20;
				break;
		}

		const int retValue = setpriority(PRIO_PROCESS, tid, niceVal);
		return (retValue == 0);
	}
#else
	Thread::Priority priorityImpl(pthread_t pthid)
	{
		const thread_port_t machThread = pthread_mach_thread_np(pthid);
		thread_precedence_policy_data_t policy;
		mach_msg_type_number_t count = THREAD_PRECEDENCE_POLICY_COUNT;
		boolean_t getDefault = FALSE;

		kern_return_t kr = thread_policy_get(machThread, THREAD_PRECEDENCE_POLICY, (thread_policy_t)&policy, &count, &getDefault);

		if (kr != KERN_SUCCESS)
			return Thread::Priority::Normal;

		if (policy.importance <= -7)
			return Thread::Priority::Lowest;
		else if (policy.importance <= -2)
			return Thread::Priority::Low;
		else if (policy.importance < 2)
			return Thread::Priority::Normal;
		else if (policy.importance < 7)
			return Thread::Priority::High;
		else
			return Thread::Priority::Highest;
	}

	bool setPriorityImpl(pthread_t pthid, Thread::Priority priority)
	{
		const thread_port_t machThread = pthread_mach_thread_np(pthid);
		thread_precedence_policy_data_t policy;

		switch (priority)
		{
			case Thread::Priority::Lowest:
				policy.importance = -10;
				break;
			case Thread::Priority::Low:
				policy.importance = -5;
				break;
			default:
			case Thread::Priority::Normal:
				policy.importance = 0;
				break;
			case Thread::Priority::High:
				policy.importance = 5;
				break;
			case Thread::Priority::Highest:
				policy.importance = 10;
				break;
		}

		const kern_return_t kr = thread_policy_set(machThread, THREAD_PRECEDENCE_POLICY, (thread_policy_t)&policy, THREAD_PRECEDENCE_POLICY_COUNT);
		return (kr == KERN_SUCCESS);
	}
#endif
}

///////////////////////////////////////////////////////////
// `ThreadAffinityMask`
///////////////////////////////////////////////////////////

#if !defined(__EMSCRIPTEN__)

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
	affinityTag_ = cpuNum + 1; // a non-zero value is a valid tag
	#else
	CPU_SET(cpuNum, &cpuSet_);
	#endif
}

void ThreadAffinityMask::clear(int cpuNum)
{
	#ifdef __APPLE__
	if (affinityTag_ == cpuNum + 1)
		affinityTag_ = THREAD_AFFINITY_TAG_NULL;
	#else
	CPU_CLR(cpuNum, &cpuSet_);
	#endif
}

bool ThreadAffinityMask::isSet(int cpuNum) const
{
	#ifdef __APPLE__
	return (affinityTag_ == cpuNum + 1);
	#else
	return CPU_ISSET(cpuNum, &cpuSet_) != 0;
	#endif
}

#endif

///////////////////////////////////////////////////////////
// `ThisThread`
///////////////////////////////////////////////////////////

unsigned long int ThisThread::threadId()
{
#if defined(__APPLE__)
	uint64_t tid = 0;
	pthread_threadid_np(nullptr, &tid);
	return tid;
#else
	return static_cast<unsigned long int>(::gettid());
#endif
}

#ifndef __EMSCRIPTEN__
void ThisThread::setName(const char *name)
{
	#ifdef WITH_TRACY
	tracy::SetThreadName(name);
	#else
	char buffer[MaxThreadNameLength];
	strncpy(buffer, name, MaxThreadNameLength - 1);
	buffer[MaxThreadNameLength - 1] = '\0';
		#ifdef __APPLE__
	pthread_setname_np(buffer);
		#else
	pthread_setname_np(pthread_self(), buffer);
		#endif
	#endif
}

ThreadAffinityMask ThisThread::affinityMask()
{
	ThreadAffinityMask affinityMask;

	#ifdef __APPLE__
	thread_affinity_policy_data_t threadAffinityPolicy;
	const thread_port_t threadPort = pthread_mach_thread_np(pthread_self());
	mach_msg_type_number_t policyCount = THREAD_AFFINITY_POLICY_COUNT;
	boolean_t getDefault = FALSE;

	if (thread_policy_get(threadPort, THREAD_AFFINITY_POLICY, reinterpret_cast<thread_policy_t>(&threadAffinityPolicy), &policyCount, &getDefault) == KERN_SUCCESS)
		affinityMask.affinityTag_ = threadAffinityPolicy.affinity_tag;
	else
		affinityMask.affinityTag_ = 0;
	#else
	sched_getaffinity(gettid(), sizeof(cpu_set_t), &affinityMask.cpuSet_);
	#endif

	return affinityMask;
}

void ThisThread::setAffinityMask(ThreadAffinityMask affinityMask)
{
	#ifdef __APPLE__
	thread_affinity_policy_data_t threadAffinityPolicy = { affinityMask.affinityTag_ };
	const thread_port_t threadPort = pthread_mach_thread_np(pthread_self());
	thread_policy_set(threadPort, THREAD_AFFINITY_POLICY, reinterpret_cast<thread_policy_t>(&threadAffinityPolicy), THREAD_AFFINITY_POLICY_COUNT);
	#else
	sched_setaffinity(gettid(), sizeof(cpu_set_t), &affinityMask.cpuSet_);
	#endif
}
#endif

Thread::Priority ThisThread::priority()
{
#ifdef __APPLE__
	const pthread_t pthid = pthread_self();
	return priorityImpl(pthid);
#else
	return priorityImpl(0);
#endif
}

bool ThisThread::setPriority(Thread::Priority priority)
{
#ifdef __APPLE__
	const pthread_t pthid = pthread_self();
	return setPriorityImpl(pthid, priority);
#else
	return setPriorityImpl(0, priority);
#endif
}

void ThisThread::yield()
{
	sched_yield();
}

[[noreturn]] void ThisThread::exit()
{
	pthread_exit(nullptr);
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Thread::Thread()
    : tid_(0), pthid_(0), threadInfo_(nctl::makeUnique<ThreadInfo>())
{
}

Thread::Thread(ThreadFunctionPtr threadFunction, void *threadArg)
    : Thread()
{
	run(threadFunction, threadArg);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Thread::run(ThreadFunctionPtr threadFunction, void *threadArg)
{
	if (pthid_ == 0)
	{
		threadInfo_->tid = &tid_;
		threadInfo_->threadFunction = threadFunction;
		threadInfo_->threadArg = threadArg;
		const int retValue = pthread_create(&pthid_, nullptr, wrapperFunction, threadInfo_.get());
		FATAL_ASSERT_MSG_X(!retValue, "Error in pthread_create(): %d", retValue);
	}
	else
		LOGW_X("Thread %u is already running", pthid_);
}

bool Thread::join()
{
	bool success = false;
	if (pthid_ != 0)
	{
		const int retValue = pthread_join(pthid_, nullptr);
		success = (retValue == 0);
		pthid_ = 0;
	}
	return success;
}

bool Thread::detach()
{
	bool success = false;
	if (pthid_ != 0)
	{
		const int retValue = pthread_detach(pthid_);
		success = (retValue == 0);
		pthid_ = 0;
	}
	return success;
}

#ifndef __EMSCRIPTEN__
	#ifndef __APPLE__
void Thread::setName(const char *name)
{
	if (pthid_ != 0)
	{
		char buffer[MaxThreadNameLength];
		strncpy(buffer, name, MaxThreadNameLength - 1);
		buffer[MaxThreadNameLength - 1] = '\0';
		pthread_setname_np(pthid_, buffer);
	}
	else
		LOGW("Cannot set the name for an invalid thread id");
}
	#endif

ThreadAffinityMask Thread::affinityMask() const
{
	ThreadAffinityMask affinityMask;

	if (pthid_ != 0)
	{
	#ifdef __APPLE__
		thread_affinity_policy_data_t threadAffinityPolicy;
		const thread_port_t threadPort = pthread_mach_thread_np(pthid_);
		mach_msg_type_number_t policyCount = THREAD_AFFINITY_POLICY_COUNT;
		boolean_t getDefault = FALSE;

		if (thread_policy_get(threadPort, THREAD_AFFINITY_POLICY, reinterpret_cast<thread_policy_t>(&threadAffinityPolicy), &policyCount, &getDefault) == KERN_SUCCESS)
			affinityMask.affinityTag_ = threadAffinityPolicy.affinity_tag;
		else
			affinityMask.affinityTag_ = 0;
	#else
		sched_getaffinity(tid_, sizeof(cpu_set_t), &affinityMask.cpuSet_);
	#endif
	}
	else
		LOGW("Cannot get the affinity mask for an invalid thread id");

	return affinityMask;
}

void Thread::setAffinityMask(ThreadAffinityMask affinityMask)
{
	if (pthid_ != 0)
	{
	#ifdef __APPLE__
		thread_affinity_policy_data_t threadAffinityPolicy = { affinityMask.affinityTag_ };
		const thread_port_t threadPort = pthread_mach_thread_np(pthid_);
		thread_policy_set(threadPort, THREAD_AFFINITY_POLICY, reinterpret_cast<thread_policy_t>(&threadAffinityPolicy), THREAD_AFFINITY_POLICY_COUNT);
	#else
		sched_setaffinity(tid_, sizeof(cpu_set_t), &affinityMask.cpuSet_);
	#endif
	}
	else
		LOGW("Cannot set the affinity mask for an invalid thread id");
}
#endif

Thread::Priority Thread::priority() const
{
#ifndef __APPLE__
	return priorityImpl(tid_);
#else
	return priorityImpl(pthid_);
#endif
}

bool Thread::setPriority(Thread::Priority priority)
{
#ifndef __APPLE__
	return setPriorityImpl(tid_, priority);
#else
	return setPriorityImpl(pthid_, priority);
#endif
}

#ifndef __ANDROID__
bool Thread::cancel()
{
	bool success = false;
	if (pthid_ != 0)
	{
		const int retValue = pthread_cancel(pthid_);
		success = (retValue == 0);
		pthid_ = 0;
	}
	return success;
}
#endif

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

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void *Thread::wrapperFunction(void *arg)
{
	const ThreadInfo *threadInfo = static_cast<ThreadInfo *>(arg);
	*threadInfo->tid = ThisThread::threadId();
	threadInfo->threadFunction(threadInfo->threadArg);

	return nullptr;
}

}
