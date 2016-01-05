#ifndef CLASS_NCINE_ITHREADPOOL
#define CLASS_NCINE_ITHREADPOOL

#include "common_defines.h"

namespace ncine {

class IThreadCommand;

/// It represents the interface to a thread pool
class DLL_PUBLIC IThreadPool
{
  public:
	virtual ~IThreadPool() = 0;

	/// Enqueues a command request for a worker thread
	virtual void enqueueCommand(IThreadCommand *threadCommand) = 0;
};

inline IThreadPool::~IThreadPool() { }


/// A fake thread pool which doesn't create any thread
class DLL_PUBLIC NullThreadPool : public IThreadPool
{
  public:
	virtual void enqueueCommand(IThreadCommand *threadCommand) { }
};

}

#endif
