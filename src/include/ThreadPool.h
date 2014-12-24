#ifndef CLASS_NCINE_THREADPOOL
#define CLASS_NCINE_THREADPOOL

#include "IThreadPool.h"
#include "List.h"
#include "ThreadSync.h"

namespace ncine {

class Thread;
class IThreadCommand;

/// Thread pool class
class ThreadPool : public IThreadPool
{
  public:
	// Creates a thread pool with as many threads as available processors
	ThreadPool();
	// Creates a thread pool with a specified number of threads
	explicit ThreadPool(unsigned int numThreads);
	~ThreadPool();

	// Enqueues a command request for a worker thread
	void enqueueCommand(IThreadCommand *threadCommand);

  private:
	struct ThreadStruct
	{
		List<IThreadCommand *> *queue;
		Mutex *queueMutex;
		CondVariable *queueCV;
		bool shouldQuit;
	};

	List<IThreadCommand *> queue_;
	Thread *threads_;
	Mutex queueMutex_;
	CondVariable queueCV_;
	Mutex quitMutex_;
	unsigned int numThreads_;

	ThreadStruct threadStruct_;
	static void workerFunction(void *arg);

	/// Private copy constructor
	ThreadPool(const ThreadPool&);
	/// Private assignment operator
	ThreadPool& operator=(const ThreadPool&);

	void init();
};

}

#endif
