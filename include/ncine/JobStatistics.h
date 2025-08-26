#ifndef CLASS_NCINE_JOBSTATISTICS
#define CLASS_NCINE_JOBSTATISTICS

#include "common_defines.h"
#include <cstdint>

namespace ncine {

/// A class to collect statistics counters of the job system
class DLL_PUBLIC JobStatistics
{
  public:
	// Align to a cache line to avoid false sharing between different threads
	struct alignas(64) JobSystemStats
	{
		uint32_t jobsCreated = 0;
		uint32_t childJobsCreated = 0;
		uint32_t jobsStolen = 0;
		uint32_t jobsExecuted = 0;
		uint32_t jobsFinished = 0;
		uint32_t childJobsFinished = 0;
		uint32_t continuationJobsPushed = 0;
		uint32_t parentJobsFinished = 0;

		void incrementJobsCreated();
		void incrementChildJobsCreated();
		void incrementJobsStolen();
		void incrementJobsExecuted();
		void incrementJobsFinished();
		void incrementChildJobsFinished();
		void incrementContinuationJobsPushed();
		void incrementParentJobsFinished();

		void add(const JobSystemStats &other);
		void reset();
	};

	// Align to a cache line to avoid false sharing between different threads
	struct alignas(64) JobPoolStats
	{
		uint32_t jobsAllocated = 0;
		uint32_t jobAllocationFails = 0;
		uint32_t jobsFreed = 0;
		uint32_t jobsRetrieved = 0;
		uint32_t jobRetrievalFails = 0;
		uint32_t threadCacheEmpty = 0;
		uint32_t threadCacheFull = 0;

		void incrementJobsAllocated();
		void incrementJobAllocationFails();
		void incrementJobsFreed();
		void incrementJobsRetrieved();
		void incrementJobRetrievalFails();
		void incrementThreadCacheEmpty();
		void incrementThreadCacheFull();

		void add(const JobPoolStats &other);
		void reset();
	};

	// Align to a cache line to avoid false sharing between different threads
	struct alignas(64) JobQueueStats
	{
		uint32_t pushes = 0;
		uint32_t pops = 0;
		uint32_t popFails = 0;
		uint32_t steals = 0;
		uint32_t stealFails = 0;

		void incrementPushes();
		void incrementPops();
		void incrementPopFails();
		void incrementSteals();
		void incrementStealFails();

		void add(const JobQueueStats &other);
		void reset();
	};

	/// Used by the job system to give access to statistics to all functions in anonymous namespace
	class JobSystemStatsHelper
	{
	  public:
		JobSystemStats &jobSystemStatsMut();

	  private:
		/// Private constructor
		JobSystemStatsHelper() {}
		friend class JobStatistics;
	};

	JobStatistics();

	/// Returns `true` is the system is initialized and working
	inline bool isAvailable() const { return numThreads_ > 0; }

	/// Returns the job system statistics for the specified thread
	const JobSystemStats &jobSystemStats(unsigned char index) const;
	/// Collects the job system statistics for all threads
	void collectAllJobSystemStats(JobSystemStats &totalStats) const;
	/// Resets the job system statistics for the specified thread
	void resetJobSystemStats(unsigned char index);
	/// Resets the job system statistics for all threads
	void resetAllJobSystemStats();

	/// Returns the job pool statistics for the specified thread
	const JobPoolStats &jobPoolStats(unsigned char index) const;
	/// Collects the job pool statistics for all threads
	void collectAllJobPoolStats(JobPoolStats &totalStats) const;
	/// Resets the job pool statistics for the specified thread
	void resetJobPoolStats(unsigned char index);
	/// Resets the job pool statistics for all threads
	void resetAllJobPoolStats();

	/// Returns the job queue statistics for the specified thread
	const JobQueueStats &jobQueueStats(unsigned char index) const;
	/// Collects the job queue statistics for all threads
	void collectAllJobQueueStats(JobQueueStats &totalStats) const;
	/// Resets the job queue statistics for the specified thread
	void resetJobQueueStats(unsigned char index);
	/// Resets the job queue statistics for all threads
	void resetAllJobQueueStats();

  private:
	unsigned char numThreads_;

	/// Called the job system constructor with the maximum number of available threads
	void initialize();
	/// Called the job system constructor with the specified number of threads
	void initialize(unsigned char numThreads);

	JobSystemStatsHelper *jobSystemStatsHelper();
	JobSystemStats &jobSystemStatsMut();
	JobPoolStats &jobPoolStatsMut();
	JobQueueStats &jobQueueStatsMut();

	friend class JobSystemStatsHelper;
	friend class JobSystem;
	friend class SerialJobSystem;
	friend class JobPool;
	friend class JobQueue;
	friend class Application;
};

// Meyers' Singleton
extern DLL_PUBLIC JobStatistics &theJobStatistics();

}

#endif
