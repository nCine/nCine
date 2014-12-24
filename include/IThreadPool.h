#ifndef CLASS_NCINE_ITHREADPOOL
#define CLASS_NCINE_ITHREADPOOL

namespace ncine {

class IThreadCommand;

/// It represents the interface to a thread pool
class IThreadPool
{
  public:
	virtual ~IThreadPool() = 0;

	/// Enqueues a command request for a worker thread
	virtual void enqueueCommand(IThreadCommand *threadCommand) = 0;
};

inline IThreadPool::~IThreadPool() { }

#endif

#ifndef CLASS_NCINE_NULLTHREADPOOL
#define CLASS_NCINE_NULLTHREADPOOL

/// A fake thread pool which doesn't create any thread
class NullThreadPool : public IThreadPool
{
  public:
	virtual void enqueueCommand(IThreadCommand *threadCommand) { }
};

}

#endif
