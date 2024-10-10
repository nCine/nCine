#ifndef CLASS_NCINE_IJOBSYSTEM
#define CLASS_NCINE_IJOBSYSTEM

// If set to 1, the `JobId` will be a 16bit integer that encodes the thread id (5 bits) and the pool index (11 bits)
// A packed id will limit the number of threads to 32, and the pool size to 2048.
// Set it to 0 to use a 64bit pointer and overcome those limits (there will be less space in the `Job` struct for remaining fields).
#define PACKED_JOBID (0)

#include <cstdint>
#include "common_defines.h"

// TODO: Create a Lua API for this?

namespace ncine {

#if PACKED_JOBID
using JobId = uint16_t;
static const unsigned int NumJobs = 2048; // 11 bits for the JobId
static_assert(NumJobs > 1 && (NumJobs & (NumJobs - 1)) == 0, "The number of jobs needs to be a power of two for the mask to work");
#else
using JobId = uintptr_t;
static const uint32_t JobDataSize = 52;
static const uint32_t JobNumContinuations = 6;
static const JobId NullJobId = reinterpret_cast<uintptr_t>(nullptr);
#endif
using JobFunction = void (*)(JobId, const void*);

/// The interface for the multi-threaded job system
class DLL_PUBLIC IJobSystem
{
  public:
	IJobSystem()
	    : numThreads_(1) {}

	IJobSystem(unsigned char numThreads)
	    : numThreads_(numThreads) {}

	virtual ~IJobSystem() = 0;

	/// Creates a new job (with custom data), and add it to the system
	virtual JobId createJob(JobFunction function, const void *data, unsigned int dataSize) = 0;
	/// Creates a new job (with custom data) as a child of the specified one, and add it to the system
	virtual JobId createJobAsChild(JobId parentId, JobFunction function, const void *data, unsigned int dataSize) = 0;

	inline JobId createJob(JobFunction function) { return createJob(function, nullptr, 0); }
	inline JobId createJobAsChild(JobId parentId, JobFunction function) { return createJobAsChild(parentId, function, nullptr, 0 ); }

	/// Adds a previously created job as a continuation for the specified ancestor one
	virtual bool addContinuation(JobId ancestor, JobId continuation) = 0;

	/// Pushes the specified job in the main thread queue and notifies all threads
	virtual void run(JobId jobId) = 0;
	/// Waits until the specified job has finished, while carrying on other jobs
	virtual void wait(const JobId jobId) = 0;

	/// Returns the number of unfinished jobs for the specified job
	virtual int32_t unfinishedJobs(JobId jobId) = 0;
	/// Returns true if a job id refers to a valid job
	virtual bool isValid(JobId jobId) = 0;

	/// Returns the total number of threads executing jobs (workers + main)
	inline unsigned char numThreads() const { return numThreads_; }

	/// Returns the thread index of the calling thread
	static unsigned char threadIndex();

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

}

#endif
