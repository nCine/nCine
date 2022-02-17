#ifndef TEST_THREAD_FUNCTIONS
#define TEST_THREAD_FUNCTIONS

#if defined(_WIN32)
	#define INFINITE 0xFFFFFFFF
	#include <ncine/common_windefines.h>
	#include <synchapi.h>
	#include <process.h>
#else
	#include <pthread.h>
#endif

namespace {

template <unsigned int NumThreads>
class ThreadRunner
{
  public:
	ThreadRunner()
	    : ptr_(nullptr) {}
	ThreadRunner(void *ptr)
	    : ptr_(ptr) {}
	void setPointer(void *ptr) { ptr_ = ptr; }

#ifdef _WIN32
	using threadFuncRet = unsigned int;

	inline threadFuncRet retFunc() { return 0u; }

	inline void runThreads(unsigned int function(void *arg))
	{
		for (unsigned int i = 0; i < NumThreads; i++)
			handles_[i] = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, function, ptr_, 0, nullptr));
		for (unsigned int i = 0; i < NumThreads; i++)
			WaitForSingleObject(handles_[i], INFINITE);
	}
#else
	using threadFuncRet = void *;

	inline threadFuncRet retFunc() { return nullptr; }

	inline void runThreads(void *function(void *arg))
	{
		for (unsigned int i = 0; i < NumThreads; i++)
			pthread_create(&tid_[i], nullptr, function, ptr_);
		for (unsigned int i = 0; i < NumThreads; i++)
			pthread_join(tid_[i], nullptr);
	}
#endif

  private:
#ifdef _WIN32
	HANDLE handles_[NumThreads];
#else
	pthread_t tid_[NumThreads];
#endif

	void *ptr_;
};

}

#endif
