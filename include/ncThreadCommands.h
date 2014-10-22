#ifndef CLASS_NCTHREADCOMMANDS
#define CLASS_NCTHREADCOMMANDS

#include "ncThread.h"
#include "ncServiceLocator.h"
#ifdef WITH_MULTITHREADING
	#include "ncSceneNode.h"
#endif

/// Thread pool command interface
class ncIThreadCommand
{
  public:
	virtual ~ncIThreadCommand() { }

	virtual void execute() = 0;
};

/// A dummy thread command for testing
class ncDummyCommand : public ncIThreadCommand
{
  public:
	ncDummyCommand(unsigned int requestCode) : requestCode_(requestCode) { }

	inline void execute() { LOGI_X("worker thread %u got request code %u", ncThread::self(), requestCode_); }

  private:
	unsigned int requestCode_;
};

#ifdef WITH_MULTITHREADING

/// A thread command to update a ncSceneNode
class ncUpdateNodeCommand : public ncIThreadCommand
{
 public:
	ncUpdateNodeCommand(ncSceneNode *node, float interval)
		: node_(node), interval_(interval) { }

	inline void execute()
	{
		if (node_)
		{
			node_->update(interval_);
		}
	}

 private:
	ncSceneNode *node_;
	float interval_;
};

#endif

#endif
