#include <cstdlib> // for exit()
#include "ThreadPool.h"
#include "Thread.h"
#include "IThreadCommand.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Creates a thread pool with as many threads as available processors
ThreadPool::ThreadPool()
	: threads_(NULL)
{
	numThreads_ = Thread::numProcessors();
	init();
}

/// Creates a thread pool with a specified number of threads
ThreadPool::ThreadPool(unsigned int numThreads)
	: threads_(NULL), numThreads_(numThreads)
{
	init();
}

ThreadPool::~ThreadPool()
{
	threadStruct_.shouldQuit = true;
	queueCV_.broadcast();

	for (unsigned int i = 0; i < numThreads_; i++)
	{
		threads_[i].join();
	}

	delete[] threads_;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Enqueues a command request for a worker thread
/*! The command should be allocated on the heap for the worker thread to release it after its execution */
void ThreadPool::enqueueCommand(IThreadCommand *pThreadCommand)
{
	queueMutex_.lock();
	queue_.pushBack(pThreadCommand);
	queueCV_.broadcast();
	queueMutex_.unlock();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ThreadPool::init()
{
	threads_ = new Thread[numThreads_];

	threadStruct_.queue = &queue_;
	threadStruct_.queueMutex = &queueMutex_;
	threadStruct_.queueCV = &queueCV_;
	threadStruct_.shouldQuit = false;

	quitMutex_.lock();

	for (unsigned int i = 0; i < numThreads_; i++)
	{
		threads_[i].run(workerFunction, &threadStruct_);
	}
}

void ThreadPool::workerFunction(void *arg)
{
	ThreadStruct *threadStruct = static_cast<ThreadStruct *>(arg);

	LOGD_X("worker thread %u is starting", Thread::self());

	while (true)
	{
		threadStruct->queueMutex->lock();
		while (threadStruct->queue->isEmpty() && threadStruct->shouldQuit == false)
		{
			threadStruct->queueCV->wait(*(threadStruct->queueMutex));
		}

		if (threadStruct->shouldQuit)
		{
			threadStruct->queueMutex->unlock();
			break;
		}

		IThreadCommand *threadCommand = threadStruct->queue->front();
		threadStruct->queue->popFront();
		threadStruct->queueMutex->unlock();

		LOGD_X("worker thread %u is executing its command", Thread::self());
		threadCommand->execute();
		delete threadCommand;
	}

	LOGD_X("worker thread %u is exiting", Thread::self());
}

}
