#include "SerialJobSystem.h"

#include "tracy.h"

// TODO: Test this implementation (fix the queue)

namespace ncine {

namespace {
	static const unsigned int Mask = SerialJobSystem::NumJobs - 1u;

	void finish(Job *job)
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
				finish(job->parent);

			// run follow-up jobs
			//for (int i = 0; i < job->continuationCount; i++) // TODO: Implement this
			//	jobQueues[JobSystem::threadIndex()].push(job->continuations[i]);
		}
	}

	void execute(Job *job)
	{
		ZoneScoped;
		LOGD_X("Main thread is executing its function");

		JobId jobId = reinterpret_cast<JobId>(job);
		(job->function)(jobId, job->data);
		finish(job);
	}
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

SerialJobSystem::SerialJobSystem()
    : allocatedJobs_(0), jobPool_(nctl::StaticArrayMode::EXTEND_SIZE), jobQueue_(nctl::StaticArrayMode::EXTEND_SIZE)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

JobId SerialJobSystem::createJob(JobFunction function, const void *data, unsigned int dataSize)
{
	ASSERT(function);

	// The main thread creates jobs and it can only touch its queue and pool
	Job *job = retrieveJob();
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

	return reinterpret_cast<JobId>(job);
}

JobId SerialJobSystem::createJobAsChild(JobId parentId, JobFunction function, const void *data, unsigned int dataSize)
{
	ASSERT(function);

	Job *parent = reinterpret_cast<Job*>(parentId);
	if (parent)
		parent->unfinishedJobs.fetchAdd(1);

	// The main thread creates jobs and it can only touch its queue and pool
	Job *job = retrieveJob();
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

	return reinterpret_cast<JobId>(job);
}

bool SerialJobSystem::addContinuation(JobId ancestor, JobId continuation)
{
	Job *ancestorJob = reinterpret_cast<Job*>(ancestor);
	Job *continuationJob = reinterpret_cast<Job*>(continuation);

	bool continuationAdded = false;
	const uint32_t count = ancestorJob->continuationCount.fetchAdd(1);
	if (count < JobNumContinuations)
	{
		ancestorJob->continuations[count - 1] = continuationJob;
		continuationAdded = true;
	}
	else
		ancestorJob->continuationCount.fetchSub(1);

	return continuationAdded;
}

void SerialJobSystem::run(JobId jobId)
{
	Job *job = reinterpret_cast<Job*>(jobId);

	ASSERT(job);
	if (job == nullptr)
		return;

	pushJob(job);
}


void SerialJobSystem::wait(const JobId jobId)
{
	Job *job = reinterpret_cast<Job*>(jobId);

	ASSERT(job);
	if (job == nullptr)
		return;

	// wait until the job has completed. in the meantime, work on any other job.
	while (job->unfinishedJobs > 0)
	{
		Job *nextJob = popJob();
		if (nextJob)
			execute(nextJob);
	}
}

int32_t SerialJobSystem::unfinishedJobs(JobId jobId)
{
	Job *job = reinterpret_cast<Job*>(jobId);

	ASSERT(job);
	return (job) ? job->unfinishedJobs.load() : 0;
}

bool SerialJobSystem::isValid(JobId jobId)
{
	const Job *job = reinterpret_cast<Job*>(jobId);
	return (job != nullptr && job->function != nullptr);
}


///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

// TODO: Check that no more than `NumJobs` entries are allocated per frame, or memset a finished job
Job *SerialJobSystem::retrieveJob()
{
	const unsigned int index = allocatedJobs_++;
	Job *job = &jobPool_[index & Mask];
	return job;
}

// TODO: Check that the queue is not full
void SerialJobSystem::pushJob(Job *job)
{
	jobQueue_[bottom_ & Mask] = job;
	bottom_++;
}

// TODO: Check that the queue is not empty
Job *SerialJobSystem::popJob()
{
	bottom_--;
	return jobQueue_[bottom_ & Mask];
}

}
