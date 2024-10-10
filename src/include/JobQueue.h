#ifndef CLASS_NCINE_JOBQUEUE
#define CLASS_NCINE_JOBQUEUE

#include <nctl/Atomic.h>
#include "Job.h"

namespace ncine {

/// The lock-free work stealing job queue
class JobQueue
{
  public:
	static const unsigned int NumJobs = 2048; // 11 bits for the JobId
	static_assert(NumJobs > 1 && (NumJobs & (NumJobs - 1)) == 0, "The number of jobs needs to be a power of two for the mask to work");

	JobQueue();

	void push(Job *job);
	Job *pop(void);
	Job *steal(void);

#if PACKED_JOBID
	/// Retrieves the next available job from the pool and returns its pool index
	Job *retrieveJob(unsigned int &poolIndex);
#else
	/// Retrieves the next available job from the pool
	Job *retrieveJob();
#endif

  private:
	nctl::Atomic32 top_;
	nctl::Atomic32 bottom_;
	uint32_t allocatedJobs_;

	Job *jobs_[NumJobs];
	Job jobPool_[NumJobs];

	/// Deleted copy constructor
	JobQueue(const JobQueue &) = delete;
	/// Deleted assignment operator
	JobQueue &operator=(const JobQueue &) = delete;
};

}

#endif
