#ifndef CLASS_NCINE_THREADCOMMANDS
#define CLASS_NCINE_THREADCOMMANDS

#include "Thread.h"
#include "ServiceLocator.h"
#ifdef WITH_MULTITHREADING
	#include "SceneNode.h"
#endif

namespace ncine {

/// Thread pool command interface
class IThreadCommand
{
  public:
	virtual ~IThreadCommand() { }

	virtual void execute() = 0;
};

/// A dummy thread command for testing
class DummyCommand : public IThreadCommand
{
  public:
	DummyCommand(unsigned int requestCode) : requestCode_(requestCode) { }

	inline void execute() { LOGI_X("worker thread %u got request code %u", Thread::self(), requestCode_); }

  private:
	unsigned int requestCode_;
};

#ifdef WITH_MULTITHREADING

/// A thread command to update a SceneNode object
class UpdateComand : public IThreadCommand
{
 public:
	UpdateComand(SceneNode *node, float interval)
		: node_(node), interval_(interval) { }

	inline void execute()
	{
		if (node_)
		{
			node_->update(interval_);
		}
	}

 private:
	SceneNode *node_;
	float interval_;
};

#endif

}

#endif
