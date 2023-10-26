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
}

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
    : tid_(0), threadInfo_(nctl::makeUnique<ThreadInfo>())
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

void Thread::run(ThreadFunctionPtr threadFunction, void *threadArg)
{
	if (tid_ == 0)
	{
		threadInfo_->threadFunction = threadFunction;
		threadInfo_->threadArg = threadArg;
		const int retValue = pthread_create(&tid_, nullptr, wrapperFunction, threadInfo_.get());
		FATAL_ASSERT_MSG_X(!retValue, "Error in pthread_create(): %d", retValue);
	}
	else
		LOGW_X("Thread %u is already running", tid_);
}

bool Thread::join()
{
	bool success = false;
	if (tid_ != 0)
	{
		const int retValue = pthread_join(tid_, nullptr);
		success = (retValue == 0);
		tid_ = 0;
	}
	return success;
}

bool Thread::detach()
{
	bool success = false;
	if (tid_ != 0)
	{
		const int retValue = pthread_detach(tid_);
		success = (retValue == 0);
		tid_ = 0;
	}
	return success;
}

#ifndef __EMSCRIPTEN__
	#ifndef __APPLE__
void Thread::setName(const char *name)
{
	if (tid_ == 0)
		return;

	const auto nameLength = strnlen(name, MaxThreadNameLength);
	if (nameLength <= MaxThreadNameLength - 1)
		pthread_setname_np(tid_, name);
	else
	{
		char buffer[MaxThreadNameLength];
		memcpy(buffer, name, MaxThreadNameLength - 1);
		buffer[MaxThreadNameLength - 1] = '\0';
		pthread_setname_np(tid_, name);
	}
}
	#endif

void Thread::setSelfName(const char *name)
{
	#ifdef WITH_TRACY
	tracy::SetThreadName(name);
	#else
	const auto nameLength = strnlen(name, MaxThreadNameLength);
	if (nameLength <= MaxThreadNameLength - 1)
	{
		#ifndef __APPLE__
		pthread_setname_np(pthread_self(), name);
		#else
		pthread_setname_np(name);
		#endif
	}
	else
	{
		char buffer[MaxThreadNameLength];
		memcpy(buffer, name, MaxThreadNameLength - 1);
		buffer[MaxThreadNameLength - 1] = '\0';
		#ifndef __APPLE__
		pthread_setname_np(pthread_self(), name);
		#else
		pthread_setname_np(name);
		#endif
	}
	#endif
}
#endif

int Thread::priority() const
{
	if (tid_ == 0)
		return 0;

	int policy;
	struct sched_param param;
	pthread_getschedparam(tid_, &policy, &param);
	return param.sched_priority;
}

void Thread::setPriority(int priority)
{
	if (tid_ != 0)
	{
		int policy;
		struct sched_param param;
		pthread_getschedparam(tid_, &policy, &param);

		param.sched_priority = priority;
		pthread_setschedparam(tid_, policy, &param);
	}
}

long int Thread::self()
{
#if defined(__APPLE__)
	return reinterpret_cast<long int>(pthread_self());
#else
	return static_cast<long int>(pthread_self());
#endif
}

[[noreturn]] void Thread::exit()
{
	pthread_exit(nullptr);
}

void Thread::yieldExecution()
{
	sched_yield();
}

#ifndef __ANDROID__
bool Thread::cancel()
{
	bool success = false;
	if (tid_ != 0)
	{
		const int retValue = pthread_cancel(tid_);
		success = (retValue == 0);
		tid_ = 0;
	}
	return success;
}

	#ifndef __EMSCRIPTEN__
ThreadAffinityMask Thread::affinityMask() const
{
	ThreadAffinityMask affinityMask;

	if (tid_ != 0)
	{
		#ifdef __APPLE__
		thread_affinity_policy_data_t threadAffinityPolicy;
		const thread_port_t threadPort = pthread_mach_thread_np(tid_);
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
		const thread_port_t threadPort = pthread_mach_thread_np(tid_);
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
	pThreadInfo->threadFunction(pThreadInfo->threadArg);

	return nullptr;
}

}
