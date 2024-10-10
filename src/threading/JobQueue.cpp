#include "JobQueue.h"
#include <cstring> // for `memset()`
#include "common_macros.h"

// Based on https://blog.molecular-matters.com/2015/09/25/job-system-2-0-lock-free-work-stealing-part-3-going-lock-free/

namespace ncine {

static_assert(MaxNumJobs > 1 && (MaxNumJobs & (MaxNumJobs - 1)) == 0, "The number of jobs needs to be a power of two for the mask to work");
static const unsigned int Mask = MaxNumJobs - 1u;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

JobQueue::JobQueue()
	: top_(0), bottom_(0), allocatedJobs_(0)
{
	memset(jobs_, 0, sizeof(Job *) * MaxNumJobs);
	memset(jobPool_, 0, sizeof(Job) * MaxNumJobs);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void JobQueue::push(Job *job)
{
	const int32_t b = bottom_.load(nctl::MemoryModel::RELAXED);
	const int32_t t = top_.load(nctl::MemoryModel::RELAXED);

	if ((b - t) >= static_cast<int32_t>(MaxNumJobs))
	{
		LOGE_X("JobQueue overflow: bottom=%d, top=%d, capacity=%u", b, t, MaxNumJobs);
		return;
	}

	jobs_[b & Mask] = job;

	bottom_.store(b + 1, nctl::MemoryModel::RELEASE);
}

Job *JobQueue::pop()
{
	int32_t b = bottom_.load(nctl::MemoryModel::RELAXED);
	b = b - 1;
	bottom_.store(b, nctl::MemoryModel::RELAXED);

	nctl::AtomicFences::threadFence(nctl::MemoryModel::ACQUIRE);

	const int32_t t = top_.load(nctl::MemoryModel::ACQUIRE);
	if (t <= b)
	{
		// Non-empty queue
		Job *job = jobs_[b & Mask];
		if (t != b)
		{
			jobs_[b & Mask] = nullptr;
			// There's still more than one item left in the queue
			return job;
		}

		// This is the last item in the queue
		if (top_.cmpExchange(t + 1, t, nctl::MemoryModel::ACQUIRE) == false)
		{
			// Failed race against steal operation
			job = nullptr;
		}

		jobs_[b & Mask] = nullptr;
		bottom_.store(t + 1, nctl::MemoryModel::RELAXED);
		return job;
	}
	else
	{
		// Deque was already empty
		bottom_.store(t, nctl::MemoryModel::RELAXED);
		return nullptr;
	}
}

Job *JobQueue::steal()
{
	int32_t t = top_.load(nctl::MemoryModel::ACQUIRE);
	const int32_t b = bottom_.load(nctl::MemoryModel::ACQUIRE);

	if (t < b)
	{
		if (top_.cmpExchange(t + 1, t, nctl::MemoryModel::ACQUIRE) == false)
		{
			// A concurrent steal or pop operation removed an element from the deque in the meantime.
			return nullptr;
		}

		// Non-empty queue
		Job *job = jobs_[t & Mask];

		jobs_[t & Mask] = nullptr;
		return job;
	}
	else
	{
		// Empty queue
		return nullptr;
	}
}

Job *JobQueue::retrieveJob()
{
	const unsigned int index = allocatedJobs_++;
	Job *job = &jobPool_[index & Mask];
	return job;
}

}
