#include "SerialJobSystem.h"
#include "JobStatistics.h"

#if JOB_DEBUG_TRACY_ZONES
	#include "tracy.h"
	#include <nctl/StaticString.h>
#endif

namespace ncine {

static_assert(MaxNumJobs > 1 && (MaxNumJobs & (MaxNumJobs - 1)) == 0, "The number of jobs needs to be a power of two for the mask to work");
static const unsigned int Mask = MaxNumJobs - 1u;

namespace {
#if JOB_DEBUG_TRACY_ZONES
	nctl::StaticString<64> zoneTextString;
#endif
}

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
#if JOB_DEBUG_TRACY_ZONES
	ZoneScoped;
#endif

	Job *parent = nullptr;
	if (parentId != InvalidJobId)
	{
		parent = retrieveJob(parentId);
		if (parent == nullptr)
			return InvalidJobId;

		const uint32_t state = parent->countersAndState.load(nctl::MemoryModel::ACQUIRE);
		if ((state & Job::Flags::CANCELLED) != 0)
			return InvalidJobId;

#if JOB_DEBUG_TRACY_ZONES
		zoneTextString.format("Parent: %u", parentId);
		ZoneText(zoneTextString.data(), zoneTextString.length());
#endif
	}

	JobId jobId = allocateJob();
	if (jobId == InvalidJobId)
		return InvalidJobId;

	Job *job = retrieveJob(jobId);
	if (job == nullptr)
		return InvalidJobId;

	job->function = function;
	job->parent = parentId;
	job->countersAndState.store(0, nctl::MemoryModel::RELEASE);
	job->incrementUnfinishedJobs();

	if (data != nullptr && dataSize > 0)
	{
		FATAL_ASSERT(dataSize <= JobDataSize);
		memcpy(job->data, data, dataSize);
	}

	if (parent != nullptr)
	{
		parent->incrementUnfinishedJobs();
		theJobStatistics().jobSystemStatsMut().incrementChildJobsCreated();
		JOB_LOG_DEP("Child job %u attached to parent %u", jobId, parentId);
	}

	theJobStatistics().jobSystemStatsMut().incrementJobsCreated();
	return jobId;
}

bool SerialJobSystem::addContinuation(JobId ancestorId, JobId continuationId)
{
#if JOB_DEBUG_TRACY_ZONES
	ZoneScoped;
#endif

	if (ancestorId == InvalidJobId || continuationId == InvalidJobId)
		return false;

	Job *ancestorJob = retrieveJob(ancestorId);
	if (ancestorJob == nullptr)
		return false;
	Job *continuationJob =  retrieveJob(continuationId);
	if (continuationJob == nullptr)
		return false;

	bool continuationAdded = false;
	const uint16_t count = ancestorJob->incrementContinuationCount();
	if (count < JobNumContinuations)
	{
		ancestorJob->continuations[count] = continuationId;
		JOB_LOG_DEP("Continuation job %u (idx: %u, gen: %u) added to %u (idx: %u, gen: %u)",
		            continuationId, Job::unpackIndex(continuationId), Job::unpackGeneration(continuationId),
		            ancestorId, Job::unpackIndex(ancestorId), Job::unpackGeneration(ancestorId));
		continuationAdded = true;

#if JOB_DEBUG_TRACY_ZONES
		zoneTextString.format("ancestorId: %u, continuationId: %u", ancestorId, continuationId);
		ZoneText(zoneTextString.data(), zoneTextString.length());
#endif
	}
	else
		ancestorJob->decrementContinuationCount();

	return continuationAdded;
}

bool SerialJobSystem::submit(JobId jobId)
{
	return (submit(&jobId, 1) == 1);
}

uint16_t SerialJobSystem::submit(const JobId *jobIds, uint16_t count)
{
#if JOB_DEBUG_TRACY_ZONES
	ZoneScoped;
#endif

	ASSERT(jobIds != nullptr);
	ASSERT(count > 0);
	uint16_t numSubmitted = 0;

	for (uint16_t i = 0; i < count; i++)
	{
		ASSERT(jobIds[i] != InvalidJobId);
		if (jobIds[i] == InvalidJobId)
			continue;

		Job *job = retrieveJob(jobIds[i]);
		if (job == nullptr)
			continue;

		const uint32_t oldFlags = job->setFlag(Job::Flags::SUBMITTED);
		if ((oldFlags & (Job::Flags::SUBMITTED | Job::Flags::CANCELLED)) != 0)
			continue;

#if JOB_DEBUG_TRACY_ZONES
		zoneTextString.format("Submitted jobId[%u]: %u", i, jobIds[i]);
		ZoneText(zoneTextString.data(), zoneTextString.length());
#endif

		pushJob(jobIds[i]);
		numSubmitted++;
	}

	return numSubmitted;
}

