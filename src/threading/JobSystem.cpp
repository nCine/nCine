#include "JobSystem.h"
#include "JobQueue.h"
#include <nctl/StaticString.h>
#include <nctl/PointerMath.h>

#include "tracy.h"

// TODO: Test with allocators
// TODO: Test parallel for and continuations
// TODO: Test on Android, Emnscripten, Windows, MinGW, macOS

namespace ncine {

namespace {
	static const unsigned int AlignmentBytes = sizeof(int);

#if PACKED_JOBID
	static const unsigned int ThreadIndexShiftBits = 7;
	static const unsigned int PoolIndexMask = (~0u) >> ThreadIndexShiftBits;

	JobId packJobId(unsigned char threadIndex, unsigned int poolIndex)
	{
		FATAL_ASSERT_MSG_X(threadIndex < (2 << ThreadIndexShiftBits), "With a packed JobId, the thread id can't be more than %u", (2 << ThreadIndexShiftBits) - 1);
		FATAL_ASSERT_MSG_X(poolIndex < (2 << (sizeof(JobId) - ThreadIndexShiftBits)), "With a packed JobId, the pool index can't be more than %u", (2 << (sizeof(JobId) - ThreadIndexShiftBits)) - 1);
		const JobId jobId = (threadIndex << ThreadIndexShiftBits) + poolIndex;
		return jobId;
	}

	Job *jobPointerFromId(const JobId jobId, JobQueue *jobQueues)
	{
		const unsigned char threadIndex = (jobId >> (sizeof(JobId) - ThreadIndexShiftBits));
		const unsigned int poolIndex = (jobId & PoolIndexMask);
		return jobQueues[threadIndex][poolIndex];
	}
#endif

	Job *getJob(JobQueue *jobQueues, unsigned char numThreads)
	{
		JobQueue &queue = jobQueues[JobSystem::threadIndex()];

		Job *job = queue.pop();
		if (job == nullptr)
		{
			// Calling thread queue is empty, try to steal from each other in turn
			Job *stolenJob = nullptr;
			for (unsigned int i = 0; i < numThreads; i++)
			{
				const unsigned int stealIndex = (JobSystem::threadIndex() + i + 1) % numThreads;
				if (stealIndex == JobSystem::threadIndex())
					continue;

				JobQueue &stealQueue = jobQueues[stealIndex];
				stolenJob = stealQueue.steal();
				if (stolenJob != nullptr)
					break;
			}
			return stolenJob;
		}

		return job;
	}

	void finish(Job *job, JobQueue *jobQueues)
	{
		ASSERT(job);
		if (job == nullptr)
			return;

		const int32_t unfinishedJobs = --job->unfinishedJobs;
		if (unfinishedJobs == 0)
		{
			// Releasing the job back to the pool.
			// When retrieving a new one, the function pointer should be checked.
			job->function = nullptr; // TODO: Should this assignment be atomic?

			if (job->parent)
				finish(job->parent, jobQueues);

			// run follow-up jobs
			for (int i = 0; i < job->continuationCount; i++)
				jobQueues[JobSystem::threadIndex()].push(job->continuations[i]);
		}
	}

