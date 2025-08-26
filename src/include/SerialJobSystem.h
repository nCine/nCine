#ifndef CLASS_NCINE_SERIALJOBSYSTEM
#define CLASS_NCINE_SERIALJOBSYSTEM

#include "IJobSystem.h"
#include "Job.h"
#include <nctl/StaticArray.h>

namespace ncine {

class JobQueue;

/// A fallback single-threaded job system used for debug and tests
class SerialJobSystem : public IJobSystem
{
  public:
	SerialJobSystem();

	JobId createJob(JobFunction function, const void *data, unsigned int dataSize) override;
	JobId createJobAsChild(JobId parentId, JobFunction function, const void *data, unsigned int dataSize) override;

	bool addContinuation(JobId ancestorId, JobId continuationId) override;

	void submit(JobId jobId) override;
	void wait(const JobId jobId) override;

	int32_t unfinishedJobs(JobId jobId) override;
	int32_t continuationCount(JobId jobId) override;

  private:
	/// The stack of free pool indices
	nctl::StaticArray<uint16_t, MaxNumJobs> freeList_;

	/// The contiguous pool of `Job` structures
	Job jobPool_[MaxNumJobs];

	unsigned int top_;
	unsigned int bottom_;
	JobId jobQueue_[MaxNumJobs];

	/// Allocates a job from the pool, returns `InvalidJobId` if none available
	JobId allocateJob();
	/// Frees a job back to the pool
	void freeJob(JobId jobId);
	/// Accesses the `Job` structure pointer from `JobId`
	Job *retrieveJob(JobId jobId);

	/// Pushes a job in the queue
	void pushJob(JobId jobId);
	/// Pops a job from the queue
	JobId popJob();

	static void finish(JobId jobId, SerialJobSystem &jobSystem);
	static void execute(JobId jobId, SerialJobSystem &jobSystem);

	/// Deleted copy constructor
	SerialJobSystem(const SerialJobSystem &) = delete;
	/// Deleted assignment operator
	SerialJobSystem &operator=(const SerialJobSystem &) = delete;
};

}

#endif
