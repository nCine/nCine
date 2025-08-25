#include "JobSystem.h"
#include "JobQueue.h"
#include "JobStatistics.h"
#include <nctl/StaticString.h>

namespace ncine {

namespace {
	JobStatistics::JobSystemStatsHelper *statsHelper = nullptr;

	JobId getJob(JobQueue *jobQueues, unsigned char numThreads)
	{
		const unsigned char threadIndex = JobSystem::threadIndex();
		const unsigned char mainThreadIndex = JobSystem::mainThreadIndex();
		static thread_local unsigned int lastStealIndex = mainThreadIndex;

		// Try to pop from own queue first
		JobId jobId = jobQueues[threadIndex].pop();
		if (jobId != InvalidJobId)
			return jobId;

		// Then try to steal from main thread's queue
		if (threadIndex != mainThreadIndex)
		{
			JobId stolenJob = jobQueues[mainThreadIndex].steal();
			if (stolenJob != InvalidJobId)
			{
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
		ASSERT(jobId != InvalidJobId);
		if (jobId == InvalidJobId)
			return;

		Job *job = jobPool.retrieveJob(jobId);
		if (job == nullptr)
			return;

		const int32_t unfinishedJobs = job->unfinishedJobs.fetchSub(1, nctl::MemoryModel::RELEASE) - 1;
		if (unfinishedJobs == 0)
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
			for (int i = 0; i < job->continuationCount; i++)
			{
				jobQueues[JobSystem::threadIndex()].push(job->continuations[i]);
				statsHelper->jobSystemStatsMut().incrementContinuationJobsPushed();
			}

			jobStateExcutingToFinished(job, jobId); // Job debug state transition
			statsHelper->jobSystemStatsMut().incrementJobsFinished();
			jobPool.freeJob(jobId);
		}
	}

	void execute(JobId jobId, JobPool &jobPool, JobQueue *jobQueues)
	{
		Job *job = jobPool.retrieveJob(jobId);
		if (job != nullptr)
		{
			jobStatePoppedToExcuting(job, jobId); // Job debug state transition
			statsHelper->jobSystemStatsMut().incrementJobsExecuted();

			// A job might have no function to execute
			if (job->function != nullptr)
				(job->function)(jobId, job->data);
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
	Job *parent = nullptr;
	if (parentId != InvalidJobId)
	{
		parent = jobPool_.retrieveJob(parentId);
		if (parent == nullptr)
			return InvalidJobId;
	}

	JobId jobId = jobPool_.allocateJob();
	if (jobId == InvalidJobId)
		return InvalidJobId;

	Job *job = jobPool_.retrieveJob(jobId);
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
		statsHelper->jobSystemStatsMut().incrementChildJobsCreated();
		JOB_LOG_DEP("Child job %u attached to parent %u", jobId, parentId);
	}

	statsHelper->jobSystemStatsMut().incrementJobsCreated();
	return jobId;
}

bool JobSystem::addContinuation(JobId ancestorId, JobId continuationId)
{
	if (ancestorId == InvalidJobId || continuationId == InvalidJobId)
		return false;

	Job *ancestorJob = jobPool_.retrieveJob(ancestorId);
	if (ancestorJob == nullptr)
		return false;
	Job *continuationJob = jobPool_.retrieveJob(continuationId);
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

void JobSystem::submit(JobId jobId)
{
	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return;

	Job *job = jobPool_.retrieveJob(jobId);
	ASSERT(job != nullptr);
	if (job == nullptr)
		return;

	JobQueue &queue = jobQueues_[threadIndex()];
	queue.push(jobId);

	queueSem_.signal();
}

void JobSystem::wait(JobId jobId)
{
	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return;

	Job *job = jobPool_.retrieveJob(jobId);
	ASSERT(job != nullptr);
	if (job == nullptr)
		return;

	unsigned int spinCount = 0;
	unsigned int yieldCount = 0;
	unsigned int spinDebugCount = 0;

	// Wait until the job has completed. In the meantime, work on any other job.
	while (job->unfinishedJobs.load(nctl::MemoryModel::ACQUIRE) > 0)
	{
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
				LOGW_X("Potential deadlock: jobId %u (idx: %u, gen: %u), unfinishedJobs=%d",
				       jobId, Job::unpackIndex(jobId), Job::unpackGeneration(jobId),
				       job->unfinishedJobs.load(nctl::MemoryModel::RELAXED));
				spinDebugCount = 0;
			}
		}
	}
}

int32_t JobSystem::unfinishedJobs(JobId jobId)
{
	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return 0;

	Job *job = jobPool_.retrieveJob(jobId);
	ASSERT(job != nullptr);
	if (job == nullptr)
		return 0;

	return job->unfinishedJobs.load(nctl::MemoryModel::ACQUIRE);
}

int32_t JobSystem::continuationCount(JobId jobId)
{
	ASSERT(jobId != InvalidJobId);
	if (jobId == InvalidJobId)
		return 0;

	Job *job = jobPool_.retrieveJob(jobId);
	ASSERT(job != nullptr);
	if (job == nullptr)
		return 0;

	return job->continuationCount.load(nctl::MemoryModel::ACQUIRE);
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
