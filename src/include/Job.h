#ifndef CLASS_NCINE_JOB
#define CLASS_NCINE_JOB

#include <nctl/Atomic.h>
#include "IJobSystem.h"

namespace ncine {

/// A structure representing a single job in the multi-threaded job system
/// \note To avoid false sharing, the structure should occupy at least one cache line.
struct alignas(64) Job
{
	static constexpr uint32_t INDEX_BITS = 16;
	static constexpr uint32_t INDEX_MASK = (1u << INDEX_BITS) - 1;

	JobFunction function = nullptr;
	JobId parent = InvalidJobId;
	nctl::Atomic32 unfinishedJobs;
	char data[JobDataSize];
	nctl::Atomic32 continuationCount;
	JobId continuations[JobNumContinuations];
	uint16_t generation = 0;

	static inline uint16_t unpackIndex(JobId jobId) { return jobId & INDEX_MASK; }
	static inline uint16_t unpackGeneration(JobId id) { return id >> INDEX_BITS; }
	static inline JobId makeJobId(uint16_t index, uint16_t generation) { return (uint32_t(generation) << INDEX_BITS) | index; }
};

static_assert(sizeof(Job) % 64 == 0, "Job structure size should be a multiple of a cache line");
static_assert(sizeof(Job) == 64, "Job structure size should be exactly one cache line");

}

#endif
