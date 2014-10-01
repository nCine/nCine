#ifndef CLASS_NCTHREAD
#define CLASS_NCTHREAD

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	#include <windows.h>
	#include <process.h>
#else
	#include <pthread.h>
#endif

/// Thread class
class ncThread
{
 public:
	typedef void (*ncThreadFunctionPtr_t)(void *);

	// A default constructor for a class without the associated function
	ncThread();
	// Creates a thread around a function and runs it
	ncThread(ncThreadFunctionPtr_t pStartFunction, void *pArg);
	~ncThread() { }

	// Gets the number of processors in the machine
	static unsigned int NumProcessors();

	// Spawns a new thread if the class hasn't one already associated
	void Run(ncThreadFunctionPtr_t pStartFunction, void *pArg);
	// Joins the thread
	void *Join();

	// Returns the calling thread id
	static long int Self();
	// Terminates the calling thread
	static void Exit(void *pRetVal);
	// Yields the calling thread in favour of another one with the same priority
	static void YieldExecution();

#ifndef __ANDROID__
	// Asks the thread for termination
	void Cancel();
#endif

 private:
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	HANDLE	m_handle;
#else
	pthread_t m_tid;
#endif

	/// The structure wrapping the information for thread creation
	struct ncThreadInfo
	{
		ncThreadInfo() : m_pStartFunction(NULL), m_pThreadArg(NULL) { }
		ncThreadFunctionPtr_t m_pStartFunction;
		void *m_pThreadArg;
	};

	ncThreadInfo m_threadInfo;
	// The wrapper start function for thread creation
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	#ifdef __GNUC__ // MinGW
		static unsigned int (__attribute__((__stdcall__)) WrapperFunction)(void *pArg);
	#else // MSVC
		static unsigned int __stdcall WrapperFunction(void *pArg);
	#endif
#else
	static void *WrapperFunction(void *pArg);
#endif
};

#endif
