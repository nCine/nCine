#include "ThreadPool.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ThreadPool::ThreadPool()
	: ThreadPool(Thread::numProcessors())
{

}

ThreadPool::ThreadPool(unsigned int numThreads)
	: threads_(numThreads, nctl::ArrayMode::FIXED_CAPACITY), numThreads_(numThreads)
{
	threadStruct_.queue = &queue_;
	threadStruct_.queueMutex = &queueMutex_;
	threadStruct_.queueCV = &queueCV_;
	threadStruct_.shouldQuit = false;

	quitMutex_.lock();

	for (unsigned int i = 0; i < numThreads_; i++)
	{
		threads_[i].run(workerFunction, &threadStruct_);
#ifndef __ANDROID__
		threads_[i].setAffinityMask(ThreadAffinityMask(i));
#endif
	}
}

ThreadPool::~ThreadPool()
{
	threadStruct_.shouldQuit = true;
	queueCV_.broadcast();

	for (unsigned int i = 0; i < numThreads_; i++)
		threads_[i].join();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ThreadPool::enqueueCommand(nctl::UniquePtr<IThreadCommand> threadCommand)
{
	ASSERT(threadCommand);

	queueMutex_.lock();
	queue_.pushBack(nctl::move(threadCommand));
	queueCV_.broadcast();
	queueMutex_.unlock();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ThreadPool::workerFunction(void *arg)
{
	ThreadStruct *threadStruct = static_cast<ThreadStruct *>(arg);

	LOGD_X("Worker thread %u is starting", Thread::self());

	while (true)
	{
		threadStruct->queueMutex->lock();
		while (threadStruct->queue->isEmpty() && threadStruct->shouldQuit == false)
			threadStruct->queueCV->wait(*(threadStruct->queueMutex));

		if (threadStruct->shouldQuit)
		{
			threadStruct->queueMutex->unlock();
			break;
		}

		nctl::UniquePtr<IThreadCommand> threadCommand = nctl::move(threadStruct->queue->front());
		threadStruct->queue->popFront();
		threadStruct->queueMutex->unlock();

		LOGD_X("Worker thread %u is executing its command", Thread::self());
		threadCommand->execute();
	}

	LOGD_X("Worker thread %u is exiting", Thread::self());
}

}
