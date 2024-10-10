#include "JobSystem.h"
#include "JobQueue.h"
#include <nctl/StaticString.h>

namespace ncine {

namespace {
	Job *getJob(JobQueue *jobQueues, unsigned char numThreads)
	{
		const unsigned char threadIndex = JobSystem::threadIndex();
		const unsigned char mainThreadIndex = JobSystem::mainThreadIndex();
		static thread_local unsigned int lastStealIndex = mainThreadIndex;

		// Try to pop from own queue first
		Job *job = jobQueues[threadIndex].pop();
		if (job != nullptr)
			return job;

		// Then try to steal from main thread's queue
		if (threadIndex != mainThreadIndex)
		{
			Job *stolenJob = jobQueues[mainThreadIndex].steal();
			if (stolenJob != nullptr)
				return stolenJob;
		}

		// At the end, try to steal from each other in turn
		for (unsigned char attempt = 0; attempt < numThreads - 1; attempt++)
		{
			const unsigned int stealIndex = (lastStealIndex + attempt) % numThreads;
			// Skip calling thread queue and main thread's one (already tried)
			if (stealIndex == threadIndex || stealIndex == mainThreadIndex)
				continue;

			Job *stolenJob = jobQueues[stealIndex].steal();
			if (stolenJob != nullptr)
			{
				lastStealIndex = (stealIndex + 1) % numThreads; // rotate start
				return stolenJob;
			}
		}

		// No job stolen: always rotate starting index to avoid starvation
		lastStealIndex = (lastStealIndex + 1) % numThreads;
		return nullptr;
	}

	void finish(Job *job, JobQueue *jobQueues)
	{
		ASSERT(job != nullptr);
		if (job == nullptr)
			return;

		const int32_t unfinishedJobs = job->unfinishedJobs.fetchSub(1, nctl::MemoryModel::RELEASE) - 1;
		if (unfinishedJobs == 0)
		{
			if (job->parent != nullptr)
				finish(job->parent, jobQueues);

			// Run follow-up jobs
			for (int i = 0; i < job->continuationCount; i++)
				jobQueues[JobSystem::threadIndex()].push(job->continuations[i]);
		}
	}

	void execute(Job *job, JobQueue *jobQueues)
	{
		if (job != nullptr)
		{
			JobId jobId = reinterpret_cast<JobId>(job);
			// A job might have no function to execute
			if (job->function != nullptr)
				(job->function)(jobId, job->data);
			finish(job, jobQueues);
		}
	}
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

JobSystem::JobSystem()
    : JobSystem(static_cast<unsigned char>(Thread::numProcessors()))
{
}

JobSystem::JobSystem(unsigned char numThreads)
    : IJobSystem(numThreads), commonData_(queueMutex_, queueCV_)
{
	ASSERT(numThreads > 0);
	if (numThreads == 0)
		numThreads_ = 1; // There is at least the main thread

	const unsigned char numProcessors = static_cast<unsigned char>(Thread::numProcessors());
	ASSERT(numThreads <= numProcessors);
	if (numThreads > numProcessors)
		numThreads_ = numProcessors;

	jobQueues_.setCapacity(numThreads_);
	for (unsigned char i = 0; i < numThreads_; i++)
		jobQueues_.emplaceBack();

	commonData_.numThreads = numThreads_;
	commonData_.jobQueues = jobQueues_.data();

	threadStructs_.setCapacity(numThreads_ - 1); // Capacity needs to be set to avoid reallocation and pointer invalidation
	threads_.setCapacity(numThreads_ - 1);
	nctl::StaticString<128> threadName;
	for (unsigned char i = 0; i < numThreads_ - 1; i++)
	{
		const unsigned char threadIndex = i + 1;
		threadStructs_.emplaceBack(commonData_);
		threadStructs_.back().threadIndex = threadIndex;

		threads_.emplaceBack();
		// Create a thread before setting its name and affinity mask
		threads_[i].run(workerFunction, &threadStructs_[i]);

#if !defined(__EMSCRIPTEN__)
	#if !defined(__APPLE__)
		threadName.format("WorkerThread#%02d", threadIndex);
		threads_.back().setName(threadName.data());
	#endif
	#if !defined(__ANDROID__)
		threads_.back().setAffinityMask(ThreadAffinityMask(threadIndex));
	#endif
#endif
	}
}

JobSystem::~JobSystem()
{
	for (unsigned char i = 0; i < numThreads_ - 1; i++)
		threadStructs_[i].shouldQuit = true;
	queueCV_.broadcast();
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
		Job *parent = reinterpret_cast<Job *>(parentId);
		if (parent == nullptr)
			return InvalidJobId;
	}

