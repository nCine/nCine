#include "JobQueue.h"
#include "Job.h"
#include <cstring> // for `memset()`

// Based on https://blog.molecular-matters.com/2015/09/25/job-system-2-0-lock-free-work-stealing-part-3-going-lock-free/

namespace ncine {

static const unsigned int Mask = JobQueue::NumJobs - 1u;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

JobQueue::JobQueue()
	: top_(0), bottom_(0), allocatedJobs_(0)
{
	memset(jobs_, 0, sizeof(Job *) * NumJobs);
	memset(jobPool_, 0, sizeof(Job) * NumJobs);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void JobQueue::push(Job *job)
{
	const int32_t b = bottom_;
	jobs_[b & Mask] = job;

	// Ensure the job is written before b+1 is published to other threads.
#if defined(__arm__) || defined(__arm64__) ||defined(_M_ARM) || defined(_M_ARM64)
	nctl::AtomicFences::threadFence(nctl::MemoryModel::SEQ_CST);
#else
	// On x86/64, a compiler barrier is enough.
	nctl::AtomicFences::signalFence(nctl::MemoryModel::SEQ_CST); // compiler barrier
#endif

	bottom_ = b + 1;
}

Job *JobQueue::pop()
{
	const int32_t b = bottom_ - 1;
	bottom_ = b;

	// Acts as a memory barier to prevent load/store reordering
	bottom_.cmpExchange(b, b);

	const int32_t t = top_;
	if (t <= b)
	{
		// Non-empty queue
		Job *job = jobs_[b & Mask];
		if (t != b)
		{
			// There's still more than one item left in the queue
			return job;
		}

		// This is the last item in the queue
		if (top_.cmpExchange(t + 1, t) == false)
		{
			// Failed race against steal operation
			job = nullptr;
		}

		bottom_ = t + 1;
		return job;
	}
	else
	{
		// Deque was already empty
		bottom_ = t;
		return nullptr;
	}
}

Job *JobQueue::steal()
{
	const int32_t t = top_;

	// Ensure that top is always read before bottom.
#if defined(__arm__) || defined(__arm64__) ||defined(_M_ARM) || defined(_M_ARM64)
	nctl::AtomicFences::threadFence(nctl::MemoryModel::SEQ_CST);
#else
	// Loads will not be reordered with other loads on x86, so a compiler barrier is enough.
	nctl::AtomicFences::signalFence(nctl::MemoryModel::SEQ_CST); // compiler barrier
#endif

	const int32_t b = bottom_;
	if (t < b)
	{
		// Non-empty queue
		Job *job = jobs_[t & Mask];

		if (top_.cmpExchange(t + 1, t) == false)
		{
			// A concurrent steal or pop operation removed an element from the deque in the meantime.
			return nullptr;
		}

		return job;
	}
	else
	{
		// Empty queue
		return nullptr;
	}
}

#if PACKED_JOBID
// TODO: Check that no more than `NumJobs` entries are allocated per frame, or memset a finished job
Job *JobQueue::retrieveJob(unsigned int &poolIndex)
{
	const unsigned int index = allocatedJobs_++;
	Job *job = &jobPool_[index & Mask];
	poolIndex = (index & Mask);
	return job;
}
#else
// TODO: Check that no more than `NumJobs` entries are allocated per frame, or memset a finished job
Job *JobQueue::retrieveJob()
{
	const unsigned int index = allocatedJobs_++;
	Job *job = &jobPool_[index & Mask];
	return job;
}
#endif

}
