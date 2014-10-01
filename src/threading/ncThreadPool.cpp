#include <cstdlib> // for exit()
#include "ncThreadPool.h"
#include "ncThreadCommands.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Creates a thread pool with as many threads as available processors
ncThreadPool::ncThreadPool()
	: m_pThreads(NULL)
{
	m_uNumThreads = ncThread::NumProcessors();
	Init();
}

/// Creates a thread pool with a specified number of threads
ncThreadPool::ncThreadPool(unsigned int uNumThreads)
	: m_pThreads(NULL), m_uNumThreads(uNumThreads)
{
	Init();
}

ncThreadPool::~ncThreadPool()
{
	m_threadStruct.bShouldQuit = true;
	m_queueCV.Broadcast();

	for (unsigned int i = 0; i < m_uNumThreads; i++)
	{
		m_pThreads[i].Join();
	}

	delete[] m_pThreads;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Enqueues a command request for a worker thread
/*! The command should be allocated on the heap for the worker thread to release it after its execution */
void ncThreadPool::EnqueueCommand(ncIThreadCommand *pThreadCommand)
{
	m_queueMutex.Lock();
	m_queue.InsertBack(pThreadCommand);
	m_queueCV.Broadcast();
	m_queueMutex.Unlock();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncThreadPool::Init()
{
	m_pThreads = new ncThread[m_uNumThreads];

	m_threadStruct.pQueue = &m_queue;
	m_threadStruct.pQueueMutex = &m_queueMutex;
	m_threadStruct.pQueueCV = &m_queueCV;
	m_threadStruct.bShouldQuit = false;

	m_quitMutex.Lock();

	for (unsigned int i = 0; i < m_uNumThreads; i++)
	{
		m_pThreads[i].Run(WorkerFunction, &m_threadStruct);
	}
}

void ncThreadPool::WorkerFunction(void *pArg)
{
	ncIThreadCommand *pThreadCommand = NULL;
	ncThreadStruct *pThreadStruct = static_cast<ncThreadStruct *>(pArg);

	ncServiceLocator::Logger().Write(ncILogger::LOG_DEBUG, (const char *)"ncThreadPool::WorkerFunction - worker thread %u is starting", ncThread::Self());

	while (true)
	{
		pThreadStruct->pQueueMutex->Lock();
		while (pThreadStruct->pQueue->isEmpty() && pThreadStruct->bShouldQuit == false)
		{
			pThreadStruct->pQueueCV->Wait(*(pThreadStruct->pQueueMutex));
		}

		if (pThreadStruct->bShouldQuit)
		{
			pThreadStruct->pQueueMutex->Unlock();
			break;
		}

		pThreadCommand = pThreadStruct->pQueue->RemoveFront();
		pThreadStruct->pQueueMutex->Unlock();

		pThreadCommand->Execute();
		delete pThreadCommand;
	}

	ncServiceLocator::Logger().Write(ncILogger::LOG_DEBUG, (const char *)"ncThreadPool::WorkerFunction - worker thread %u is exiting", ncThread::Self());
}
