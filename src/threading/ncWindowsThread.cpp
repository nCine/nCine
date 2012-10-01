#include "ncThread.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// A default constructor for a class without the associated function
ncThread::ncThread()
	: m_handle(0)
{

}

/// Creates a thread around a function and runs it
ncThread::ncThread(ncThreadFunctionPtr_t pStartFunction, void * pArg)
	: m_handle(0)
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

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	uNumProcs = si.dwNumberOfProcessors;

	return uNumProcs;
}

/// Spawns a new thread if the class hasn't one already associated
void ncThread::Run(ncThreadFunctionPtr_t pStartFunction, void *pArg)
{
	if (m_handle == 0)
	{
		m_threadInfo.m_pStartFunction = pStartFunction;
		m_threadInfo.m_pThreadArg = pArg;
		m_handle = (HANDLE)_beginthreadex(NULL, 0, WrapperFunction, &m_threadInfo, 0, NULL);
		if (m_handle <= 0)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncThread::ncThread - _beginthreadex error");
			exit(-1);
		}
	}
	else
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncThread::ncThread - thread %u is already running", m_handle);
}

/// Joins the thread
void* ncThread::Join()
{
	WaitForSingleObject(m_handle, INFINITE);
	return NULL;
}

/// Returns the calling thread id
unsigned int ncThread::Self()
{
	return GetCurrentThreadId();
}

/// Terminates the calling thread
void ncThread::Exit(void *pRetVal)
{
	_endthreadex(0);
	*static_cast<unsigned int *>(pRetVal) = 0;
}

/// Yields the calling thread in favour of another one with the same priority
void ncThread::YieldExecution()
{
	Sleep(0);
}

/// Asks the thread for termination
void ncThread::Cancel()
{
	TerminateThread(m_handle, 0);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// The wrapper start function for thread creation
unsigned int ncThread::WrapperFunction(void *pArg)
{
	ncThreadInfo* pThreadInfo = static_cast<ncThreadInfo*>(pArg);
	pThreadInfo->m_pStartFunction(pThreadInfo->m_pThreadArg);

	return 0;
}
