#ifndef CLASS_NCINE_JOBQUEUE
#define CLASS_NCINE_JOBQUEUE

#include <nctl/Atomic.h>
#include "Job.h"

namespace ncine {

/// The lock-free work stealing job queue
class JobQueue
{
  public:
	JobQueue();

	void push(Job *job);
	Job *pop(void);
	Job *steal(void);

	/// Retrieves the next available job from the pool
	Job *retrieveJob();

  private:
	// Aligned variables will be on separate cache lines to avoid false sharing
	alignas(64) nctl::Atomic32 top_;
	char pad0[64 - sizeof(nctl::Atomic32)];

	alignas(64) nctl::Atomic32 bottom_;
	char pad1[64 - sizeof(nctl::Atomic32)];

	uint32_t allocatedJobs_;

	Job *jobs_[MaxNumJobs];
	Job jobPool_[MaxNumJobs];

	/// Deleted copy constructor
	JobQueue(const JobQueue &) = delete;
	/// Deleted assignment operator
	JobQueue &operator=(const JobQueue &) = delete;
};

}

#endif
