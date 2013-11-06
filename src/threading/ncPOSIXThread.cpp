#include <cstdlib> // for exit()
#include <unistd.h> // for sysconf()
#include <sched.h> // for sched_yield()
#include "ncThread.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// A default constructor for a class without the associated function
ncThread::ncThread()
	: m_tid(0)
{

}

/// Creates a thread around a function and runs it
ncThread::ncThread(ncThreadFunctionPtr_t pStartFunction, void *pArg)
	: m_tid(0)
{
	Run(pStartFunction, pArg);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

// Gets the number of processors in the machine
unsigned int ncThread::NumProcessors()
{
	unsigned int uNumProcs = 0;

	long int lConfRet = -1;
#if defined(_SC_NPROCESSORS_ONLN)
	lConfRet = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_SC_NPROC_ONLN)
	lConfRet = sysconf(_SC_NPROC_ONLN);
#endif

  if (lConfRet > 0)
	  uNumProcs = static_cast<unsigned int>(lConfRet);

  return uNumProcs;
}

/// Spawns a new thread if the class hasn't one already associated
void ncThread::Run(ncThreadFunctionPtr_t pStartFunction, void *pArg)
{
	if (m_tid == 0)
	{
		m_threadInfo.m_pStartFunction = pStartFunction;
		m_threadInfo.m_pThreadArg = pArg;
		if (int iError = pthread_create(&m_tid, NULL, WrapperFunction, &m_threadInfo))
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncThread::ncThread - pthread_create error: %i", iError);
			exit(EXIT_FAILURE);
		}
	}
	else
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncThread::ncThread - thread %u is already running", m_tid);
}

/// Joins the thread
void* ncThread::Join()
{
	void* pRetVal = NULL;
	pthread_join(m_tid, &pRetVal);
	return pRetVal;
}

/// Returns the calling thread id
long int ncThread::Self()
{
#if defined(__APPLE__)
	return reinterpret_cast<long int>(pthread_self());
#else
	return static_cast<long int>(pthread_self());
#endif
}

/// Terminates the calling thread
void ncThread::Exit(void *pRetVal)
{
	pthread_exit(pRetVal);
}

/// Yields the calling thread in favour of another one with the same priority
void ncThread::YieldExecution()
{
	sched_yield();
}

#ifndef __ANDROID__
/// Asks the thread for termination
void ncThread::Cancel()
{
	pthread_cancel(m_tid);
}
#endif

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// The wrapper start function for thread creation
void *ncThread::WrapperFunction(void *pArg)
{
	ncThreadInfo* pThreadInfo = static_cast<ncThreadInfo*>(pArg);
	pThreadInfo->m_pStartFunction(pThreadInfo->m_pThreadArg);

	return NULL;
}
