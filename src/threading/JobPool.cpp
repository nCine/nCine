#include "JobPool.h"
#include "JobStatistics.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

JobPool::JobPool()
    : numThreads_(0)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

JobId JobPool::allocateJob()
{
	FATAL_ASSERT(numThreads_ > 0);
	const unsigned char threadIndex = IJobSystem::threadIndex();

	ThreadCache &cache = threadCaches_[threadIndex];

	if (cache.top > 0)
	{
		const uint16_t index = cache.indices[--cache.top];
		Job &job = jobPool_[index];

		JobId jobId = Job::makeJobId(index, job.generation);
		FATAL_ASSERT_MSG(jobId != InvalidJobId, "Generated an invalid job id");

		JOB_LOG_POOL_ALLOC(jobId, index, job.generation);
		theJobStatistics().jobPoolStatsMut().incrementJobsAllocated();
		return jobId;
	}

	// Cache empty, refill from global pool
	{
		LockGuard lock(mutex_);
		uint16_t count = freeList_.size();
		if (ThreadCacheSize < count)
			count = ThreadCacheSize;
		if (count == 0)
		{
			theJobStatistics().jobPoolStatsMut().incrementJobAllocationFails();
			return InvalidJobId;
		}

		for (uint16_t i = 0; i < count; ++i)
		{
			cache.indices[i] = freeList_.back();
			freeList_.popBack();
		}
		cache.top = count;
		theJobStatistics().jobPoolStatsMut().incrementThreadCacheEmpty();
	}

	// Retry allocation (now cache has items)
	return allocateJob();
}

void JobPool::freeJob(JobId jobId)
{
	FATAL_ASSERT(numThreads_ > 0);
	const unsigned char threadIndex = IJobSystem::threadIndex();

	const uint16_t index = Job::unpackIndex(jobId);
	ASSERT(index < MaxNumJobs);

	Job *job = &jobPool_[index];
	jobStateFinishedToFree(job, jobId); // Job debug state transition
	JOB_LOG_POOL_FREE(jobId, index, job->generation);

	job->function = nullptr;
	job->parent = InvalidJobId;
	job->continuationCount = 0;
	job->generation++;

	ThreadCache &cache = threadCaches_[threadIndex];
	if (cache.top < ThreadCacheSize)
		cache.indices[cache.top++] = index;
	else
	{
		// Cache full, flush half back to global pool
		LockGuard lock(mutex_);
		for (int i = ThreadCacheSize / 2; i < ThreadCacheSize; ++i)
			freeList_.pushBack(cache.indices[i]);
		cache.top = ThreadCacheSize / 2;
		cache.indices[cache.top++] = index;
		theJobStatistics().jobPoolStatsMut().incrementThreadCacheFull();
	}

	theJobStatistics().jobPoolStatsMut().incrementJobsFreed();
}

Job *JobPool::retrieveJob(JobId jobId)
{
	FATAL_ASSERT(numThreads_ > 0);
	ASSERT(jobId != InvalidJobId);

	const uint16_t index = Job::unpackIndex(jobId);
	if (jobId == InvalidJobId || index >= MaxNumJobs)
	{
		theJobStatistics().jobPoolStatsMut().incrementJobRetrievalFails();
		return nullptr;
	}

	Job *job = &jobPool_[index];
	if (job->generation == Job::unpackGeneration(jobId))
	{
		theJobStatistics().jobPoolStatsMut().incrementJobsRetrieved();
		return job;
	}

	theJobStatistics().jobPoolStatsMut().incrementJobRetrievalFails();
	return nullptr;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void JobPool::initialize(unsigned char numThreads)
{
	FATAL_ASSERT(numThreads > 0);
	// Initialize the free list with all job indices (backwards)
	for (unsigned int i = 0; i < MaxNumJobs; i++)
		freeList_.pushBack(MaxNumJobs - 1 - i);

	threadCaches_.setCapacity(numThreads);
	for (unsigned char i = 0; i < numThreads; i++)
		threadCaches_.emplaceBack();

	numThreads_ = numThreads;
}

}
