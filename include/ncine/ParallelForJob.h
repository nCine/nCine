#ifndef CLASS_NCINE_PARALLELFORJOB
#define CLASS_NCINE_PARALLELFORJOB

#include "ServiceLocator.h"

namespace ncine {

/// A policy class that splits the workload based on the number of elements to process
class CountSplitter
{
  public:
	explicit CountSplitter(unsigned int count)
	    : count_(count)
	{}

	template <typename T> inline bool split(unsigned int count) const {	return (count > count_); }

  private:
	unsigned int count_;
};

/// A policy class that splits the workload based on the size of a chunk of elements to process
class DataSizeSplitter
{
  public:
	explicit DataSizeSplitter(unsigned int size)
	    : size_(size)
	{}

	template <typename T> inline bool split(unsigned int count) const { return (count * sizeof(T) > size_); }

  private:
	unsigned int size_;
};

/// The parallel for job that either splits the worlkoad in two or executes it, based on a `Splitter` policy class
template <typename JobData>
void parallelForJob(JobId job, const void *jobData)
{
	const JobData *data = static_cast<const JobData*>(jobData);
	const typename JobData::SplitterType& splitter = data->splitter;

	if (splitter.template split<typename JobData::DataType>(data->count))
	{
		IJobSystem &jobSystem = theServiceLocator().jobSystem();

		// split in two
		const unsigned int leftCount = data->count / 2u;
		const JobData leftData(data->data, leftCount, data->function, splitter);
		JobId left = jobSystem.createJobAsChild(job, &parallelForJob<JobData>, &leftData, sizeof(JobData));
		jobSystem.run(left);

		const unsigned int rightCount = data->count - leftCount;
		const JobData rightData(data->data + leftCount, rightCount, data->function, splitter);
		JobId right = jobSystem.createJobAsChild(job, &parallelForJob<JobData>, &rightData, sizeof(JobData));
		jobSystem.run(right);
	}
	else
	{
		// execute the function on the range of data
		(data->function)(data->data, data->count);
	}
}

/// The data for the parallel for job
template <typename T, typename S>
struct parallelForJobData
{
	typedef T DataType;
	typedef S SplitterType;

	parallelForJobData(DataType *data, unsigned int count, void (*function)(DataType *, unsigned int), const SplitterType &splitter)
	    : data(data), count(count), function(function), splitter(splitter)
	{}

	DataType *data;
	unsigned int count;
	void (*function)(DataType *, unsigned int);
	SplitterType splitter;
};

/// Creates a parallel job to automatically divide data processing
template <typename T, typename S>
JobId parallelFor(T *data, unsigned int count, void (*function)(T *, unsigned int), const S &splitter)
{
	typedef parallelForJobData<T, S> JobData;
	const JobData jobData(data, count, function, splitter);

	IJobSystem &jobSystem = theServiceLocator().jobSystem();
	JobId job = jobSystem.createJob(&parallelForJob<JobData>, &jobData, sizeof(JobData));

	return job;
}

}

#endif
