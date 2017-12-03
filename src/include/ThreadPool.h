#ifndef CLASS_NCINE_THREADPOOL
#define CLASS_NCINE_THREADPOOL

#include "IThreadPool.h"
#include "nctl/List.h"
#include "ThreadSync.h"

namespace ncine {

class Thread;
class IThreadCommand;

/// Thread pool class
class ThreadPool : public IThreadPool
{
  public:
	/// Creates a thread pool with as many threads as available processors
	ThreadPool();
	/// Creates a thread pool with a specified number of threads
	explicit ThreadPool(unsigned int numThreads);
	~ThreadPool() override;

	/// Enqueues a command request for a worker thread
	void enqueueCommand(IThreadCommand *threadCommand) override;

  private:
	struct ThreadStruct
	{
		nctl::List<IThreadCommand *> *queue;
		Mutex *queueMutex;
		CondVariable *queueCV;
		bool shouldQuit;
	};

	nctl::List<IThreadCommand *> queue_;
	Thread *threads_;
	Mutex queueMutex_;
	CondVariable queueCV_;
	Mutex quitMutex_;
	unsigned int numThreads_;

	ThreadStruct threadStruct_;
	static void workerFunction(void *arg);

	/// Deleted copy constructor
	ThreadPool(const ThreadPool &) = delete;
	/// Deleted assignment operator
	ThreadPool &operator=(const ThreadPool &) = delete;
};

}

#endif
