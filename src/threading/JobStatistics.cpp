#include "JobStatistics.h"
#include "Thread.h"
#include "jobsystem_debug.h"

#define JOBSYSTEM_STATISTICS (JOB_DEBUG_COUNTERS)
#define JOBQUEUE_STATISTICS (JOB_DEBUG_COUNTERS)
#define JOBPOOL_STATISTICS (JOB_DEBUG_COUNTERS)

#define JOBSYSTEM_TRACY_PLOTS (JOB_DEBUG_COUNTERS_TRACY_PLOT)
#define JOBQUEUE_TRACY_PLOTS (JOB_DEBUG_COUNTERS_TRACY_PLOT)
#define JOBPOOL_TRACY_PLOTS (JOB_DEBUG_COUNTERS_TRACY_PLOT)

#if defined(WITH_TRACY) || NCINE_WITH_TRACY
	#define HAVE_TRACY_ENABLED 1
#else
	#define HAVE_TRACY_ENABLED 0
#endif

#define WITH_JOBSYSTEM_TRACY_PLOTS (HAVE_TRACY_ENABLED && JOBSYSTEM_STATISTICS && JOBSYSTEM_TRACY_PLOTS)
#define WITH_JOBPOOL_TRACY_PLOTS (HAVE_TRACY_ENABLED && JOBPOOL_STATISTICS && JOBPOOL_TRACY_PLOTS)
#define WITH_JOBQUEUE_TRACY_PLOTS (HAVE_TRACY_ENABLED && JOBQUEUE_STATISTICS && JOBQUEUE_TRACY_PLOTS)

#define WITH_ANYJOB_STATISTICS (JOBSYSTEM_STATISTICS || JOBPOOL_STATISTICS || JOBQUEUE_STATISTICS)
#define WITH_ANYJOB_TRACY_PLOTS (WITH_JOBSYSTEM_TRACY_PLOTS || WITH_JOBPOOL_TRACY_PLOTS || WITH_JOBQUEUE_TRACY_PLOTS)

#if WITH_ANYJOB_STATISTICS
	#include <nctl/Array.h>
#endif

#if WITH_ANYJOB_TRACY_PLOTS
	#include "tracy.h"
	#include <nctl/StaticString.h>
#endif

namespace ncine {

#if JOB_DEBUG_COUNTERS
nctl::Array<JobStatistics::JobSystemStats> jobSystemStatsArray;
nctl::Array<JobStatistics::JobPoolStats> jobPoolStatsArray;
nctl::Array<JobStatistics::JobQueueStats> jobQueueStatsArray;
#else
JobStatistics::JobSystemStats jobSystemStatsDummy;
JobStatistics::JobPoolStats jobPoolStatsDummy;
JobStatistics::JobQueueStats jobQueueStatsDummy;
#endif

JobStatistics &theJobStatistics()
{
	static JobStatistics instance;
	return instance;
}

namespace {
#if WITH_ANYJOB_TRACY_PLOTS
	namespace PlotNameFormat
	{
	#if WITH_JOBSYSTEM_TRACY_PLOTS
		char const * const JobsCreated = "JobsCreated#%02d";
		char const * const ChildJobsCreated = "ChildJobsCreated#%02d";
		char const * const JobsStolen = "JobsStolen#%02d";
		char const * const JobsExecuted = "JobsExecuted#%02d";
		char const * const JobsFinished = "JobsFinished#%02d";
		char const * const ChildJobsFinished = "ChildJobsFinished#%02d";
		char const * const ContinuationJobsPushed = "ContinuationJobsPushed#%02d";
		char const * const ParentJobsFinished = "ParentJobsFinished#%02d";
	#endif

