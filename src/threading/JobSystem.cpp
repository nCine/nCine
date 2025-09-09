#include "JobSystem.h"
#include "JobQueue.h"
#include "JobStatistics.h"
#include <nctl/StaticString.h>

#if JOB_DEBUG_TRACY_ZONES
	#include "tracy.h"
#endif

namespace ncine {

namespace {
	JobStatistics::JobSystemStatsHelper *statsHelper = nullptr;
#if JOB_DEBUG_TRACY_ZONES
	nctl::StaticString<64> zoneTextString;
#endif

	JobId getJob(JobQueue *jobQueues, unsigned char numThreads)
	{
#if JOB_DEBUG_TRACY_ZONES
		ZoneScoped;
#endif

		const unsigned char threadIndex = JobSystem::threadIndex();
		const unsigned char mainThreadIndex = JobSystem::mainThreadIndex();
		static thread_local unsigned int lastStealIndex = mainThreadIndex;

		// Try to pop from own queue first
		JobId jobId = jobQueues[threadIndex].pop();
		if (jobId != InvalidJobId)
		{
#if JOB_DEBUG_TRACY_ZONES
			zoneTextString.format("Popped JobId: %u", jobId);
			ZoneText(zoneTextString.data(), zoneTextString.length());
#endif
			return jobId;
		}

		// Then try to steal from main thread's queue
		if (threadIndex != mainThreadIndex)
		{
			JobId stolenJob = jobQueues[mainThreadIndex].steal();
			if (stolenJob != InvalidJobId)
			{
#if JOB_DEBUG_TRACY_ZONES
				zoneTextString.format("Stolen from main thread's queue, JobId: %u", stolenJob);
				ZoneText(zoneTextString.data(), zoneTextString.length());
#endif
				statsHelper->jobSystemStatsMut().incrementJobsStolen();
				return stolenJob;
			}
		}

		// At the end, try to steal from each other in turn
		for (unsigned char attempt = 0; attempt < numThreads - 1; attempt++)
		{
			const unsigned int stealIndex = (lastStealIndex + attempt) % numThreads;
			// Skip calling thread queue and main thread's one (already tried)
			if (stealIndex == threadIndex || stealIndex == mainThreadIndex)
				continue;

			JobId stolenJob = jobQueues[stealIndex].steal();
			if (stolenJob != InvalidJobId)
			{
#if JOB_DEBUG_TRACY_ZONES
				zoneTextString.format("Stolen from thread #%02d, JobId: %u", stealIndex, stolenJob);
				ZoneText(zoneTextString.data(), zoneTextString.length());
#endif
				statsHelper->jobSystemStatsMut().incrementJobsStolen();
				lastStealIndex = (stealIndex + 1) % numThreads; // rotate start
				return stolenJob;
			}
		}

		// No job stolen: always rotate starting index to avoid starvation
		lastStealIndex = (lastStealIndex + 1) % numThreads;
		return InvalidJobId;
	}

	void finish(JobId jobId, JobPool &jobPool, JobQueue *jobQueues)
	{
#if JOB_DEBUG_TRACY_ZONES
		ZoneScoped;
#endif

		ASSERT(jobId != InvalidJobId);
		if (jobId == InvalidJobId)
			return;

		Job *job = jobPool.retrieveJob(jobId);
		if (job == nullptr)
			return;

		job->clearFlag(Job::Flags::RUNNING);
		const uint16_t prevUnfinishedJobs = job->decrementUnfinishedJobs();
		if (prevUnfinishedJobs == 1)
		{
			if (job->parent != InvalidJobId)
			{
				JOB_LOG_DEP("Child %u finished, decrementing parent", jobId);
				statsHelper->jobSystemStatsMut().incrementChildJobsFinished();
				finish(job->parent, jobPool, jobQueues);
			}
			else
				statsHelper->jobSystemStatsMut().incrementParentJobsFinished();

			// Run follow-up jobs
			const uint16_t continuationCount = job->loadContinuationCount(nctl::MemoryModel::ACQUIRE);
			for (uint16_t i = 0; i < continuationCount; i++)
			{
				jobQueues[JobSystem::threadIndex()].push(job->continuations[i]);
				statsHelper->jobSystemStatsMut().incrementContinuationJobsPushed();
			}

#if JOB_DEBUG_TRACY_ZONES
			zoneTextString.format("JobId: %u", jobId);
			ZoneText(zoneTextString.data(), zoneTextString.length());
#endif
			jobStateExcutingToFinished(job, jobId); // Job debug state transition
			statsHelper->jobSystemStatsMut().incrementJobsFinished();
			jobPool.freeJob(jobId);
		}
	}

