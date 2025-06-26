#include <unistd.h> // for sysconf()
#include <sched.h> // for sched_yield()
#include <cstring>

#ifdef __APPLE__
	#include <mach/thread_act.h>
	#include <mach/thread_policy.h>
#endif

#include "common_macros.h"
#include "Thread.h"

#ifdef WITH_TRACY
	#include "common/TracySystem.hpp"
#endif

namespace ncine {

namespace {
	const unsigned int MaxThreadNameLength = 16;

	int priorityImpl(pthread_t pthid)
	{
		int priority = 0;

		if (pthid != 0)
		{
			int policy;
			struct sched_param param;
			pthread_getschedparam(pthid, &policy, &param);
			priority = param.sched_priority;
		}

		return priority;
	}

#ifndef __APPLE__
	bool setPriorityImpl(pthread_t pthid, int priority)
	{
		if (pthid != 0)
		{
			int policy;
			struct sched_param param;
			pthread_getschedparam(pthid, &policy, &param);
			param.sched_priority = priority;
			return (pthread_setschedparam(pthid, policy, &param) == 0);
		}
		return false;
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

bool ThreadAffinityMask::isSet(int cpuNum) const
{
	#ifdef __APPLE__
	return ((affinityTag_ >> cpuNum) & 1) != 0;
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
	thread_policy_get(threadPort, THREAD_AFFINITY_POLICY, reinterpret_cast<thread_policy_t>(&threadAffinityPolicy), &policyCount, &getDefault);
	affinityMask.affinityTag_ = threadAffinityPolicy.affinity_tag;
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

int ThisThread::priority()
{
	const pthread_t pthid = pthread_self();
	return priorityImpl(pthid);
}

bool ThisThread::setPriority(int priority)
{
	const pthread_t pthid = pthread_self();

#ifdef __APPLE__
	const thread_act_t machThread = pthread_mach_thread_np(pthid);

	thread_precedence_policy_data_t policy;
	policy.importance = priority;
	kern_return_t result = thread_policy_set(machThread, THREAD_PRECEDENCE_POLICY, reinterpret_cast<thread_policy_t>(&policy), THREAD_PRECEDENCE_POLICY_COUNT);
	return (result == KERN_SUCCESS);
#else
	return setPriorityImpl(pthid, priority);
#endif
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
		thread_policy_get(threadPort, THREAD_AFFINITY_POLICY, reinterpret_cast<thread_policy_t>(&threadAffinityPolicy), &policyCount, &getDefault);
		affinityMask.affinityTag_ = threadAffinityPolicy.affinity_tag;
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

// TODO: Check minimum and maximum values for POSIX: `sched_get_priority_min()` and `sched_get_priority_max()`
// TODO: Use only valid priority values on Windows: https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreadpriority
// TODO: Check priority values for macOS
int Thread::priority() const
{
	return priorityImpl(pthid_);
}

bool Thread::setPriority(int priority)
{
	return setPriorityImpl(pthid_, priority);
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

void Thread::yieldExecution()
{
	sched_yield();
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