	#if WITH_JOBPOOL_TRACY_PLOTS
		char const * const JobsAllocatedPool = "JobsAllocatedPool#%02d";
		char const * const JobAllocationFailsPool = "JobAllocationFailsPool#%02d";
		char const * const JobsFreedPool = "JobsFreedPool#%02d";
		char const * const JobsRetrievedPool = "JobsRetrievedPool#%02d";
		char const * const JobRetrievalFailsPool = "JobRetrievalFails#%02d";
		char const * const ThreadCacheEmptyPool = "ThreadCacheEmpty#%02d";
		char const * const ThreadCacheFullPool = "ThreadCacheFull#%02d";
	#endif

	#if WITH_JOBQUEUE_TRACY_PLOTS
		char const * const PushesQueue = "PushesQueue#%02d";
		char const * const PopsQueue = "PopsQueue#%02d";
		char const * const PopFailsQueue = "PopFailsQueue#%02d";
		char const * const StealsQueue = "StealsQueue#%02d";
		char const * const StealFailsQueue = "StealFailsQueue#%02d";
	#endif
	}

	#if WITH_JOBSYSTEM_TRACY_PLOTS
	nctl::Array<nctl::StaticString<32>> jobsCreatedPlotStrings;
	nctl::Array<nctl::StaticString<32>> childJobsCreatedPlotStrings;
	nctl::Array<nctl::StaticString<32>> jobsStolenPlotStrings;
	nctl::Array<nctl::StaticString<32>> jobsExecutedPlotStrings;
	nctl::Array<nctl::StaticString<32>> jobsFinishedPlotStrings;
	nctl::Array<nctl::StaticString<32>> childJobsFinishedPlotStrings;
	nctl::Array<nctl::StaticString<32>> continuationJobsPushedPlotStrings;
	nctl::Array<nctl::StaticString<32>> parentJobsFinishedPlotStrings;
	#endif

	#if WITH_JOBPOOL_TRACY_PLOTS
	nctl::Array<nctl::StaticString<32>> jobsAllocatedPoolPlotStrings;
	nctl::Array<nctl::StaticString<32>> jobAllocationFailsPoolPlotStrings;
	nctl::Array<nctl::StaticString<32>> jobsFreedPoolPlotStrings;
	nctl::Array<nctl::StaticString<32>> jobsRetrievedPoolPlotStrings;
	nctl::Array<nctl::StaticString<32>> jobRetrievalFailsPoolPlotStrings;
	nctl::Array<nctl::StaticString<32>> threadCacheEmptyPoolPlotStrings;
	nctl::Array<nctl::StaticString<32>> threadCacheFullPoolPlotStrings;
	#endif

	#if WITH_JOBQUEUE_TRACY_PLOTS
	nctl::Array<nctl::StaticString<32>> pushesQueuePlotStrings;
	nctl::Array<nctl::StaticString<32>> popsQueuePlotStrings;
	nctl::Array<nctl::StaticString<32>> popFailsQueuePlotStrings;
	nctl::Array<nctl::StaticString<32>> stealsQueuePlotStrings;
	nctl::Array<nctl::StaticString<32>> stealFailsQueuePlotStrings;
	#endif
#endif
}

//////////////////////////////////////////////////////////
// `JobSystemStats`
///////////////////////////////////////////////////////////

#if JOBSYSTEM_STATISTICS
void JobStatistics::JobSystemStats::incrementJobsCreated()
{
	jobsCreated++;
	#if WITH_JOBSYSTEM_TRACY_PLOTS
	TracyPlot(jobsCreatedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsCreated));
	#endif
}

void JobStatistics::JobSystemStats::incrementChildJobsCreated()
{
	childJobsCreated++;
	#if WITH_JOBSYSTEM_TRACY_PLOTS
	TracyPlot(childJobsCreatedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsCreated));
	#endif
}

void JobStatistics::JobSystemStats::incrementJobsStolen()
{
	jobsStolen++;
	#if WITH_JOBSYSTEM_TRACY_PLOTS
	TracyPlot(jobsStolenPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsStolen));
	#endif
}

void JobStatistics::JobSystemStats::incrementJobsExecuted()
{
	jobsExecuted++;
	#if WITH_JOBSYSTEM_TRACY_PLOTS
	TracyPlot(jobsExecutedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsExecuted));
	#endif
}