	void execute(Job *job, JobQueue *jobQueues)
	{
		ZoneScoped;
		//LOGD_X("Worker thread %u (index: #%u) is executing its function", Thread::self(), JobSystem::threadIndex());

		JobId jobId = reinterpret_cast<JobId>(job);
		ASSERT(job->function);
		(job->function)(jobId, job->data);
		finish(job, jobQueues);
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

	// ===============================================================================
	//numThreads_ = 2; // TODO: ONE THREAD DEBUG
	// ===============================================================================

#ifdef __EMSCRIPTEN__
	// TODO: This does not work and there is always one thread on Emscripten
	numThreads_ = 5; // TODO: use the value passed to `PTHREAD_POOL_SIZE`
#endif

	const unsigned int bytes = numThreads_ * sizeof(JobQueue);
	const unsigned int alignedBytes = bytes + AlignmentBytes; // One align adjustment in `initPointers()`
#if !NCINE_WITH_ALLOCATORS
	jobQueuesBuffer_ = static_cast<uint8_t *>(::operator new(alignedBytes));
#else
	// TODO: Use a job system only allocator? (BTW, test well custom allocator and job system)
	jobQueuesBuffer_ = static_cast<uint8_t *>(nctl::theDefaultAllocator().allocate(alignedBytes));
#endif

	initPointers();

	commonData_.numThreads = numThreads_;
	commonData_.jobQueues = jobQueues_;

	threadStructs_.setCapacity(numThreads_ - 1); // Capacity needs to be set to avoid reallocation and pointer invalidation
	threads_.setCapacity(numThreads_ - 1);
	nctl::StaticString<128> threadName;
	for (unsigned int i = 0; i < numThreads_ - 1; i++)
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
		// TODO: Check that affinity works, and check how to distribue indices among cores
		threads_.back().setAffinityMask(ThreadAffinityMask(threadIndex));
	#endif
#endif
	}
}

JobSystem::~JobSystem()
{
	for (unsigned int i = 0; i < numThreads_ - 1; i++)
		threadStructs_[i].shouldQuit = true;
	queueCV_.broadcast();
	for (unsigned int i = 0; i < numThreads_ - 1; i++)
		threads_[i].join();

#if !NCINE_WITH_ALLOCATORS
	::operator delete(jobQueuesBuffer_);
#else
	nctl::theDefaultAllocator().deallocate(jobQueuesBuffer_);
#endif
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

JobId JobSystem::createJob(JobFunction function, const void *data, unsigned int dataSize)
{
	ASSERT(function);

	// The main thread creates jobs and it can only touch its queue and pool
#if PACKED_JOBID
	unsigned int poolIndex = 0;
	Job *job = jobQueues_[threadIndex()].retrieveJob(poolIndex);
#else
	Job *job = jobQueues_[threadIndex()].retrieveJob();
#endif
	if (job && job->function == nullptr)
	{
		job->function = function; // TODO: Test with lambda
		job->parent = nullptr;
		job->unfinishedJobs = 1;

		if (data != nullptr && dataSize > 0)
		{
			FATAL_ASSERT(dataSize <= sizeof(Job::data));
			memcpy(job->data, data, dataSize);
		}
	}

	// TODO: Make it possible for the application to check if `job` is nullptr
#if PACKED_JOBID
	return packJobId(threadIndex(), poolIndex);
#else
	return reinterpret_cast<JobId>(job);
#endif
}


JobId JobSystem::createJobAsChild(JobId parentId, JobFunction function, const void *data, unsigned int dataSize)
{
	ASSERT(function);

#if PACKED_JOBID
	Job *parent = jobPointerFromId(parentId, jobQueues_);
#else
	Job *parent = reinterpret_cast<Job*>(parentId);
#endif
	if (parent)
		parent->unfinishedJobs.fetchAdd(1);

	// The main thread creates jobs and it can only touch its queue and pool
#if PACKED_JOBID
	unsigned int poolIndex = 0;
	Job *job = jobQueues_[threadIndex()].retrieveJob(poolIndex);
#else
	Job *job = jobQueues_[threadIndex()].retrieveJob();
#endif
	if (job && job->function == nullptr)
	{
		job->function = function; // TODO: Test with lambda
		job->parent = reinterpret_cast<Job*>(parent);
		job->unfinishedJobs = 1;

		if (data != nullptr && dataSize > 0)
		{
			FATAL_ASSERT(dataSize <= sizeof(Job::data));
			memcpy(job->data, data, dataSize);
		}
	}

	// TODO: Make it possible for the application to check if `job` is nullptr
#if PACKED_JOBID
	return packJobId(threadIndex(), poolIndex);
#else
	return reinterpret_cast<JobId>(job);
#endif
}

bool JobSystem::addContinuation(JobId ancestor, JobId continuation)
{
#if PACKED_JOBID
	Job *ancestorJob = jobPointerFromId(ancestor, jobQueues_);
	Job *continuationJob = jobPointerFromId(continuation, jobQueues_);
#else
	Job *ancestorJob = reinterpret_cast<Job*>(ancestor);
	Job *continuationJob = reinterpret_cast<Job*>(continuation);
#endif

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

void JobSystem::run(JobId jobId)
{
	Job *job = reinterpret_cast<Job*>(jobId);

	ASSERT(job);
	if (job == nullptr)
		return;

	JobQueue &queue = jobQueues_[threadIndex()];
	queue.push(job);

	queueCV_.broadcast();
}

void JobSystem::wait(JobId jobId)
{
	Job *job = reinterpret_cast<Job*>(jobId);

	ASSERT(job);
	if (job == nullptr)
		return;

	// Wait until the job has completed. In the meantime, work on any other job.
	while (job->unfinishedJobs > 0)
	{
		//LOGI_X("unfinished: %u", job->unfinishedJobs.load());

		Job *nextJob = getJob(jobQueues_, numThreads_);
		if (nextJob)
			execute(nextJob, jobQueues_);

		Thread::yieldExecution(); // TODO: With or without?
	}
	//LOGI_X("unfinished: %u", job->unfinishedJobs.load());
}

int32_t JobSystem::unfinishedJobs(JobId jobId)
{
	Job *job = reinterpret_cast<Job*>(jobId);

	ASSERT(job);
	return (job) ? job->unfinishedJobs.load() : 0;
}

bool JobSystem::isValid(JobId jobId)
{
	const Job *job = reinterpret_cast<Job*>(jobId);
	return (job != nullptr && job->function != nullptr);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void JobSystem::initPointers()
{
	unsigned int alignAdjustment = nctl::PointerMath::alignAdjustment(jobQueuesBuffer_, AlignmentBytes);
	uint8_t *pointer = reinterpret_cast<uint8_t *>(nctl::PointerMath::align(jobQueuesBuffer_, AlignmentBytes));
	jobQueues_ = reinterpret_cast<JobQueue *>(pointer);
	pointer += sizeof(JobQueue) * numThreads_;

	const unsigned int bytes = numThreads_ * sizeof(JobQueue);
	FATAL_ASSERT(pointer == jobQueuesBuffer_ + bytes + alignAdjustment);

#if !NCINE_WITH_ALLOCATORS
	jobQueues_ = static_cast<JobQueue *>(::operator new(numThreads_ * sizeof(JobQueue)));
#else
	jobQueues_ = static_cast<JobQueue *>(nctl::theDefaultAllocator().allocate(numThreads_ * sizeof(JobQueue)));
#endif

	for (unsigned int i = 0; i < numThreads_; i++)
		new (jobQueues_ + i) JobQueue();
}

void JobSystem::workerFunction(void *arg)
{
	const ThreadStruct *threadStruct = static_cast<const ThreadStruct *>(arg);
	Mutex &queueMutex = threadStruct->commonData.queueMutex;
	CondVariable &queueCV = threadStruct->commonData.queueCV;
	setThreadIndex(threadStruct->threadIndex);

	//LOGD_X("Worker thread %u (id: #%u) is starting", Thread::self(), threadStruct->threadIndex);

	while (true)
	{
		Job *job = nullptr;
		while ((job = getJob(threadStruct->commonData.jobQueues, threadStruct->commonData.numThreads)) == nullptr &&
		       threadStruct->shouldQuit == false)
		{
			queueMutex.lock();
			queueCV.wait(queueMutex);
			queueMutex.unlock();
		}

		if (threadStruct->shouldQuit)
			break;

		ASSERT(job != nullptr);
		execute(job, threadStruct->commonData.jobQueues);
	}

	//LOGD_X("Worker thread %u (id: #%u) is exiting", Thread::self(), threadStruct->threadIndex);
}

}
