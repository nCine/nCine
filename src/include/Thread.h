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

#if !defined(__EMSCRIPTEN__)

// Forward declarations for friends
class ThreadAffinityMask;
namespace ThisThread {
	ThreadAffinityMask affinityMask();
	void setAffinityMask(ThreadAffinityMask affinityMask);
}

/// A class representing the CPU affinity mask for a thread
/*! \note On macOS this is not a mask, but a numeric tag. */
class ThreadAffinityMask
{
  public:
	ThreadAffinityMask() { zero(); }

	explicit ThreadAffinityMask(int cpuNum)
	    : ThreadAffinityMask()
	{
		set(cpuNum);
	}

	/// Clears the CPU set
	void zero();
	/// Includes the specified CPU number into the set
	void set(int cpuNum);
	/// Excludes the specified CPU number from the set
	void clear(int cpuNum);
	/// Returns `true` if the specified CPU number belongs to the set
	bool isSet(int cpuNum) const;

  private:
	#if defined(_WIN32)
	DWORD_PTR affinityMask_;
	#elif defined(__APPLE__)
	integer_t affinityTag_;
	#else
	cpu_set_t cpuSet_;
	#endif

	friend class Thread;
	friend ThreadAffinityMask ThisThread::affinityMask();
	friend void ThisThread::setAffinityMask(ThreadAffinityMask affinityMask);
};

#endif

/// Thread class
class Thread
{
  public:
	using ThreadFunctionPtr = void (*)(void *);

	enum class Priority
	{
		Lowest,
		Low,
		Normal,
		High,
		Highest
	};

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

	/// Returns the thread unique integral id
	inline unsigned long int threadId() const { return tid_; }

	/// Spawns a new thread if the object hasn't one already associated
	void run(ThreadFunctionPtr threadFunction, void *threadArg);
	/// Joins the thread
	bool join();
	/// Returns `true` if the thread native id is valid
	bool joinable() const;
	/// Detaches the thread
	bool detach();

#ifndef __EMSCRIPTEN__
	#ifndef __APPLE__
	/// Sets the thread name
	void setName(const char *name);
	#endif

	/// Gets the thread affinity mask
	ThreadAffinityMask affinityMask() const;
	/// Sets the thread affinity mask
	void setAffinityMask(ThreadAffinityMask affinityMask);
#endif

	/// Gets the thread priority
	Priority priority() const;
	/// Sets the thread priority
	bool setPriority(Priority priority);

#ifndef __ANDROID__
	/// Asks the thread for termination
	bool cancel();
#endif

	/// Returns the number of processors in the machine
	static unsigned int numProcessors();

  private:
	/// The structure wrapping the information for thread creation
	struct ThreadInfo
	{
		unsigned long int *tid = nullptr;
		ThreadFunctionPtr threadFunction = nullptr;
		void *threadArg = nullptr;
	};

	/// Kernel thread id
	unsigned long int tid_;
#if defined(_WIN32)
	/// Windows thread handle
	HANDLE handle_;
#else
	/// POSIX Threads id
	pthread_t pthid_;
#endif

	/// The information structure for thread creation
	/*! \note It needs to stay on the heap as thread creation can occur after a moved object goes out of scope. */
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

/// A collection of functions that operate on the calling thread only
namespace ThisThread {

	/// Returns the calling thread unique integral id
	unsigned long int threadId();

#ifndef __EMSCRIPTEN__
	/// Sets the calling thread name
	void setName(const char *name);

	/// Gets the calling thread affinity mask
	ThreadAffinityMask affinityMask();
	/// Sets the calling thread affinity mask
	void setAffinityMask(ThreadAffinityMask affinityMask);
#endif

	/// Gets the calling thread priority
	Thread::Priority priority();
	/// Sets the calling thread priority
	bool setPriority(Thread::Priority priority);

	/// Yields the calling thread in favour of another one with the same priority
	void yield();

	/// Terminates the calling thread
	[[noreturn]] void exit();

}

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
	nctl::swap(first.tid_, second.tid_);
#if defined(_WIN32)
	nctl::swap(first.handle_, second.handle_);
#else
	nctl::swap(first.pthid_, second.pthid_);
#endif
	nctl::swap(first.threadInfo_, second.threadInfo_);
}

inline bool Thread::joinable() const
{
#if defined(_WIN32)
	return (handle_ != 0);
#else
	return (pthid_ != 0);
#endif
}

}

#endif
