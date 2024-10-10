#ifndef CLASS_NCINE_IJOBSYSTEM
#define CLASS_NCINE_IJOBSYSTEM

#include <cstdint>
#include "common_defines.h"

namespace ncine {

static const unsigned int MaxNumJobs = 8192;

using JobId = uintptr_t;
static const JobId InvalidJobId = reinterpret_cast<uintptr_t>(nullptr);
using JobFunction = void (*)(JobId, const void *);
static const uint32_t JobDataSize = 40;
static const uint32_t JobNumContinuations = 8;
static_assert(JobDataSize >= sizeof(uintptr_t), "At least one user pointer should fit the Job structure");

/// The interface for the multi-threaded job system
class DLL_PUBLIC IJobSystem
{
  public:
	IJobSystem()
	    : numThreads_(0) {}

	IJobSystem(unsigned char numThreads)
	    : numThreads_(numThreads) {}

	virtual ~IJobSystem() = 0;

	/// Creates an id for a new job, with optional custom data
	virtual JobId createJob(JobFunction function, const void *data, unsigned int dataSize) = 0;
	/// Creates an id for a new job as a child of the specified one, with optional custom data
	virtual JobId createJobAsChild(JobId parentId, JobFunction function, const void *data, unsigned int dataSize) = 0;

	/// Creates an id for a new job
	inline JobId createJob(JobFunction function) { return createJob(function, nullptr, 0); }
	/// Creates an id for a new job as a child of the specified one
	inline JobId createJobAsChild(JobId parentId, JobFunction function) { return createJobAsChild(parentId, function, nullptr, 0); }

	/// Adds a previously created job as a continuation for the specified ancestor one
	virtual bool addContinuation(JobId ancestorId, JobId continuationId) = 0;

	/// Submits the specified job to the calling thread queue
	virtual void submit(JobId jobId) = 0;
	/// Waits until the specified job has finished, while carrying on other jobs
	virtual void wait(JobId jobId) = 0;

	/// Returns the number of unfinished jobs for the specified job
	virtual int32_t unfinishedJobs(JobId jobId) = 0;
	/// Returns the number of continuation jobs for the specified job
	virtual int32_t continuationCount(JobId jobId) = 0;

	/// Returns the total number of threads executing jobs (workers + main)
	inline unsigned char numThreads() const { return numThreads_; }

	/// Returns the thread index of the calling thread
	static unsigned char threadIndex();
	/// Returns the thread index of the main thread
	static unsigned char mainThreadIndex();
	/// Returns `true` if called from the main thread
	static bool isMainThread();

  protected:
	/// Total number of threads executing jobs (workers + main)
	unsigned char numThreads_;

	/// Sets the thread index for the calling thread
	/*! \note Only to be used by a job system implementation during its initialization. */
	static void setThreadIndex(unsigned char threadIndex);

  private:
	/// Deleted copy constructor
	IJobSystem(const IJobSystem &) = delete;
	/// Deleted assignment operator
	IJobSystem &operator=(const IJobSystem &) = delete;
};

inline IJobSystem::~IJobSystem() {}

/// A fake job system class that does nothing
class DLL_PUBLIC NullJobSystem : public IJobSystem
{
  public:
	inline JobId createJob(JobFunction function, const void *data, unsigned int dataSize) override { return InvalidJobId; }
	inline JobId createJobAsChild(JobId parentId, JobFunction function, const void *data, unsigned int dataSize) override { return InvalidJobId; }

	inline bool addContinuation(JobId ancestorId, JobId continuationId) override { return false; }

	inline void submit(JobId jobId) override {}
	inline void wait(JobId jobId) override {}

	inline int32_t unfinishedJobs(JobId jobId) override { return 0; }
	inline int32_t continuationCount(JobId jobId) override { return 0; }
};

}

#endif
