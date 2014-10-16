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
  public:
	// Creates a thread pool with as many threads as available processors
	ncThreadPool();
	// Creates a thread pool with a specified number of threads
	ncThreadPool(unsigned int numThreads);
	~ncThreadPool();

	// Enqueues a command request for a worker thread
	void enqueueCommand(ncIThreadCommand *threadCommand);

  private:
	struct ncThreadStruct
	{
		ncList<ncIThreadCommand *> *queue;
		ncMutex *queueMutex;
		ncCondVariable *queueCV;
		bool shouldQuit;
	};

	ncList<ncIThreadCommand *> queue_;
	ncThread *threads_;
	ncMutex queueMutex_;
	ncCondVariable queueCV_;
	ncMutex quitMutex_;
	unsigned int numThreads_;

	ncThreadStruct threadStruct_;
	static void workerFunction(void *arg);

	void init();
};

#endif
