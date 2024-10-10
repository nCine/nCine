#ifndef CLASS_NCINE_THREAD
#define CLASS_NCINE_THREAD

#if defined(_WIN32)
	#include "common_windefines.h"
	#include <windef.h>
	#include <winbase.h>
	#include <process.h>
	#include <processthreadsapi.h>
#else
	#include <pthread.h>
#endif

#if defined(__APPLE__)
	#include <mach/mach_init.h>
#endif

#include <nctl/UniquePtr.h>

namespace ncine {

#if !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)

/// A class representing the CPU affinity mask for a thread
class ThreadAffinityMask
{
  public:
	ThreadAffinityMask() { zero(); }
	ThreadAffinityMask(int cpuNum)
	    : ThreadAffinityMask()
	{
		zero();
		set(cpuNum);
	}

	/// Clears the CPU set
	void zero();
	/// Sets the specified CPU number to be included in the set
	void set(int cpuNum);
	/// Sets the specified CPU number to be excluded by the set
	void clear(int cpuNum);
	/// Returns true if the specified CPU number belongs to the set
	bool isSet(int cpuNum);

  private:
	#if defined(_WIN32)
	DWORD_PTR affinityMask_;
	#elif defined(__APPLE__)
	integer_t affinityTag_;
	#else
	cpu_set_t cpuSet_;
	#endif

	friend class Thread;
};

#endif

/// Thread class
class Thread
{
  public:
	using ThreadFunctionPtr = void (*)(void *);

	/// A default constructor for an object without the associated function
	Thread();
	/// Creates a thread around a function and runs it
	Thread(ThreadFunctionPtr threadFunction, void *threadArg);
	/// Destructor that detaches the thread
	~Thread();

	/// Move constructor
	Thread(Thread &&other);
	/// Move assignment operator
	Thread &operator=(Thread &&other);
	/// Deleted copy constructor
	Thread(const Thread &) = delete;
	/// Deleted assignment operator
	Thread &operator=(const Thread &) = delete;

	/// Swaps the state of two thread objects
	void swap(Thread &first, Thread &second);

	/// Returns the number of processors in the machine
	static unsigned int numProcessors();

	/// Spawns a new thread if the object hasn't one already associated
	void run(ThreadFunctionPtr threadFunction, void *threadArg);
	/// Joins the thread
	bool join();
	/// Returns true if the thread id is valid
	bool joinable() const;
	/// Detaches the thread
	bool detach();

#ifndef __EMSCRIPTEN__
	#ifndef __APPLE__
	/// Sets the thread name
	void setName(const char *name);
	#endif

	/// Sets the calling thread name
	static void setSelfName(const char *name);
#endif

	/// Gets the thread priority
	int priority() const;
	/// Sets the thread priority
	void setPriority(int priority);

	/// Returns the calling thread id
	static long int self();
	/// Terminates the calling thread
	[[noreturn]] static void exit();
	/// Yields the calling thread in favour of another one with the same priority
	static void yieldExecution();

#ifndef __ANDROID__
	/// Asks the thread for termination
	bool cancel();

	#ifndef __EMSCRIPTEN__
	/// Gets the thread affinity mask
	ThreadAffinityMask affinityMask() const;
	/// Sets the thread affinity mask
	void setAffinityMask(ThreadAffinityMask affinityMask);
	#endif
#endif

  private:
	/// The structure wrapping the information for thread creation
	struct ThreadInfo
	{
		ThreadFunctionPtr threadFunction = nullptr;
		void *threadArg = nullptr;
	};

#if defined(_WIN32)
	HANDLE handle_;
#else
	pthread_t tid_;
#endif

	/// The information structure for thread creation
	/*! \note It needs to stay on the heap as thread creation can occur after a moved object goes out of scope */
	nctl::UniquePtr<ThreadInfo> threadInfo_;
	/// The wrapper start function for thread creation
#if defined(_WIN32)
	#ifdef __MINGW32__
	static unsigned int(__attribute__((__stdcall__)) wrapperFunction)(void *arg);
	#else
	static unsigned int __stdcall wrapperFunction(void *arg);
	#endif
#else
	static void *wrapperFunction(void *arg);
#endif
};

inline Thread::~Thread()
{
	detach();
}

/// Move constructor
inline Thread::Thread(Thread &&other)
    : Thread()
{
	swap(*this, other);
}

inline Thread &Thread::operator=(Thread &&other)
{
	// Avoid self-assignment
	if (this != &other)
	{
#ifndef __ANDROID__
		cancel();
#else
		detach();
#endif
		// At this point the thread id should be 0 and objects can be swapped
		swap(*this, other);
	}

	return *this;
}

inline void Thread::swap(Thread &first, Thread &second)
{
#if defined(_WIN32)
	nctl::swap(first.handle_, second.handle_);
#else
	nctl::swap(first.tid_, second.tid_);
#endif
	nctl::swap(first.threadInfo_, second.threadInfo_);
}

inline bool Thread::joinable() const
{
#if defined(_WIN32)
	return (handle_ != 0);
#else
	return (tid_ != 0);
#endif
}

}

#endif