void JobStatistics::JobSystemStats::incrementJobsFinished()
{
	jobsFinished++;
	#if WITH_JOBSYSTEM_TRACY_PLOTS
	TracyPlot(jobsFinishedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsFinished));
	#endif
}

void JobStatistics::JobSystemStats::incrementChildJobsFinished()
{
	childJobsFinished++;
	#if WITH_JOBSYSTEM_TRACY_PLOTS
	TracyPlot(childJobsFinishedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(childJobsFinished));
	#endif
}

void JobStatistics::JobSystemStats::incrementContinuationJobsPushed()
{
	continuationJobsPushed++;
	#if WITH_JOBSYSTEM_TRACY_PLOTS
	TracyPlot(continuationJobsPushedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(continuationJobsPushed));
	#endif
}

void JobStatistics::JobSystemStats::incrementParentJobsFinished()
{
	parentJobsFinished++;
	#if WITH_JOBSYSTEM_TRACY_PLOTS
	TracyPlot(parentJobsFinishedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(parentJobsFinished));
	#endif
}
#else
// Empty functions that are completely optimized-out in release when the define is not set
void JobStatistics::JobSystemStats::incrementJobsCreated() {}
void JobStatistics::JobSystemStats::incrementChildJobsCreated() {}
void JobStatistics::JobSystemStats::incrementJobsStolen() {}
void JobStatistics::JobSystemStats::incrementJobsExecuted() {}
void JobStatistics::JobSystemStats::incrementJobsFinished() {}
void JobStatistics::JobSystemStats::incrementChildJobsFinished() {}
void JobStatistics::JobSystemStats::incrementContinuationJobsPushed() {}
void JobStatistics::JobSystemStats::incrementParentJobsFinished() {}
#endif

void JobStatistics::JobSystemStats::add(const JobSystemStats &other)
{
	jobsCreated += other.jobsCreated;
	childJobsCreated += other.childJobsCreated;
	jobsStolen += other.jobsStolen;
	jobsExecuted += other.jobsExecuted;
	jobsFinished += other.jobsFinished;
	childJobsFinished += other.childJobsFinished;
	continuationJobsPushed += other.continuationJobsPushed;
	parentJobsFinished += other.parentJobsFinished;
}

void JobStatistics::JobSystemStats::reset()
{
	jobsCreated = 0;
	childJobsCreated = 0;
	jobsStolen = 0;
	jobsExecuted = 0;
	jobsFinished = 0;
	childJobsFinished = 0;
	continuationJobsPushed = 0;
	parentJobsFinished = 0;

#if WITH_JOBSYSTEM_TRACY_PLOTS
	TracyPlot(jobsCreatedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsCreated));
	TracyPlot(childJobsCreatedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(childJobsCreated));
	TracyPlot(jobsStolenPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsStolen));
	TracyPlot(jobsExecutedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsExecuted));
	TracyPlot(jobsFinishedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsFinished));
	TracyPlot(childJobsFinishedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(childJobsFinished));
	TracyPlot(continuationJobsPushedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(continuationJobsPushed));
	TracyPlot(parentJobsFinishedPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(parentJobsFinished));
#endif
}

JobStatistics::JobSystemStats &JobStatistics::JobSystemStatsHelper::jobSystemStatsMut()
{
	return theJobStatistics().jobSystemStatsMut();
}

///////////////////////////////////////////////////////////
// `JobPoolStats`
///////////////////////////////////////////////////////////

#if JOBPOOL_STATISTICS
void JobStatistics::JobPoolStats::incrementJobsAllocated()
{
	jobsAllocated++;
	#if WITH_JOBPOOL_TRACY_PLOTS
	TracyPlot(jobsAllocatedPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsAllocated));
	#endif
}

void JobStatistics::JobPoolStats::incrementJobAllocationFails()
{
	jobAllocationFails++;
	#if WITH_JOBPOOL_TRACY_PLOTS
	TracyPlot(jobAllocationFailsPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobAllocationFails));
	#endif
}

