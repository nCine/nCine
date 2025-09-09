#include "jobsystem_debug.h"

#if JOB_DEBUG_STATE
	#include "Job.h"
	#include "JobPool.h"
	#include "common_macros.h"

namespace ncine {

const char *jobStateToString(Job::State state)
{
	switch (state)
	{
		default:
		case Job::State::FREE: return "Free";
		case Job::State::PUSHED: return "Pushed";
		case Job::State::POPPED: return "Popped";
		case Job::State::EXECUTING: return "Executing";
		case Job::State::FINISHED: return "Finished";
	}
}

void transitionJobState(Job *job, JobId jobId, Job::State oldState, Job::State newState)
{
	ASSERT(job != nullptr);

	if (job != nullptr)
	{
		uint32_t expected = static_cast<uint32_t>(oldState);
		const bool success = job->state.cmpExchange(expected, static_cast<uint32_t>(newState), nctl::MemoryModel::ACQUIRE);
		ASSERT_MSG_X(success == true, "Cannot change job %u (index: %u, generation: %u) state from %s to %s (current: %s)",
		             jobId, Job::unpackIndex(jobId), Job::unpackGeneration(jobId),
		             jobStateToString(oldState), jobStateToString(newState), jobStateToString(Job::State(expected)));
	}
}

void transitionJobState(JobPool *jobPool, JobId jobId, Job::State oldState, Job::State newState)
{
	FATAL_ASSERT(jobPool != nullptr);
	Job *job = jobPool->retrieveJob(jobId);
	transitionJobState(job, jobId, oldState, newState);
}

void jobStateFreeToPushed(JobPool *jobPool, JobId jobId)
{
	transitionJobState(jobPool, jobId, Job::State::FREE, Job::State::PUSHED);
}

void jobStatePushedToPopped(JobPool *jobPool, JobId jobId)
{
	transitionJobState(jobPool, jobId, Job::State::PUSHED, Job::State::POPPED);
}

void jobStateFreeToPushed(Job *job, JobId jobId)
{
	transitionJobState(job, jobId, Job::State::FREE, Job::State::PUSHED);
}

void jobStatePushedToPopped(Job *job, JobId jobId)
{
	transitionJobState(job, jobId, Job::State::PUSHED, Job::State::POPPED);
}

void jobStateFinishedToFree(Job *job, JobId jobId)
{
	transitionJobState(job, jobId, Job::State::FINISHED, Job::State::FREE);
	const uint16_t unfinishedJobs = job->loadUnfinishedJobs(nctl::MemoryModel::ACQUIRE);
	ASSERT_MSG_X(unfinishedJobs == 0, "Job %u has still %u unfinished jobs", jobId, unfinishedJobs);
}

void jobStatePoppedToExcuting(Job *job, JobId jobId)
{
	transitionJobState(job, jobId, Job::State::POPPED, Job::State::EXECUTING);
}

void jobStateExcutingToFinished(Job *job, unsigned int jobId)
{
	transitionJobState(job, jobId, Job::State::EXECUTING, Job::State::FINISHED);
}

void jobStateForceToFinished(Job *job, unsigned int jobId)
{
	ASSERT(job != nullptr);
	if (job != nullptr)
	{
		job->state.store(static_cast<uint32_t>(Job::State::FINISHED), nctl::MemoryModel::RELEASE);
		while (job->loadUnfinishedJobs(nctl::MemoryModel::ACQUIRE) > 0)
			job->decrementUnfinishedJobs();
	}
}

}
#endif
