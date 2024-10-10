#ifndef CLASS_NCINE_JOB
#define CLASS_NCINE_JOB

#include <nctl/Atomic.h>
#include "IJobSystem.h"

namespace ncine {

/// A structure representing a single job in the multi-threaded job system
/// \note To avoid false sharing, the structure should occupy at least one cache line.
struct alignas(64) Job
{
	JobFunction function = nullptr;
	Job *parent = nullptr;
	nctl::Atomic32 unfinishedJobs;
	char data[JobDataSize];
	nctl::Atomic32 continuationCount;
	Job *continuations[JobNumContinuations];
};

static_assert(sizeof(Job) % 64 == 0, "Job structure size should be a multiple of a cache line");
static_assert(sizeof(Job) == 128, "Job structure size should be exactly two cache lines");

}

#endif
