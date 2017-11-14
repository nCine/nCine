#ifndef CLASS_NCINE_THREADCOMMANDS
#define CLASS_NCINE_THREADCOMMANDS

#include "common_macros.h"
#include "IThreadCommand.h"
#ifdef WITH_MULTITHREADING
	#include "SceneNode.h"
#endif

namespace ncine {

/// A dummy thread command for testing
class DummyCommand : public IThreadCommand
{
  public:
	explicit DummyCommand(unsigned int requestCode) : requestCode_(requestCode) { }

	inline void execute() { LOGI_X("worker thread got request code %u", requestCode_); }

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
