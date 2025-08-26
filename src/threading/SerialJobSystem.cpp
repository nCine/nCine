#include "SerialJobSystem.h"
#include "JobStatistics.h"

namespace ncine {

static_assert(MaxNumJobs > 1 && (MaxNumJobs & (MaxNumJobs - 1)) == 0, "The number of jobs needs to be a power of two for the mask to work");
static const unsigned int Mask = MaxNumJobs - 1u;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

SerialJobSystem::SerialJobSystem()
    : IJobSystem(1),
      freeList_(nctl::StaticArrayMode::EXTEND_SIZE),
      top_(0), bottom_(0)
{
	// Initialize the free list with all job indices (backwards)
	for (unsigned int i = 0; i < MaxNumJobs; i++)
		freeList_[i] = MaxNumJobs - 1 - i;

	theJobStatistics().initialize(numThreads_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/*! \warning Always check that the returned `JobId` is valid
 *  \note The `function` can be `nullptr` for synchronization-only jobs */
JobId SerialJobSystem::createJob(JobFunction function, const void *data, unsigned int dataSize)
{
	return createJobAsChild(InvalidJobId, function, data, dataSize);
}

/*! \warning Always check that the returned `JobId` is valid
 *  \note The `function` can be `nullptr` for synchronization-only jobs */
JobId SerialJobSystem::createJobAsChild(JobId parentId, JobFunction function, const void *data, unsigned int dataSize)
{
	Job *parent = nullptr;
	if (parentId != InvalidJobId)
	{
		parent = retrieveJob(parentId);
		if (parent == nullptr)
			return InvalidJobId;
	}

	JobId jobId = allocateJob();
	if (jobId == InvalidJobId)
		return InvalidJobId;

	Job *job = retrieveJob(jobId);
	if (job == nullptr)
		return InvalidJobId;

	job->function = function;
	job->parent = parentId;
	job->unfinishedJobs = 1;
	job->continuationCount = 0;

	if (data != nullptr && dataSize > 0)
	{
		FATAL_ASSERT(dataSize <= JobDataSize);
		memcpy(job->data, data, dataSize);
	}

	if (parent != nullptr)
	{
		parent->unfinishedJobs.fetchAdd(1);
		theJobStatistics().jobSystemStatsMut().incrementChildJobsCreated();
		JOB_LOG_DEP("Child job %u attached to parent %u", jobId, parentId);
	}

	theJobStatistics().jobSystemStatsMut().incrementJobsCreated();
	return jobId;
}

bool SerialJobSystem::addContinuation(JobId ancestorId, JobId continuationId)
{
	if (ancestorId == InvalidJobId || continuationId == InvalidJobId)
		return false;

	Job *ancestorJob = retrieveJob(ancestorId);
	if (ancestorJob == nullptr)
		return false;
	Job *continuationJob =  retrieveJob(continuationId);
	if (continuationJob == nullptr)
		return false;

	bool continuationAdded = false;
	const uint32_t count = ancestorJob->continuationCount.fetchAdd(1);
	if (count < JobNumContinuations)
	{
		ancestorJob->continuations[count] = continuationId;
		JOB_LOG_DEP("Continuation job %u (idx: %u, gen: %u) added to %u (idx: %u, gen: %u)",
		            continuationId, Job::unpackIndex(continuationId), Job::unpackGeneration(continuationId),
		            ancestorId, Job::unpackIndex(ancestorId), Job::unpackGeneration(ancestorId));
		continuationAdded = true;
	}
	else
		ancestorJob->continuationCount.fetchSub(1);

	return continuationAdded;
}

void SerialJobSystem::submit(JobId jobId)
{
	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return;

	Job *job = retrieveJob(jobId);
	if (job != nullptr)
		pushJob(jobId);
}

void SerialJobSystem::wait(const JobId jobId)
{
	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return;

	Job *job = retrieveJob(jobId);
	ASSERT(job != nullptr);
	if (job == nullptr)
		return;

	// Wait until the job has completed. In the meantime, work on any other job.
	while (job->unfinishedJobs > 0)
	{
		JobId nextJob = popJob();
		if (nextJob != InvalidJobId)
			execute(nextJob, *this);
	}
}

int32_t SerialJobSystem::unfinishedJobs(JobId jobId)
{
	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return 0;

	Job *job = retrieveJob(jobId);
	ASSERT(job != nullptr);
	if (job == nullptr)
		return 0;

	return job->unfinishedJobs.load(nctl::MemoryModel::ACQUIRE);
}

int32_t SerialJobSystem::continuationCount(JobId jobId)
{
	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return 0;

	Job *job = retrieveJob(jobId);
	ASSERT(job != nullptr);
	if (job == nullptr)
		return 0;

	return job->continuationCount.load(nctl::MemoryModel::ACQUIRE);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

JobId SerialJobSystem::allocateJob()
{
	if (freeList_.isEmpty() == false)
	{
		const uint16_t index = freeList_.back();
		freeList_.popBack();
		Job &job = jobPool_[index];

		JobId jobId = Job::makeJobId(index, job.generation);
		FATAL_ASSERT_MSG(jobId != InvalidJobId, "Generated an invalid job id");

		JOB_LOG_POOL_ALLOC(jobId, index, job.generation);
		theJobStatistics().jobPoolStatsMut().incrementJobsAllocated();
		return jobId;
	}
	else
	{
		theJobStatistics().jobPoolStatsMut().incrementJobAllocationFails();
		return InvalidJobId;
	}
}

void SerialJobSystem::freeJob(JobId jobId)
{
	const uint16_t index = Job::unpackIndex(jobId);
	ASSERT(index < MaxNumJobs);

	Job *job = &jobPool_[index];
	jobStateFinishedToFree(job, jobId);
	JOB_LOG_POOL_FREE(jobId, index, job->generation);

	job->function = nullptr;
	job->parent = InvalidJobId;
	job->continuationCount = 0;
	job->generation++;

	freeList_.pushBack(index);
	theJobStatistics().jobPoolStatsMut().incrementJobsFreed();
}

Job *SerialJobSystem::retrieveJob(JobId jobId)
{
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

void SerialJobSystem::pushJob(JobId jobId)
{
	const unsigned int nextBottom = (bottom_ + 1) & Mask;
	if (nextBottom == (top_ & Mask))
		return; // Job queue overflow

	jobQueue_[bottom_ & Mask] = jobId;

	jobStateFreeToPushed(retrieveJob(jobId), jobId); // Job debug state transition
	JOB_LOG_QUEUE_PUSH(jobId, Job::unpackIndex(jobId), Job::unpackGeneration(jobId), bottom_);
	theJobStatistics().jobQueueStatsMut().incrementPushes();

	bottom_ = nextBottom;
}

JobId SerialJobSystem::popJob()
{
	if (top_ == bottom_)
	{
		theJobStatistics().jobQueueStatsMut().incrementPopFails();
		return InvalidJobId;
	}

	JobId jobId = jobQueue_[top_ & Mask];
	top_ = (top_ + 1) & Mask;

	jobStatePushedToPopped(retrieveJob(jobId), jobId); // Job debug state transition
	JOB_LOG_QUEUE_POP(jobId, Job::unpackIndex(jobId), Job::unpackGeneration(jobId), bottom_, top_);
	theJobStatistics().jobQueueStatsMut().incrementPops();

	return jobId;
}

void SerialJobSystem::finish(JobId jobId, SerialJobSystem &jobSystem)
{
	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return;

	Job *job = jobSystem.retrieveJob(jobId);
	if (job == nullptr)
		return;

	const int32_t unfinishedJobs = job->unfinishedJobs.fetchSub(1, nctl::MemoryModel::RELEASE) - 1;
	if (unfinishedJobs == 0)
	{
		if (job->parent != InvalidJobId)
		{
			JOB_LOG_DEP("Child %u finished, decrementing parent", jobId);
			theJobStatistics().jobSystemStatsMut().incrementChildJobsFinished();
			finish(job->parent, jobSystem);
		}
		else
			theJobStatistics().jobSystemStatsMut().incrementParentJobsFinished();

		// Run follow-up jobs
		for (int i = 0; i < job->continuationCount; i++)
		{
			jobSystem.pushJob(job->continuations[i]);
			theJobStatistics().jobSystemStatsMut().incrementContinuationJobsPushed();
		}

		jobStateExcutingToFinished(job, jobId); // Job debug state transition
		theJobStatistics().jobSystemStatsMut().incrementJobsFinished();
		jobSystem.freeJob(jobId);
	}
}

void SerialJobSystem::execute(JobId jobId, SerialJobSystem &jobSystem)
{
	Job *job = jobSystem.retrieveJob(jobId);
	if (job != nullptr)
	{
		jobStatePoppedToExcuting(job, jobId); // Job debug state transition
		theJobStatistics().jobSystemStatsMut().incrementJobsExecuted();

		// A job might have no function to execute
		if (job->function != nullptr)
			(job->function)(jobId, job->data);
		finish(jobId, jobSystem);
	}
}

}
