#ifndef CLASS_NCITHREADPOOL
#define CLASS_NCITHREADPOOL

class ncIThreadCommand;

/// It represents the interface to a thread pool
class ncIThreadPool
{
 public:
	virtual ~ncIThreadPool() = 0;

	/// Enqueues a command request for a worker thread
	virtual void EnqueueCommand(ncIThreadCommand *pThreadCommand) = 0;
};

inline ncIThreadPool::~ncIThreadPool() { }

#endif

#ifndef CLASS_NCNULLTHREADPOOL
#define CLASS_NCNULLTHREADPOOL

/// A fake thread pool which doesn't create any thread
class ncNullThreadPool : public ncIThreadPool
{
 public:
	virtual void EnqueueCommand(ncIThreadCommand *pThreadCommand) { }
};

#endif
