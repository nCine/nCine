#ifndef CLASS_NCINE_JOBHANDLE
#define CLASS_NCINE_JOBHANDLE

#include <ncine/IJobSystem.h>
#include <ncine/ParallelForJob.h>

namespace ncine {

/// A wrapper class around the job system `JobId`
class DLL_PUBLIC JobHandle
{
  public:
	/// Creates a job handle for a new job, with an invalid job id
	JobHandle();
	/// Creates a job handle for an existing job with the specified job id
	explicit JobHandle(JobId jobId);
	/// Creates a job handle for a new job, with optional custom data
	JobHandle(JobFunction function, const void *data, unsigned int dataSize);
	/// Creates a job handle for a new job
	explicit JobHandle(JobFunction function);
	/// Destructor does not wait for the job to finish
	virtual ~JobHandle() = default;

	/// Move constructor
	JobHandle(JobHandle &&other);
	/// Move assignment operator
	JobHandle &operator=(JobHandle &&other);

	/// Creates a job handle for a new job, with optional custom data
	NODISCARD static JobHandle createJob(JobFunction function, const void *data, unsigned int dataSize);
	//// Creates a job handle for a new job
	NODISCARD static inline JobHandle createJob(JobFunction function) { return createJob(function, nullptr, 0); }

	/// Creates a job handle for a new parallel for job
	template <typename T, typename S>
	NODISCARD static JobHandle createParallelForJob(T *data, unsigned int count, void (*function)(T *, unsigned int), const S &splitter);

	/// Creates a job handle for a new child job, with optional custom data
	JobHandle createChildJob(JobFunction function, const void *data, unsigned int dataSize);
	//// Creates a job handle for a new child job
	JobHandle inline createChildJob(JobFunction function) { return createChildJob(function, nullptr, 0); }

	/// Creates a job handle for a new continuation job, with optional custom data
	JobHandle createContinuationJob(JobFunction function, const void *data, unsigned int dataSize);
	/// Creates a job handle for a new continuation job
	JobHandle inline createContinuationJob(JobFunction function) { return createContinuationJob(function, nullptr, 0); }

	/// Submits the job to the calling thread queue
	bool submit();
	/// Submits multiple jobs to the calling thread queue
	static uint32_t submit(const JobHandle *handles, uint32_t count);
	/// Waits until the job has finished, while carrying on other jobs
	void wait();
	/// Tries to cancel the job before it gets executed
	bool cancel();
	/// Releases the current job id without waiting on it
	JobId release();

	/// Returns the number of unfinished jobs for this job
	int32_t unfinishedJobs() const;
	/// Returns the number of continuation jobs for this job
	int32_t continuationCount() const;

	/// Returns `true` if the job id is valid
	bool isValid() const;

  protected:
	JobId jobId_;

  private:
	/// Deleted copy constructor
	JobHandle(const JobHandle &) = delete;
	/// Deleted assignment operator
	JobHandle &operator=(const JobHandle &) = delete;
};

/// A wrapper class around the job system `JobId`, that waits the job on destruction
class DLL_PUBLIC ScopedJobHandle : public JobHandle
{
  public:
	/// Creates a scoped job handle for a new job, with an invalid job id
	ScopedJobHandle();
	/// Creates a scoped job handle for an existing job with the specified job id
	explicit ScopedJobHandle(JobId jobId);
	/// Creates a scoped job handle from an existing simple one
	explicit ScopedJobHandle(JobHandle jobHandle);
	/// Creates a scoped job handle for a new job, with optional custom data
	ScopedJobHandle(JobFunction function, const void *data, unsigned int dataSize);
	/// Creates a scoped job handle for a new job
	explicit ScopedJobHandle(JobFunction function);
	/// Destructor waits for job completion before releasing the id
	~ScopedJobHandle() override;

	/// Move constructor
	ScopedJobHandle(ScopedJobHandle &&other);
	/// Move assignment operator
	ScopedJobHandle &operator=(ScopedJobHandle &&other);

	/// Creates a scoped job handle for a new job, with optional custom data
	NODISCARD static ScopedJobHandle createJob(JobFunction function, const void *data, unsigned int dataSize);
	/// Creates a scoped job handle for a new job
	NODISCARD static inline ScopedJobHandle createJob(JobFunction function) { return createJob(function, nullptr, 0); }

	template <typename T, typename S>
	NODISCARD static ScopedJobHandle createParallelForJob(T *data, unsigned int count, void (*function)(T *, unsigned int), const S &splitter);

  private:
	/// Tries to cancel a job, if it cannot it waits on it
	void cancelOrWait();

	/// Deleted copy constructor
	ScopedJobHandle(const ScopedJobHandle &) = delete;
	/// Deleted assignment operator
	ScopedJobHandle &operator=(const ScopedJobHandle &) = delete;
};

template <typename T, typename S>
JobHandle JobHandle::createParallelForJob(T *data, unsigned int count, void (*function)(T *, unsigned int), const S &splitter)
{
	typedef parallelForJobData<T, S> JobData;
	static_assert(sizeof(JobData) <= JobDataSize, "The embedded Job data buffer is too small for parallelFor");
	const JobData jobData(data, count, function, splitter);

	JobId jobId = theServiceLocator().jobSystem().createJob(&parallelForJob<JobData>, &jobData, sizeof(JobData));
	return JobHandle(jobId);
}

template <typename T, typename S>
ScopedJobHandle ScopedJobHandle::createParallelForJob(T *data, unsigned int count, void (*function)(T *, unsigned int), const S &splitter)
{
	typedef parallelForJobData<T, S> JobData;
	static_assert(sizeof(JobData) <= JobDataSize, "The embedded Job data buffer is too small for parallelFor");
	const JobData jobData(data, count, function, splitter);

	JobId jobId = theServiceLocator().jobSystem().createJob(&parallelForJob<JobData>, &jobData, sizeof(JobData));
	return ScopedJobHandle(jobId);
}

}

#endif