void JobStatistics::JobPoolStats::incrementJobsFreed()
{
	jobsFreed++;
	#if WITH_JOBPOOL_TRACY_PLOTS
	TracyPlot(jobsFreedPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsFreed));
	#endif
}

void JobStatistics::JobPoolStats::incrementJobsRetrieved()
{
	jobsRetrieved++;
	#if WITH_JOBPOOL_TRACY_PLOTS
	TracyPlot(jobsRetrievedPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsRetrieved));
	#endif
}

void JobStatistics::JobPoolStats::incrementJobRetrievalFails()
{
	jobRetrievalFails++;
	#if WITH_JOBPOOL_TRACY_PLOTS
	TracyPlot(jobRetrievalFailsPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobRetrievalFails));
	#endif
}

void JobStatistics::JobPoolStats::incrementThreadCacheEmpty()
{
	threadCacheEmpty++;
	#if WITH_JOBPOOL_TRACY_PLOTS
	TracyPlot(threadCacheEmptyPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(threadCacheEmpty));
	#endif
}

void JobStatistics::JobPoolStats::incrementThreadCacheFull()
{
	threadCacheFull++;
	#if WITH_JOBPOOL_TRACY_PLOTS
	TracyPlot(threadCacheFullPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(threadCacheFull));
	#endif
}
#else
// Empty functions that are completely optimized-out in release when the define is not set
void JobStatistics::JobPoolStats::incrementJobsAllocated() {}
void JobStatistics::JobPoolStats::incrementJobAllocationFails() {}
void JobStatistics::JobPoolStats::incrementJobsFreed() {}
void JobStatistics::JobPoolStats::incrementJobsRetrieved() {}
void JobStatistics::JobPoolStats::incrementJobRetrievalFails() {}
void JobStatistics::JobPoolStats::incrementThreadCacheEmpty() {}
void JobStatistics::JobPoolStats::incrementThreadCacheFull() {}
#endif

void JobStatistics::JobPoolStats::add(const JobPoolStats &other)
{
	jobsAllocated += other.jobsAllocated;
	jobAllocationFails += other.jobAllocationFails;
	jobsFreed += other.jobsFreed;
	jobsRetrieved += other.jobsRetrieved;
	jobRetrievalFails += other.jobRetrievalFails;
	threadCacheEmpty += other.threadCacheEmpty;
	threadCacheFull += other.threadCacheFull;
}

void JobStatistics::JobPoolStats::reset()
{
	jobsAllocated = 0;
	jobAllocationFails = 0;
	jobsFreed = 0;
	jobsRetrieved = 0;
	jobRetrievalFails = 0;
	threadCacheEmpty = 0;
	threadCacheFull = 0;

#if WITH_JOBPOOL_TRACY_PLOTS
	TracyPlot(jobsAllocatedPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsAllocated));
	TracyPlot(jobAllocationFailsPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobAllocationFails));
	TracyPlot(jobsFreedPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsFreed));
	TracyPlot(jobsRetrievedPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobsRetrieved));
	TracyPlot(jobRetrievalFailsPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(jobRetrievalFails));
	TracyPlot(threadCacheEmptyPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(threadCacheEmpty));
	TracyPlot(threadCacheFullPoolPlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(threadCacheFull));
#endif
}

///////////////////////////////////////////////////////////
// `JobQueueStats`
///////////////////////////////////////////////////////////

#if JOBQUEUE_STATISTICS
void JobStatistics::JobQueueStats::incrementPushes()
{
	pushes++;
	#if WITH_JOBQUEUE_TRACY_PLOTS
	TracyPlot(pushesQueuePlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(pushes));
	#endif
}

void JobStatistics::JobQueueStats::incrementPops()
{
	pops++;
	#if WITH_JOBQUEUE_TRACY_PLOTS
	TracyPlot(popsQueuePlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(pops));
	#endif
}

