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
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncDummyCommand::execute - worker thread %u got request code %u",
		ncThread::self(), requestCode_);
}

#endif
