#ifndef CLASS_NCINE_JOBSYSTEM
#define CLASS_NCINE_JOBSYSTEM

#include "IJobSystem.h"
#include "Job.h"
#include <nctl/StaticArray.h>

namespace ncine {

class JobQueue;

/// A single-threaded job system used for tests and as a fallback
class SerialJobSystem: public IJobSystem
{
  public:
	static const unsigned int NumJobs = 2048; // 11 bits for the JobId
	static_assert(NumJobs > 1 && (NumJobs & (NumJobs - 1)) == 0, "The number of jobs needs to be a power of two for the mask to work"); // TODO: Remove thos two lines (have it in `IJobSystem` only once)?

	/// Creates a serial job system
	SerialJobSystem();

	JobId createJob(JobFunction function, const void *data, unsigned int dataSize) override;
	JobId createJobAsChild(JobId parentId, JobFunction function, const void *data, unsigned int dataSize) override;

	bool addContinuation(JobId ancestor, JobId continuation) override;

	void run(JobId jobId) override;
	void wait(const JobId jobId) override;

	int32_t unfinishedJobs(JobId jobId) override;
	bool isValid(JobId jobId) override;

  private:
	unsigned int allocatedJobs_;
	nctl::StaticArray<Job, NumJobs> jobPool_;

	unsigned int bottom_;
	nctl::StaticArray<Job *, NumJobs> jobQueue_;

	/// Retrieves the next available job from the pool
	Job *retrieveJob();
	/// Pushes a job in the queue
	void pushJob(Job *job);
	/// Pops a job in the queue
	Job *popJob();

	/// Deleted copy constructor
	SerialJobSystem(const SerialJobSystem &) = delete;
	/// Deleted assignment operator
	SerialJobSystem &operator=(const SerialJobSystem &) = delete;
};

}

#endif