void JobStatistics::JobQueueStats::incrementPopFails()
{
	popFails++;
	#if WITH_JOBQUEUE_TRACY_PLOTS
	TracyPlot(popFailsQueuePlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(popFails));
	#endif
}

void JobStatistics::JobQueueStats::incrementSteals()
{
	steals++;
	#if WITH_JOBQUEUE_TRACY_PLOTS
	TracyPlot(stealsQueuePlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(steals));
	#endif
}

void JobStatistics::JobQueueStats::incrementStealFails()
{
	stealFails++;
	#if WITH_JOBQUEUE_TRACY_PLOTS
	TracyPlot(stealFailsQueuePlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(stealFails));
	#endif
}
#else
// Empty functions that are completely optimized-out in release when the define is not set
void JobStatistics::JobQueueStats::incrementPushes() {}
void JobStatistics::JobQueueStats::incrementPops() {}
void JobStatistics::JobQueueStats::incrementPopFails() {}
void JobStatistics::JobQueueStats::incrementSteals() {}
void JobStatistics::JobQueueStats::incrementStealFails() {}
#endif

void JobStatistics::JobQueueStats::add(const JobQueueStats &other)
{
	pushes += other.pushes;
	pops += other.pops;
	popFails += other.popFails;
	steals += other.steals;
	stealFails += other.stealFails;
}

