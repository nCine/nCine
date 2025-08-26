#ifndef NCINE_JOBSYSTEM_DEBUG_DEFINES
#define NCINE_JOBSYSTEM_DEBUG_DEFINES

///////////////////////////////////////////////////////////
// INSTRUMENTATION DEFINES
///////////////////////////////////////////////////////////

/// Job system, queue, and pool statistics counters
#define JOB_DEBUG_COUNTERS (1 && NCINE_DEBUG)

/// Tracy plots of statistics counters
#define JOB_DEBUG_COUNTERS_TRACY_PLOT (0 && JOB_DEBUG_COUNTERS)

/// Queue and pool operations logs
#define JOB_DEBUG_LIFECYCLE_LOGGING (0 && NCINE_DEBUG)

/// Parent/child relationship logs
#define JOB_DEBUG_DEPENDENCIES_LOGGING (0 && NCINE_DEBUG)

/// State transitions and atomic checks
#define JOB_DEBUG_STATE (0 && NCINE_DEBUG)

///////////////////////////////////////////////////////////
// LOGGING MACRO WRAPPERS
///////////////////////////////////////////////////////////

#if JOB_DEBUG_LIFECYCLE_LOGGING
	#define JOB_LOG_QUEUE_PUSH(jobId, idx, gen, btm) \
		LOGI_X("[QUEUE] PUSH: jobId=%u idx=%u gen=%u bottom=%d", jobId, idx, gen, btm)
	#define JOB_LOG_QUEUE_POP(jobId, idx, gen, btm, top) \
		LOGI_X("[QUEUE] POP: jobId=%u idx=%u gen=%u bottom=%d top=%d", jobId, idx, gen, btm, top)
	#define JOB_LOG_QUEUE_STEAL(jobId, idx, gen, btm, top) \
		LOGI_X("[QUEUE] STEAL: jobId=%u idx=%u gen=%u bottom=%d top=%d", jobId, idx, gen, btm, top)
	#define JOB_LOG_POOL_FREE(jobId, idx, gen) \
		LOGI_X("[POOL] FREE: jobId=%u idx=%u gen=%u", jobId, idx, gen)
	#define JOB_LOG_POOL_ALLOC(jobId, idx, gen) \
		LOGI_X("[POOL] ALLOC: jobId=%u idx=%u gen=%u", jobId, idx, gen)
#else
	#define JOB_LOG_QUEUE_PUSH(jobId, idx, gen, btm) ((void)0)
	#define JOB_LOG_QUEUE_POP(jobId, idx, gen, btm, top) ((void)0)
	#define JOB_LOG_QUEUE_STEAL(jobId, idx, gen, btm, top) ((void)0)
	#define JOB_LOG_POOL_FREE(jobId, idx, gen) ((void)0)
	#define JOB_LOG_POOL_ALLOC(jobId, idx, gen) ((void)0)
#endif

#if JOB_DEBUG_DEPENDENCIES_LOGGING
	#define JOB_LOG_DEP(msg, ...) LOGI_X("[DEP] " msg, ##__VA_ARGS__)
#else
	#define JOB_LOG_DEP(msg, ...) ((void)0)
#endif

///////////////////////////////////////////////////////////
// JOB STATE TRANSITION FUNCTIONS
///////////////////////////////////////////////////////////

namespace ncine {

class JobPool;
struct Job;

#if JOB_DEBUG_STATE
void jobStateFreeToPushed(JobPool *jobPool, unsigned int jobId);
void jobStatePushedToPopped(JobPool *jobPool, unsigned int jobId);
void jobStateFreeToPushed(Job *job, unsigned int jobId);
void jobStatePushedToPopped(Job *job, unsigned int jobId);
void jobStateFinishedToFree(Job *job, unsigned int jobId);
void jobStatePoppedToExcuting(Job *job, unsigned int jobId);
void jobStateExcutingToFinished(Job *job, unsigned int jobId);
#else
inline void jobStateFreeToPushed(JobPool *jobPool, unsigned int jobId) {}
inline void jobStatePushedToPopped(JobPool *jobPool, unsigned int jobId) {}
inline void jobStateFreeToPushed(Job *job, unsigned int jobId) {}
inline void jobStatePushedToPopped(Job *job, unsigned int jobId) {}
inline void jobStateFinishedToFree(Job *job, unsigned int jobId) {}
inline void jobStatePoppedToExcuting(Job *job, unsigned int jobId) {}
inline void jobStateExcutingToFinished(Job *job, unsigned int jobId) {}
#endif

}

#endif
