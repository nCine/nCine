#ifndef CLASS_NCINE_ITHREADPOOL
#define CLASS_NCINE_ITHREADPOOL

#include "common_defines.h"
#include "IThreadCommand.h"
#include "nctl/UniquePtr.h"

namespace ncine {

/// Thread pool interface class
class DLL_PUBLIC IThreadPool
{
  public:
	virtual ~IThreadPool() = 0;

	/// Enqueues a command request for a worker thread
	virtual void enqueueCommand(nctl::UniquePtr<IThreadCommand> threadCommand) = 0;
};

inline IThreadPool::~IThreadPool() { }


/// A fake thread pool which doesn't create any thread
class DLL_PUBLIC NullThreadPool : public IThreadPool
{
  public:
	void enqueueCommand(nctl::UniquePtr<IThreadCommand> threadCommand) override { }
};

}

#endif
