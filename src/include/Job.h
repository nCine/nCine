#ifndef CLASS_NCINE_JOB
#define CLASS_NCINE_JOB

#include <nctl/Atomic.h>
#include "IJobSystem.h"
#include "jobsystem_debug.h"

namespace ncine {

/// A structure representing a single job in the multi-threaded job system
/// \note To avoid false sharing, the structure should occupy at least one cache line.
struct alignas(64) Job
{
	static constexpr uint32_t INDEX_BITS = 16;
	static constexpr uint32_t INDEX_MASK = (1u << INDEX_BITS) - 1;

	static constexpr uint32_t UNFINISHED_BITS = 16;
	static constexpr uint32_t CONTINUATION_BITS = 13;
	static constexpr uint32_t FLAGS_BITS  = 3;
	static_assert(UNFINISHED_BITS + CONTINUATION_BITS + FLAGS_BITS == 32, "The counters and state flags should fit a 32-bit atomic");

	static constexpr uint32_t CONTINUATION_SHIFT = UNFINISHED_BITS;
	static constexpr uint32_t FLAGS_SHIFT = UNFINISHED_BITS + CONTINUATION_BITS;

	static constexpr uint32_t UNFINISHED_MASK = (1u << UNFINISHED_BITS) - 1u;
	static constexpr uint32_t CONTINUATION_MASK = ((1u << CONTINUATION_BITS) - 1u) << CONTINUATION_SHIFT;
	static constexpr uint32_t FLAGS_MASK = ((1u << FLAGS_BITS) - 1u) << FLAGS_SHIFT;

#if JOB_DEBUG_STATE
	enum class State : uint32_t
	{
		FREE = 0,
		PUSHED = 1,
		POPPED = 2,
		EXECUTING = 3,
		FINISHED = 4,
	};
	nctl::AtomicU32 state; // automatically initialized with 0, `State::FREE`
#endif

	enum Flags : uint32_t
	{
		SUBMITTED = 1u << 29,
		RUNNING = 1u << 30,
		CANCELLED = 1u << 31
	};

	JobFunction function = nullptr;
	JobId parent = InvalidJobId;
	nctl::AtomicU32 countersAndState;
	char data[JobDataSize];
	JobId continuations[JobNumContinuations];
	uint16_t generation = 0;

	/// Atomically loads the counter variable and unpacks the unfinished jobs part
	inline uint16_t loadUnfinishedJobs(nctl::MemoryModel memoryModel);
	/// Atomically increments unfinished jobs and returns previous value
	inline uint16_t incrementUnfinishedJobs();
	/// Atomically decrements unfinished jobs and returns previous value
	inline uint16_t decrementUnfinishedJobs();

	/// Atomically loads the counter variable and unpacks the ucontinuation count part
	inline uint16_t loadContinuationCount(nctl::MemoryModel memoryModel);
	/// Atomically increments continuation count and returns previous value
	inline uint16_t incrementContinuationCount();
	/// Atomically decrements continuation count and returns previous value
	inline uint16_t decrementContinuationCount();

	static inline bool hasFlag(const uint32_t value, Flags flags) { return ((value & flags) != 0); }
	inline uint32_t setFlag(Flags flags);
	inline uint32_t clearFlag(Flags flags);

	static inline uint16_t unpackIndex(JobId jobId) { return jobId & INDEX_MASK; }
	static inline uint16_t unpackGeneration(JobId id) { return id >> INDEX_BITS; }
	static inline JobId makeJobId(uint16_t index, uint16_t generation) { return (uint32_t(generation) << INDEX_BITS) | index; }

	static inline uint16_t unpackUnfinishedJobs(uint32_t val) { return static_cast<uint16_t>(val & UNFINISHED_MASK); }
	static inline uint16_t unpackContinuationCount(uint32_t val) { return static_cast<uint16_t>((val & CONTINUATION_MASK) >> CONTINUATION_SHIFT);	}
};

uint16_t Job::loadUnfinishedJobs(nctl::MemoryModel memoryModel)
{
	return unpackUnfinishedJobs(countersAndState.load(memoryModel));
}

uint16_t Job::incrementUnfinishedJobs()
{
	const uint32_t oldValue = countersAndState.fetchAdd(1);
	return unpackUnfinishedJobs(oldValue);
}

uint16_t Job::decrementUnfinishedJobs()
{
	const uint32_t oldValue = countersAndState.fetchSub(1);
	return unpackUnfinishedJobs(oldValue);
}

uint16_t Job::loadContinuationCount(nctl::MemoryModel memoryModel)
{
	return unpackContinuationCount(countersAndState.load(memoryModel));
}

uint16_t Job::incrementContinuationCount()
{
	const uint32_t add = 1u << CONTINUATION_SHIFT;
	const uint32_t oldValue = countersAndState.fetchAdd(add);
	return unpackContinuationCount(oldValue);
}

uint16_t Job::decrementContinuationCount()
{
	const uint32_t sub = 1u << CONTINUATION_SHIFT;
	const uint32_t oldValue = countersAndState.fetchSub(sub);
	return unpackContinuationCount(oldValue);
}

uint32_t Job::setFlag(Flags flags)
{
	return (countersAndState.fetchOr(flags, nctl::MemoryModel::RELEASE) & FLAGS_MASK);
}

uint32_t Job::clearFlag(Flags flags)
{
	return (countersAndState.fetchAnd(~flags, nctl::MemoryModel::RELEASE) & FLAGS_MASK);
}

static_assert(JobNumContinuations < (1u << Job::CONTINUATION_BITS), "JobNumContinuations doesn't fit in continuation bits");
static_assert(sizeof(Job) % 64 == 0, "Job structure size should be a multiple of a cache line");
static_assert(sizeof(Job) == 64, "Job structure size should be exactly one cache line");

}

#endif
