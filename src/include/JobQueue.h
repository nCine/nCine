#ifndef CLASS_NCINE_JOBQUEUE
#define CLASS_NCINE_JOBQUEUE

#include <nctl/Atomic.h>
#include "IJobSystem.h"
#include "jobsystem_debug.h"

namespace ncine {

/// The lock-free work stealing job queue
class JobQueue
{
  public:
	JobQueue();

	void push(JobId jobId);
	JobId pop(void);
	JobId steal(void);

  private:
	// Aligned variables will be on separate cache lines to avoid false sharing
	alignas(64) nctl::Atomic32 top_;
	char pad0[64 - sizeof(nctl::Atomic32)];

	alignas(64) nctl::Atomic32 bottom_;
	char pad1[64 - sizeof(nctl::Atomic32)];

	JobId jobs_[MaxNumJobs];

#if JOB_DEBUG_STATE
	void setJobPool(JobPool *jobPool);
	friend class JobSystem;
#endif

	/// Deleted copy constructor
	JobQueue(const JobQueue &) = delete;
	/// Deleted assignment operator
	JobQueue &operator=(const JobQueue &) = delete;
};

}

#endif
