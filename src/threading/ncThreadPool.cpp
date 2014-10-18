#include <cstdlib> // for exit()
#include "ncThreadPool.h"
#include "ncThreadCommands.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Creates a thread pool with as many threads as available processors
ncThreadPool::ncThreadPool()
	: threads_(NULL)
{
	numThreads_ = ncThread::numProcessors();
	init();
}

/// Creates a thread pool with a specified number of threads
ncThreadPool::ncThreadPool(unsigned int numThreads)
	: threads_(NULL), numThreads_(numThreads)
{
	init();
}

ncThreadPool::~ncThreadPool()
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
void ncThreadPool::enqueueCommand(ncIThreadCommand *pThreadCommand)
{
	queueMutex_.lock();
	queue_.insertBack(pThreadCommand);
	queueCV_.broadcast();
	queueMutex_.unlock();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncThreadPool::init()
{
	threads_ = new ncThread[numThreads_];

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

void ncThreadPool::workerFunction(void *arg)
{
	ncIThreadCommand *threadCommand = NULL;
	ncThreadStruct *threadStruct = static_cast<ncThreadStruct *>(arg);

	LOGD_X("worker thread %u is starting", ncThread::self());

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

		threadCommand = threadStruct->queue->removeFront();
		threadStruct->queueMutex->unlock();

		threadCommand->execute();
		delete threadCommand;
	}

	LOGD_X("worker thread %u is exiting", ncThread::self());
}
