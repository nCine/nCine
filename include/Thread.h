#ifndef CLASS_NCINE_THREAD
#define CLASS_NCINE_THREAD

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	#include <windows.h>
	#include <process.h>
#else
	#include <pthread.h>
#endif

namespace ncine {

/// Thread class
class Thread
{
  public:
	typedef void (*ThreadFunctionPtr)(void *);

	// A default constructor for a class without the associated function
	Thread();
	// Creates a thread around a function and runs it
	Thread(ThreadFunctionPtr startFunction, void* arg);
	~Thread() { }

	// Gets the number of processors in the machine
	static unsigned int numProcessors();

	// Spawns a new thread if the class hasn't one already associated
	void run(ThreadFunctionPtr startFunction, void* arg);
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
	struct ThreadInfo
	{
		ThreadInfo() : startFunction(NULL), threadArg(NULL) { }
		ThreadFunctionPtr startFunction;
		void *threadArg;
	};

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	HANDLE	handle_;
#else
	pthread_t tid_;
#endif

	ThreadInfo threadInfo_;
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

}

#endif