bool SerialJobSystem::cancel(JobId jobId)
{
#if JOB_DEBUG_TRACY_ZONES
	ZoneScoped;
#endif

	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return false;

	Job *job = retrieveJob(jobId);
	if (job == nullptr)
		return false;

	uint32_t oldState = job->countersAndState.load(nctl::MemoryModel::RELAXED);
	while (true)
	{
		// If already running, cannot cancel
		if ((oldState & (Job::Flags::RUNNING)) != 0)
			return false;

		const uint32_t newState = oldState | Job::Flags::CANCELLED;
		if (job->countersAndState.cmpExchange(oldState, newState, nctl::MemoryModel::SEQ_CST))
		{
			// If the job has not been submitted yet, we can free it immediately
			if ((oldState & Job::Flags::SUBMITTED) == 0)
			{
				jobStateForceToFinished(job, jobId); // Job debug state transition
				freeJob(jobId);
			}

#if JOB_DEBUG_TRACY_ZONES
			zoneTextString.format("JobId: %u", jobId);
			ZoneText(zoneTextString.data(), zoneTextString.length());
#endif
			return true;
		}

		// CAS failed, retry with updated `oldState`
	}
}

void SerialJobSystem::wait(JobId jobId)
{
#if JOB_DEBUG_TRACY_ZONES
	ZoneScoped;
#endif

	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return;

	Job *job = retrieveJob(jobId);
	// Not asserting here as a valid job may complete and be recycled before
	// the caller reaches `wait()`, so `retrieveJob()` can return `nullptr`.
	if (job == nullptr)
		return;

#if JOB_DEBUG_TRACY_ZONES
	zoneTextString.format("JobId: %u", jobId);
	ZoneText(zoneTextString.data(), zoneTextString.length());
#endif

	// Wait until the job has completed. In the meantime, work on any other job.
	while(true)
	{
		const uint16_t unfinishedJobs = job->loadUnfinishedJobs(nctl::MemoryModel::ACQUIRE);
		const uint32_t state = job->countersAndState.load(nctl::MemoryModel::ACQUIRE);
		if (unfinishedJobs == 0 || (state & Job::Flags::CANCELLED) != 0)
			break;

		JobId nextJob = popJob();
		if (nextJob != InvalidJobId)
			execute(nextJob, *this);
	}
}

uint16_t SerialJobSystem::unfinishedJobs(JobId jobId)
{
	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return 0;

	Job *job = retrieveJob(jobId);
	return job->loadUnfinishedJobs(nctl::MemoryModel::ACQUIRE);
}

uint16_t SerialJobSystem::continuationCount(JobId jobId)
{
	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return 0;

	Job *job = retrieveJob(jobId);
	return job->loadContinuationCount(nctl::MemoryModel::ACQUIRE);
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
	job->countersAndState.store(0, nctl::MemoryModel::RELEASE);
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
#if JOB_DEBUG_TRACY_ZONES
		ZoneScoped;
#endif

	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return;

	Job *job = jobSystem.retrieveJob(jobId);
	if (job == nullptr)
		return;

	job->clearFlag(Job::Flags::RUNNING);
	const uint16_t prevUnfinishedJobs = job->decrementUnfinishedJobs();
	if (prevUnfinishedJobs == 1)
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
		const uint16_t continuationCount = job->loadContinuationCount(nctl::MemoryModel::ACQUIRE);
		for (uint16_t i = 0; i < continuationCount; i++)
		{
			jobSystem.pushJob(job->continuations[i]);
			theJobStatistics().jobSystemStatsMut().incrementContinuationJobsPushed();
		}

#if JOB_DEBUG_TRACY_ZONES
			zoneTextString.format("JobId: %u", jobId);
			ZoneText(zoneTextString.data(), zoneTextString.length());
#endif
		jobStateExcutingToFinished(job, jobId); // Job debug state transition
		theJobStatistics().jobSystemStatsMut().incrementJobsFinished();
		jobSystem.freeJob(jobId);
	}
}

void SerialJobSystem::execute(JobId jobId, SerialJobSystem &jobSystem)
{
#if JOB_DEBUG_TRACY_ZONES
		ZoneScoped;
#endif

	Job *job = jobSystem.retrieveJob(jobId);
	if (job != nullptr)
	{
		jobStatePoppedToExcuting(job, jobId); // Job debug state transition
		theJobStatistics().jobSystemStatsMut().incrementJobsExecuted();

		const uint32_t oldFlags = job->setFlag(Job::Flags::RUNNING);
		// Skip execution if job has been requested cancellation
		if ((oldFlags & Job::Flags::CANCELLED) == 0)
		{
			// A job might have no function to execute
			if (job->function != nullptr)
				(job->function)(jobId, job->data);

#if JOB_DEBUG_TRACY_ZONES
			zoneTextString.format("JobId: %u", jobId);
			ZoneText(zoneTextString.data(), zoneTextString.length());
#endif
		}
		finish(jobId, jobSystem);
	}
}

}