	void execute(JobId jobId, JobPool &jobPool, JobQueue *jobQueues)
	{
#if JOB_DEBUG_TRACY_ZONES
		ZoneScoped;
#endif

		Job *job = jobPool.retrieveJob(jobId);
		if (job != nullptr)
		{
			jobStatePoppedToExcuting(job, jobId); // Job debug state transition
			statsHelper->jobSystemStatsMut().incrementJobsExecuted();

			const uint32_t oldFlags = job->setFlag(Job::Flags::RUNNING);
			// Skip execution if cancellation has been requested for the job
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
			finish(jobId, jobPool, jobQueues);
		}
	}
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

JobSystem::JobSystem()
    : JobSystem(0)
{
}

JobSystem::JobSystem(unsigned char numThreads)
    : IJobSystem(numThreads), commonData_(jobPool_, queueSem_)
{
	// Zero threads means automatic
	if (numThreads_ == 0)
		numThreads_ = cpuTopology_.numPhysicalCores();
	if (numThreads > cpuTopology_.numTotalCores())
		numThreads_ = cpuTopology_.numTotalCores();

	theJobStatistics().initialize(numThreads_);
	statsHelper = theJobStatistics().jobSystemStatsHelper();

	jobPool_.initialize(numThreads_);
	jobQueues_.setCapacity(numThreads_);
	for (unsigned char i = 0; i < numThreads_; i++)
		jobQueues_.emplaceBack();

#if JOB_DEBUG_STATE
	for (unsigned char i = 0; i < numThreads_; i++)
		jobQueues_[i].setJobPool(&jobPool_);
#endif

	commonData_.numThreads = numThreads_;
	commonData_.jobQueues = jobQueues_.data();

	threadStructs_.setCapacity(numThreads_ - 1); // Capacity needs to be set to avoid reallocation and pointer invalidation
	threads_.setCapacity(numThreads_ - 1);
	for (unsigned char i = 0; i < numThreads_ - 1; i++)
	{
		const unsigned char threadIndex = i + 1;
		threadStructs_.emplaceBack(commonData_);
		threadStructs_.back().threadIndex = threadIndex;
		threadStructs_.back().cpuId = cpuTopology_.cpuInfo(threadIndex).id;

		threads_.emplaceBack();
		// Create a thread before setting its name and affinity mask
		threads_[i].run(workerFunction, &threadStructs_[i]);
	}

	// Setting the affinity mask of the main thread
#ifndef __EMSCRIPTEN__
	if (numThreads_ > 1)
	{
		const unsigned char cpuId = cpuTopology_.cpuInfo(threadIndex()).id;
		ThisThread::setAffinityMask(ThreadAffinityMask(cpuId));
	}
#endif
}

JobSystem::~JobSystem()
{
	for (unsigned char i = 0; i < numThreads_ - 1; i++)
		threadStructs_[i].shouldQuit = true;
	queueSem_.signal(numThreads_);
	for (unsigned char i = 0; i < numThreads_ - 1; i++)
		threads_[i].join();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/*! \warning Always check that the returned `JobId` is valid
 *  \note The `function` can be `nullptr` for synchronization-only jobs */
JobId JobSystem::createJob(JobFunction function, const void *data, unsigned int dataSize)
{
	return createJobAsChild(InvalidJobId, function, data, dataSize);
}

/*! \warning Always check that the returned `JobId` is valid
 *  \note The `function` can be `nullptr` for synchronization-only jobs */
JobId JobSystem::createJobAsChild(JobId parentId, JobFunction function, const void *data, unsigned int dataSize)
{
#if JOB_DEBUG_TRACY_ZONES
	ZoneScoped;
#endif

	Job *parent = nullptr;
	if (parentId != InvalidJobId)
	{
		parent = jobPool_.retrieveJob(parentId);
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

	JobId jobId = jobPool_.allocateJob();
	if (jobId == InvalidJobId)
		return InvalidJobId;

	Job *job = jobPool_.retrieveJob(jobId);
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
		statsHelper->jobSystemStatsMut().incrementChildJobsCreated();
		JOB_LOG_DEP("Child job %u attached to parent %u", jobId, parentId);
	}

	statsHelper->jobSystemStatsMut().incrementJobsCreated();
	return jobId;
}

bool JobSystem::addContinuation(JobId ancestorId, JobId continuationId)
{
#if JOB_DEBUG_TRACY_ZONES
	ZoneScoped;
#endif

	if (ancestorId == InvalidJobId || continuationId == InvalidJobId)
		return false;

	Job *ancestorJob = jobPool_.retrieveJob(ancestorId);
	if (ancestorJob == nullptr)
		return false;
	Job *continuationJob = jobPool_.retrieveJob(continuationId);
	if (continuationJob == nullptr)
		return false;

	bool continuationAdded = false;
	const uint32_t state = ancestorJob->countersAndState.load(nctl::MemoryModel::ACQUIRE);
	if ((state & Job::Flags::CANCELLED) != 0)
		return false;

	const uint32_t count = ancestorJob->incrementContinuationCount();
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

bool JobSystem::submit(JobId jobId)
{
	return (submit(&jobId, 1) == 1);
}

uint16_t JobSystem::submit(const JobId *jobIds, uint16_t count)
{
#if JOB_DEBUG_TRACY_ZONES
	ZoneScoped;
#endif

	ASSERT(jobIds != nullptr);
	ASSERT(count > 0);

	JobQueue &queue = jobQueues_[threadIndex()];
	uint16_t numSubmitted = 0;

	for (uint16_t i = 0; i < count; i++)
	{
		ASSERT(jobIds[i] != InvalidJobId);
		if (jobIds[i] == InvalidJobId)
			continue;

		Job *job = jobPool_.retrieveJob(jobIds[i]);
		if (job == nullptr)
			continue;

		const uint32_t oldFlags = job->setFlag(Job::Flags::SUBMITTED);
		if ((oldFlags & (Job::Flags::SUBMITTED | Job::Flags::CANCELLED)) != 0)
			continue;

#if JOB_DEBUG_TRACY_ZONES
		zoneTextString.format("Submitted jobId[%u]: %u", i, jobIds[i]);
		ZoneText(zoneTextString.data(), zoneTextString.length());
#endif

		queue.push(jobIds[i]);
		numSubmitted++;
	}

	if (numSubmitted > 0)
		queueSem_.signal(numSubmitted);

	return numSubmitted;
}

bool JobSystem::cancel(JobId jobId)
{
#if JOB_DEBUG_TRACY_ZONES
	ZoneScoped;
#endif

	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return false;

	Job *job = jobPool_.retrieveJob(jobId);
	if (job == nullptr)
		return false;

	uint32_t oldState = job->countersAndState.load(nctl::MemoryModel::RELAXED);
	while (true)
	{
		// If already running, cannot cancel
		if ((oldState & Job::Flags::RUNNING) != 0)
			return false;

		const uint32_t newState = oldState | Job::Flags::CANCELLED;
		if (job->countersAndState.cmpExchange(oldState, newState, nctl::MemoryModel::ACQ_REL))
		{
			// If the job has not been submitted yet, we can free it immediately
			if ((oldState & Job::Flags::SUBMITTED) == 0)
			{
				jobStateForceToFinished(job, jobId); // Job debug state transition
				jobPool_.freeJob(jobId);
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

void JobSystem::wait(JobId jobId)
{
#if JOB_DEBUG_TRACY_ZONES
	ZoneScoped;
#endif

	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return;

	Job *job = jobPool_.retrieveJob(jobId);
	// Not asserting here as a valid job may complete and be recycled before
	// the caller reaches `wait()`, so `retrieveJob()` can return `nullptr`.
	if (job == nullptr)
		return;

#if JOB_DEBUG_TRACY_ZONES
	zoneTextString.format("JobId: %u", jobId);
	ZoneText(zoneTextString.data(), zoneTextString.length());
#endif

	unsigned int spinCount = 0;
	unsigned int yieldCount = 0;
	unsigned int spinDebugCount = 0;

	// Wait until the job has completed. In the meantime, work on any other job.
	while (true)
	{
		const uint16_t unfinishedJobs = job->loadUnfinishedJobs(nctl::MemoryModel::ACQUIRE);
		const uint32_t state = job->countersAndState.load(nctl::MemoryModel::ACQUIRE);
		if (unfinishedJobs == 0 || (state & Job::Flags::CANCELLED) != 0)
			break;

		JobId nextJob = getJob(jobQueues_.data(), numThreads_);
		if (nextJob != InvalidJobId)
		{
			execute(nextJob, jobPool_, jobQueues_.data());
			spinCount = 0;
			yieldCount = 0;
			spinDebugCount = 0;
		}
		else
		{
			// No job available (spin, then yield)
			if (spinCount < 1000)
				spinCount++;
			else if (yieldCount < 50)
			{
				ThisThread::yield();
				yieldCount++;
			}

			if (++spinDebugCount > 100000)
			{
				LOGW_X("Potential deadlock: jobId %u (idx: %u, gen: %u), unfinishedJobs=%d, flags=%dS%dR%dC",
				       jobId, Job::unpackIndex(jobId), Job::unpackGeneration(jobId), job->loadUnfinishedJobs(nctl::MemoryModel::RELAXED),
				       Job::hasFlag(state, Job::Flags::SUBMITTED), Job::hasFlag(state, Job::Flags::RUNNING), Job::hasFlag(state, Job::Flags::CANCELLED));
				spinDebugCount = 0;
			}
		}
	}
}

uint16_t JobSystem::unfinishedJobs(JobId jobId)
{
	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return 0;

	Job *job = jobPool_.retrieveJob(jobId);
	return job->loadUnfinishedJobs(nctl::MemoryModel::ACQUIRE);
}

uint16_t JobSystem::continuationCount(JobId jobId)
{
	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return 0;

	Job *job = jobPool_.retrieveJob(jobId);
	return job->loadContinuationCount(nctl::MemoryModel::ACQUIRE);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void JobSystem::workerFunction(void *arg)
{
	const ThreadStruct *threadStruct = static_cast<const ThreadStruct *>(arg);
	const unsigned char numThreads = threadStruct->commonData.numThreads;
	SemType &queueSem = threadStruct->commonData.queueSem;
	JobPool &jobPool = threadStruct->commonData.jobPool;
	JobQueue *jobQueues = threadStruct->commonData.jobQueues;

	setThreadIndex(threadStruct->threadIndex);
	nctl::StaticString<16> threadName;
	threadName.format("WorkerThread#%02d", threadStruct->threadIndex);
#ifndef __EMSCRIPTEN__
	ThisThread::setName(threadName.data());
	ThisThread::setAffinityMask(ThreadAffinityMask(threadStruct->cpuId));
#endif

	LOGI_X("WorkerThread#%02d (id: %lu) is starting on CPU#%02u", threadStruct->threadIndex, ThisThread::threadId(), threadStruct->cpuId);

	while (true)
	{
		// Wait until a job is available or we are asked to quit
		queueSem.wait();

		if (threadStruct->shouldQuit)
			break;

		JobId jobId = getJob(jobQueues, numThreads);
		if (jobId == InvalidJobId)
			continue; // Spurious wake-up or nothing to steal, just wait again

		execute(jobId, jobPool, jobQueues);
	}

	LOGI_X("WorkerThread#%02d (id: %lu) on CPU#%02u is exiting", threadStruct->threadIndex, ThisThread::threadId(), threadStruct->cpuId);
}

}
