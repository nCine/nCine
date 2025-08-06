#ifndef CLASS_NCINE_JOBPOOL
#define CLASS_NCINE_JOBPOOL

#include <nctl/Array.h>
#include <nctl/StaticArray.h>
#include "Job.h"
#include "ThreadSync.h"

namespace ncine {

class JobPool
{
  public:
	JobPool();

	/// Allocates a job from the pool, returns `InvalidJobId` if none available
	JobId allocateJob();

	/// Frees a job back to the pool
	void freeJob(JobId jobId);

	/// Accesses the `Job` structure pointer from `JobId`
	Job *retrieveJob(JobId jobId);

  private:
	/// The number of elements in the per-thread cache of free pool indices
	static const int ThreadCacheSize = 64;

	// The per-thread cache of free pool indices
	struct ThreadCache
	{
		uint16_t indices[ThreadCacheSize];
		int top = 0;
	};

	unsigned char numThreads_;

	/// The stack of free pool indices
	nctl::StaticArray<uint16_t, MaxNumJobs> freeList_;

	/// Per-thread caches of free pool indices
	nctl::Array<ThreadCache> threadCaches_;

	/// The contiguous pool of `Job` structures
	Job jobPool_[MaxNumJobs];

	/// The mutex protecting access to the global free list
	Mutex mutex_;

	/// Initializes the pool with as many caches as the specified number of threads
	void initialize(unsigned char numThreads);

	/// Deleted copy constructor
	JobPool(const JobPool &) = delete;
	/// Deleted assignment operator
	JobPool &operator=(const JobPool &) = delete;

	friend class JobSystem;
};

}

#endif
