#include "JobQueue.h"
#include "common_macros.h"

// Based on https://blog.molecular-matters.com/2015/09/25/job-system-2-0-lock-free-work-stealing-part-3-going-lock-free/

namespace ncine {

static_assert(MaxNumJobs > 1 && (MaxNumJobs & (MaxNumJobs - 1)) == 0, "The number of jobs needs to be a power of two for the mask to work");
static const unsigned int Mask = MaxNumJobs - 1u;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

JobQueue::JobQueue()
    : top_(0), bottom_(0)
{
	for (unsigned int i = 0; i < MaxNumJobs; i++)
		jobs_[i] = InvalidJobId;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void JobQueue::push(JobId jobId)
{
	const int32_t b = bottom_.load(nctl::MemoryModel::RELAXED);
	const int32_t t = top_.load(nctl::MemoryModel::RELAXED);

	if ((b - t) >= static_cast<int32_t>(MaxNumJobs))
	{
		LOGE_X("JobQueue overflow: bottom=%d, top=%d, capacity=%u", b, t, MaxNumJobs);
		return;
	}

	jobs_[b & Mask] = jobId;

	bottom_.store(b + 1, nctl::MemoryModel::RELEASE);
}

JobId JobQueue::pop()
{
	int32_t b = bottom_.load(nctl::MemoryModel::RELAXED);
	b = b - 1;
	bottom_.store(b, nctl::MemoryModel::RELAXED);

	nctl::AtomicFences::threadFence(nctl::MemoryModel::ACQUIRE);

	int32_t t = top_.load(nctl::MemoryModel::ACQUIRE);
	if (t <= b)
	{
		// Non-empty queue
		JobId jobId = jobs_[b & Mask];

		if (t != b)
		{
			jobs_[b & Mask] = InvalidJobId;
			// There's still more than one item left in the queue
			return jobId;
		}

		// This is the last item in the queue
		if (top_.cmpExchange(t, t + 1, nctl::MemoryModel::ACQ_REL) == false)
		{
			// Failed race against steal operation
			jobId = InvalidJobId;
		}

		jobs_[b & Mask] = InvalidJobId;
		bottom_.store(t + 1, nctl::MemoryModel::RELAXED);
		return jobId;
	}
	else
	{
		// Deque was already empty
		bottom_.store(t, nctl::MemoryModel::RELAXED);
		return InvalidJobId;
	}
}

JobId JobQueue::steal()
{
	int32_t t = top_.load(nctl::MemoryModel::ACQUIRE);
	const int32_t b = bottom_.load(nctl::MemoryModel::ACQUIRE);

	if (t < b)
	{
		if (top_.cmpExchange(t, t + 1, nctl::MemoryModel::ACQUIRE) == false)
		{
			// A concurrent steal or pop operation removed an element from the deque in the meantime.
			return InvalidJobId;
		}

		// Non-empty queue
		JobId jobId = jobs_[t & Mask];
		jobs_[t & Mask] = InvalidJobId;
		return jobId;
	}
	else
	{
		// Empty queue
		return InvalidJobId;
	}
}

}