	Job *job = jobQueues_[threadIndex()].retrieveJob();
	if (job == nullptr)
		return InvalidJobId;

	job->function = function;
	job->parent = parent;
	job->unfinishedJobs = 1;
	job->continuationCount = 0;

	if (data != nullptr && dataSize > 0)
	{
		FATAL_ASSERT(dataSize <= JobDataSize);
		memcpy(job->data, data, dataSize);
	}

	if (parent != nullptr)
		parent->unfinishedJobs.fetchAdd(1);

	return reinterpret_cast<JobId>(job);
}

bool JobSystem::addContinuation(JobId ancestorId, JobId continuationId)
{
	if (ancestorId == InvalidJobId || continuationId == InvalidJobId)
		return false;

	Job *ancestorJob = reinterpret_cast<Job *>(ancestorId);
	if (ancestorJob == nullptr)
		return false;
	Job *continuationJob = reinterpret_cast<Job *>(continuationId);
	if (continuationJob == nullptr)
		return false;

	bool continuationAdded = false;
	const uint32_t count = ancestorJob->continuationCount.fetchAdd(1);
	if (count < JobNumContinuations)
	{
		ancestorJob->continuations[count] = continuationJob;
		continuationAdded = true;
	}
	else
		ancestorJob->continuationCount.fetchSub(1);

	return continuationAdded;
}

void JobSystem::submit(JobId jobId)
{
	Job *job = reinterpret_cast<Job *>(jobId);
	ASSERT(job != nullptr);
	if (job == nullptr)
		return;

	JobQueue &queue = jobQueues_[threadIndex()];
	queue.push(job);

	queueCV_.broadcast();
}

void JobSystem::wait(JobId jobId)
{
	Job *job = reinterpret_cast<Job *>(jobId);
	ASSERT(job != nullptr);
	if (job == nullptr)
		return;

	unsigned int spinCount = 0;
	unsigned int yieldCount = 0;
	unsigned int spinDebugCount = 0;

	// Wait until the job has completed. In the meantime, work on any other job.
	while (job->unfinishedJobs.load(nctl::MemoryModel::ACQUIRE) > 0)
	{
		Job *nextJob = getJob(jobQueues_.data(), numThreads_);
		if (nextJob != nullptr)
		{
			execute(nextJob, jobQueues_.data());
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
				Thread::yieldExecution();
				yieldCount++;
			}

			if (++spinDebugCount > 100000)
			{
				LOGW_X("Potential deadlock: jobId %u, unfinishedJobs=%d",
				       jobId, job->unfinishedJobs.load());
				spinDebugCount = 0;
			}
		}
	}
}

int32_t JobSystem::unfinishedJobs(JobId jobId)
{
	Job *job = reinterpret_cast<Job *>(jobId);
	ASSERT(job != nullptr);
	if (job == nullptr)
		return 0;

	return job->unfinishedJobs.load(nctl::MemoryModel::ACQUIRE);
}

int32_t JobSystem::continuationCount(JobId jobId)
{
	Job *job = reinterpret_cast<Job *>(jobId);
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
	Mutex &queueMutex = threadStruct->commonData.queueMutex;
	CondVariable &queueCV = threadStruct->commonData.queueCV;
	JobQueue *jobQueues = threadStruct->commonData.jobQueues;
	setThreadIndex(threadStruct->threadIndex);

	LOGI_X("WorkerThread#%02d (id: %lu) is starting", threadStruct->threadIndex, Thread::self());

	while (true)
	{
		// Wait until a job is available or we are asked to quit
		queueMutex.lock();
		queueCV.wait(queueMutex);
		queueMutex.unlock();

		if (threadStruct->shouldQuit)
			break;

		Job *job = getJob(jobQueues, numThreads);
		if (job == nullptr)
			continue; // Spurious wake-up or nothing to steal, just wait again

		execute(job, jobQueues);
	}

	LOGI_X("WorkerThread#%02d (id: %lu) is exiting", threadStruct->threadIndex, Thread::self());
}

}
