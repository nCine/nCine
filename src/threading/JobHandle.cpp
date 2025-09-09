#include "JobHandle.h"
#include "ServiceLocator.h"
#include <nctl/Array.h>

namespace ncine {

///////////////////////////////////////////////////////////
// `JobHandle` class
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

JobHandle::JobHandle()
    : jobId_(InvalidJobId)
{
}

JobHandle::JobHandle(JobId jobId)
    : jobId_(jobId)
{
}

JobHandle::JobHandle(JobFunction function, const void *data, unsigned int dataSize)
{
	jobId_ = theServiceLocator().jobSystem().createJob(function, data, dataSize);
}

JobHandle::JobHandle(JobFunction function)
    : JobHandle(function, nullptr, 0)
{
}

JobHandle::JobHandle(JobHandle &&other)
    : jobId_(other.release())
{
}

JobHandle &JobHandle::operator=(JobHandle &&other)
{
	if (this != &other)
		jobId_ = other.release();
	return *this;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

NODISCARD JobHandle JobHandle::createJob(JobFunction function, const void *data, unsigned int dataSize)
{
	JobId jobId = theServiceLocator().jobSystem().createJob(function, data, dataSize);
	return JobHandle(jobId);
}

JobHandle JobHandle::createChildJob(JobFunction function, const void *data, unsigned int dataSize)
{
	if (isValid() == false)
		return JobHandle(InvalidJobId);

	JobId childId = theServiceLocator().jobSystem().createJobAsChild(jobId_, function, data, dataSize);
	return JobHandle(childId);
}

JobHandle JobHandle::createContinuationJob(JobFunction function, const void *data, unsigned int dataSize)
{
	if (isValid() == false)
		return JobHandle(InvalidJobId);

	JobId continuationId = theServiceLocator().jobSystem().createJob(function, data, dataSize);
	if (theServiceLocator().jobSystem().addContinuation(jobId_, continuationId))
		return JobHandle(continuationId);
	else
	{
		theServiceLocator().jobSystem().cancel(continuationId);
		return JobHandle(InvalidJobId);
	}
}

bool JobHandle::submit()
{
	return theServiceLocator().jobSystem().submit(jobId_);
}

uint32_t JobHandle::submit(const JobHandle *handles, uint32_t count)
{
	if (handles == nullptr || count <= 0)
		return 0;

	nctl::Array<JobId> jobIds(count);
	for (uint32_t i = 0; i < count; i++)
	{
		if (handles[i].isValid())
			jobIds.pushBack(handles[i].jobId_);
	}

	if (jobIds.isEmpty())
		return 0;

	return theServiceLocator().jobSystem().submit(jobIds.data(), jobIds.size());
}

/*! \note When the wait is over, the job id will become invalid */
void JobHandle::wait()
{
	theServiceLocator().jobSystem().wait(jobId_);
	jobId_ = InvalidJobId;
}

bool JobHandle::cancel()
{
	bool canBeCancelled = theServiceLocator().jobSystem().cancel(jobId_);
	if (canBeCancelled)
		jobId_ = InvalidJobId;
	return canBeCancelled;
}

/*! \note After releasing it, the job id will become invalid */
JobId JobHandle::release()
{
	JobId releasedId = jobId_;
	jobId_ = InvalidJobId;
	return releasedId;
}

int32_t JobHandle::unfinishedJobs() const
{
	return theServiceLocator().jobSystem().unfinishedJobs(jobId_);
}

int32_t JobHandle::continuationCount() const
{
	return theServiceLocator().jobSystem().continuationCount(jobId_);
}

bool JobHandle::isValid() const
{
	return (jobId_ != InvalidJobId);
}

///////////////////////////////////////////////////////////
// `ScopedJobHandle` class
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ScopedJobHandle::ScopedJobHandle()
    : JobHandle()
{
}

ScopedJobHandle::ScopedJobHandle(JobId jobId)
    : JobHandle(jobId)
{
}

ScopedJobHandle::ScopedJobHandle(JobHandle jobHandle)
    : ScopedJobHandle(jobHandle.release())
{
}

ScopedJobHandle::ScopedJobHandle(JobFunction function, const void *data, unsigned int dataSize)
    : JobHandle(function, data, dataSize)
{
}

ScopedJobHandle::ScopedJobHandle(JobFunction function)
    : JobHandle(function, nullptr, 0)
{
}

ScopedJobHandle::~ScopedJobHandle()
{
	cancelOrWait();
}

ScopedJobHandle::ScopedJobHandle(ScopedJobHandle &&other)
    : JobHandle(nctl::move(other))
{
}

ScopedJobHandle &ScopedJobHandle::operator=(ScopedJobHandle &&other)
{
	if (this != &other)
	{
		cancelOrWait();
		jobId_ = other.release();
	}
	return *this;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

NODISCARD ScopedJobHandle ScopedJobHandle::createJob(JobFunction function, const void *data, unsigned int dataSize)
{
	JobId jobId = theServiceLocator().jobSystem().createJob(function, data, dataSize);
	return ScopedJobHandle(jobId);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ScopedJobHandle::cancelOrWait()
{
	if (theServiceLocator().jobSystem().cancel(jobId_) == false)
		theServiceLocator().jobSystem().wait(jobId_);

	jobId_ = InvalidJobId;
}

}
