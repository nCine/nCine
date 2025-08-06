#ifndef CLASS_NCINE_JOBSYSTEM
#define CLASS_NCINE_JOBSYSTEM

#include "IJobSystem.h"
#include "Thread.h"
#include "ThreadSync.h"
#include "JobPool.h"
#include <nctl/Array.h>

namespace ncine {

class JobQueue;

/// The multi-threaded job system
class JobSystem : public IJobSystem
{
  public:
	/// Creates a job system with as many threads as available processors for its pool
	JobSystem();
	/// Creates a job system with the specified number of threads for its pool
	explicit JobSystem(unsigned char numThreads);
	~JobSystem() override;

	JobId createJob(JobFunction function, const void *data, unsigned int dataSize) override;
	JobId createJobAsChild(JobId parentId, JobFunction function, const void *data, unsigned int dataSize) override;

	bool addContinuation(JobId ancestorId, JobId continuationId) override;

	void submit(JobId jobId) override;
	void wait(JobId jobId) override;

	int32_t unfinishedJobs(JobId jobId) override;
	int32_t continuationCount(JobId jobId) override;

  private:
#if HAVE_USER_SEMAPHORE
	using SemType = UserSemaphore;
#else
	using SemType = Semaphore;
#endif

	struct CommonThreadDataStruct
	{
		CommonThreadDataStruct(JobPool &pool, SemType &sem)
		    : numThreads(0), jobPool(pool), jobQueues(nullptr), queueSem(sem) {}

		unsigned char numThreads;
		JobPool &jobPool;
		JobQueue *jobQueues;
		SemType &queueSem;
	};

	struct ThreadStruct
	{
		explicit ThreadStruct(const CommonThreadDataStruct &commonThreadData)
		    : threadIndex(0), shouldQuit(false), commonData(commonThreadData) {}

		unsigned char threadIndex;
		unsigned char shouldQuit;
		const CommonThreadDataStruct &commonData;
	};

	JobPool jobPool_;
	nctl::Array<JobQueue> jobQueues_;
	nctl::Array<Thread> threads_;
	nctl::Array<ThreadStruct> threadStructs_;
	SemType queueSem_;
	CommonThreadDataStruct commonData_;

	static void workerFunction(void *arg);

	/// Deleted copy constructor
	JobSystem(const JobSystem &) = delete;
	/// Deleted assignment operator
	JobSystem &operator=(const JobSystem &) = delete;
};

}

#endif
