#ifndef CLASS_NCTHREADCOMMANDS
#define CLASS_NCTHREADCOMMANDS

#include "ncThread.h"
#include "ncServiceLocator.h"

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

	void execute();

  private:
	unsigned int requestCode_;
};

inline void ncDummyCommand::execute()
{
	LOGI_X("worker thread %u got request code %u", ncThread::self(), requestCode_);
}

#endif
