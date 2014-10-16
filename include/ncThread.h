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
	typedef void (*ncThreadFunctionPtr)(void *);

	// A default constructor for a class without the associated function
	ncThread();
	// Creates a thread around a function and runs it
	ncThread(ncThreadFunctionPtr startFunction, void* arg);
	~ncThread() { }

	// Gets the number of processors in the machine
	static unsigned int numProcessors();

	// Spawns a new thread if the class hasn't one already associated
	void run(ncThreadFunctionPtr startFunction, void* arg);
	// Joins the thread
	void* join();

	// Returns the calling thread id
	static long int self();
	// Terminates the calling thread
	static void exit(void *retVal);
	// Yields the calling thread in favour of another one with the same priority
	static void yieldExecution();

#ifndef __ANDROID__
	// Asks the thread for termination
	void cancel();
#endif

  private:
	/// The structure wrapping the information for thread creation
	struct ncThreadInfo
	{
		ncThreadInfo() : startFunction(NULL), threadArg(NULL) { }
		ncThreadFunctionPtr startFunction;
		void *threadArg;
	};

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	HANDLE	handle_;
#else
	pthread_t tid_;
#endif

	ncThreadInfo threadInfo_;
	// The wrapper start function for thread creation
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	#ifdef __GNUC__ // MinGW
		static unsigned int (__attribute__((__stdcall__)) WrapperFunction)(void* arg);
	#else // MSVC
		static unsigned int __stdcall wrapperFunction(void* arg);
	#endif
#else
	static void *wrapperFunction(void* arg);
#endif
};

#endif