void JobStatistics::JobQueueStats::reset()
{
	pushes = 0;
	pops = 0;
	popFails = 0;
	steals = 0;
	stealFails = 0;

#if WITH_JOBQUEUE_TRACY_PLOTS
	TracyPlot(pushesQueuePlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(pushes));
	TracyPlot(popsQueuePlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(pops));
	TracyPlot(popFailsQueuePlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(popFails));
	TracyPlot(stealsQueuePlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(steals));
	TracyPlot(stealFailsQueuePlotStrings[IJobSystem::threadIndex()].data(),
	          static_cast<int64_t>(stealFails));
#endif
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

JobStatistics::JobStatistics()
    : numThreads_(0)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

const JobStatistics::JobSystemStats &JobStatistics::jobSystemStats(unsigned char index) const
{
#if JOBSYSTEM_STATISTICS
	ASSERT(index < numThreads_);
	if (index > numThreads_ - 1)
		index = 0;

	FATAL_ASSERT(numThreads_ > 0);
	return jobSystemStatsArray[index];
#else
	return jobSystemStatsDummy;
#endif
}

void JobStatistics::collectAllJobSystemStats(JobSystemStats &totalStats) const
{
#if JOBSYSTEM_STATISTICS
	FATAL_ASSERT(numThreads_ > 0);
	// The passed structure is not cleared, the application can decide how to aggregate data
	for (unsigned char i = 0; i < numThreads_; i++)
		totalStats.add(jobSystemStatsArray[i]);
#endif
}

void JobStatistics::resetJobSystemStats(unsigned char index)
{
#if JOBSYSTEM_STATISTICS
	ASSERT(index < numThreads_);
	if (index > numThreads_ - 1)
		index = 0;

	FATAL_ASSERT(numThreads_ > 0);
	jobSystemStatsArray[index].reset();
#endif
}

void JobStatistics::resetAllJobSystemStats()
{
#if JOBSYSTEM_STATISTICS
	FATAL_ASSERT(numThreads_ > 0);
	for (unsigned char i = 0; i < numThreads_; i++)
		jobSystemStatsArray[i].reset();
#endif
}

const JobStatistics::JobPoolStats &JobStatistics::jobPoolStats(unsigned char index) const
{
#if JOBPOOL_STATISTICS
	ASSERT(index < numThreads_);
	if (index > numThreads_ - 1)
		index = 0;

	FATAL_ASSERT(numThreads_ > 0);
	return jobPoolStatsArray[index];
#else
	return jobPoolStatsDummy;
#endif
}

void JobStatistics::collectAllJobPoolStats(JobPoolStats &totalStats) const
{
#if JOBPOOL_STATISTICS
	FATAL_ASSERT(numThreads_ > 0);
	// The passed structure is not cleared, the application can decide how to aggregate data
	for (unsigned char i = 0; i < numThreads_; i++)
		totalStats.add(jobPoolStatsArray[i]);
#endif
}

void JobStatistics::resetJobPoolStats(unsigned char index)
{
#if JOBPOOL_STATISTICS
	ASSERT(index < numThreads_);
	if (index > numThreads_ - 1)
		index = 0;

	FATAL_ASSERT(numThreads_ > 0);
	jobPoolStatsArray[index].reset();
#endif
}

void JobStatistics::resetAllJobPoolStats()
{
#if JOBPOOL_STATISTICS
	FATAL_ASSERT(numThreads_ > 0);
	for (unsigned char i = 0; i < numThreads_; i++)
		jobPoolStatsArray[i].reset();
#endif
}

const JobStatistics::JobQueueStats &JobStatistics::jobQueueStats(unsigned char index) const
{
#if JOBQUEUE_STATISTICS
	ASSERT(index < numThreads_);
	if (index > numThreads_ - 1)
		index = 0;

	FATAL_ASSERT(numThreads_ > 0);
	return jobQueueStatsArray[index];
#else
	return jobQueueStatsDummy;
#endif
}

void JobStatistics::collectAllJobQueueStats(JobQueueStats &totalStats) const
{
#if JOBQUEUE_STATISTICS
	FATAL_ASSERT(numThreads_ > 0);
	// The passed structure is not cleared, the application can decide how to aggregate data
	for (unsigned char i = 0; i < numThreads_; i++)
		totalStats.add(jobQueueStatsArray[i]);
#endif
}

void JobStatistics::resetJobQueueStats(unsigned char index)
{
#if JOBQUEUE_STATISTICS
	ASSERT(index < numThreads_);
	if (index > numThreads_ - 1)
		index = 0;

	FATAL_ASSERT(numThreads_ > 0);
	jobQueueStatsArray[index].reset();
#endif
}

void JobStatistics::resetAllJobQueueStats()
{
#if JOBQUEUE_STATISTICS
	FATAL_ASSERT(numThreads_ > 0);
	for (unsigned char i = 0; i < numThreads_; i++)
		jobQueueStatsArray[i].reset();
#endif
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void JobStatistics::initialize()
{
#if WITH_ANYJOB_STATISTICS
	initialize(Thread::numProcessors());
#endif
}

void JobStatistics::initialize(unsigned char numThreads)
{
#if WITH_ANYJOB_STATISTICS
	FATAL_ASSERT(numThreads > 0);
	if (numThreads_ == 0)
	{
	#if JOBSYSTEM_STATISTICS
		jobSystemStatsArray.setSize(numThreads);
	#endif
	#if JOBPOOL_STATISTICS
		jobPoolStatsArray.setSize(numThreads);
	#endif
	#if JOBQUEUE_STATISTICS
		jobQueueStatsArray.setSize(numThreads);
	#endif

	#if WITH_JOBSYSTEM_TRACY_PLOTS
		jobsCreatedPlotStrings.setSize(numThreads);
		childJobsCreatedPlotStrings.setSize(numThreads);
		jobsStolenPlotStrings.setSize(numThreads);
		jobsExecutedPlotStrings.setSize(numThreads);
		jobsFinishedPlotStrings.setSize(numThreads);
		childJobsFinishedPlotStrings.setSize(numThreads);
		continuationJobsPushedPlotStrings.setSize(numThreads);
		parentJobsFinishedPlotStrings.setSize(numThreads);
		for (unsigned char i = 0; i < numThreads; i++)
		{
			jobsCreatedPlotStrings[i].format(PlotNameFormat::JobsCreated, i);
			childJobsCreatedPlotStrings[i].format(PlotNameFormat::ChildJobsCreated, i);
			jobsStolenPlotStrings[i].format(PlotNameFormat::JobsStolen, i);
			jobsExecutedPlotStrings[i].format(PlotNameFormat::JobsExecuted, i);
			jobsFinishedPlotStrings[i].format(PlotNameFormat::JobsFinished, i);
			childJobsFinishedPlotStrings[i].format(PlotNameFormat::ChildJobsFinished, i);
			continuationJobsPushedPlotStrings[i].format(PlotNameFormat::ContinuationJobsPushed, i);
			parentJobsFinishedPlotStrings[i].format(PlotNameFormat::ParentJobsFinished, i);
		}
	#endif

	#if WITH_JOBPOOL_TRACY_PLOTS
		jobsAllocatedPoolPlotStrings.setSize(numThreads);
		jobAllocationFailsPoolPlotStrings.setSize(numThreads);
		jobsFreedPoolPlotStrings.setSize(numThreads);
		jobsRetrievedPoolPlotStrings.setSize(numThreads);
		jobRetrievalFailsPoolPlotStrings.setSize(numThreads);
		threadCacheEmptyPoolPlotStrings.setSize(numThreads);
		threadCacheFullPoolPlotStrings.setSize(numThreads);
		for (unsigned char i = 0; i < numThreads; i++)
		{
			jobsAllocatedPoolPlotStrings[i].format(PlotNameFormat::JobsAllocatedPool, i);
			jobAllocationFailsPoolPlotStrings[i].format(PlotNameFormat::JobAllocationFailsPool, i);
			jobsFreedPoolPlotStrings[i].format(PlotNameFormat::JobsFreedPool, i);
			jobsRetrievedPoolPlotStrings[i].format(PlotNameFormat::JobsRetrievedPool, i);
			jobRetrievalFailsPoolPlotStrings[i].format(PlotNameFormat::JobRetrievalFailsPool, i);
			threadCacheEmptyPoolPlotStrings[i].format(PlotNameFormat::ThreadCacheEmptyPool, i);
			threadCacheFullPoolPlotStrings[i].format(PlotNameFormat::ThreadCacheFullPool, i);
		}
	#endif

	#if WITH_JOBQUEUE_TRACY_PLOTS
		pushesQueuePlotStrings.setSize(numThreads);
		popsQueuePlotStrings.setSize(numThreads);
		popFailsQueuePlotStrings.setSize(numThreads);
		stealsQueuePlotStrings.setSize(numThreads);
		stealFailsQueuePlotStrings.setSize(numThreads);
		for (unsigned char i = 0; i < numThreads; i++)
		{
			pushesQueuePlotStrings[i].format(PlotNameFormat::PushesQueue, i);
			popsQueuePlotStrings[i].format(PlotNameFormat::PopsQueue, i);
			popFailsQueuePlotStrings[i].format(PlotNameFormat::PopFailsQueue, i);
			stealsQueuePlotStrings[i].format(PlotNameFormat::StealsQueue, i);
			stealFailsQueuePlotStrings[i].format(PlotNameFormat::StealFailsQueue, i);
		}
	#endif

		numThreads_ = numThreads;
	}
#endif
}

JobStatistics::JobSystemStatsHelper *JobStatistics::jobSystemStatsHelper()
{
	static JobSystemStatsHelper instance;
	return &instance;
}

JobStatistics::JobSystemStats &JobStatistics::jobSystemStatsMut()
{
#if JOBQUEUE_STATISTICS
	return jobSystemStatsArray[IJobSystem::threadIndex()];
#else
	return jobSystemStatsDummy;
#endif
}

JobStatistics::JobPoolStats &JobStatistics::jobPoolStatsMut()
{
#if JOBPOOL_STATISTICS
	return jobPoolStatsArray[IJobSystem::threadIndex()];
#else
	return jobPoolStatsDummy;
#endif
}

JobStatistics::JobQueueStats &JobStatistics::jobQueueStatsMut()
{
#if JOBQUEUE_STATISTICS
	return jobQueueStatsArray[IJobSystem::threadIndex()];
#else
	return jobQueueStatsDummy;
#endif
}

}
