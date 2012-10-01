#ifndef CLASS_NCTHREADPOOL
#define CLASS_NCTHREADPOOL

#include "ncIThreadPool.h"
#include "ncList.h"
#include "ncThreadSync.h"
class ncThread;
class ncIThreadCommand;

/// Thread pool class
class ncThreadPool : public ncIThreadPool
{
private:
	struct ncThreadStruct
	{
		ncList<ncIThreadCommand *> *pQueue;
		ncMutex *pQueueMutex;
		ncCondVariable *pQueueCV;
		bool bShouldQuit;
	};

	ncList<ncIThreadCommand *> m_queue;
	ncThread *m_pThreads;
	ncMutex m_queueMutex;
	ncCondVariable m_queueCV;
	ncMutex m_quitMutex;
	unsigned int m_uNumThreads;

	ncThreadStruct m_threadStruct;
	static void WorkerFunction(void *pArg);

	void Init();
public:
	// Creates a thread pool with as many threads as available processors
	ncThreadPool();
	// Creates a thread pool with a specified number of threads
	ncThreadPool(unsigned int uNumThreads);
	~ncThreadPool();

	// Enqueues a command request for a worker thread
	void EnqueueCommand(ncIThreadCommand *pThreadCommand);
};

#endif
