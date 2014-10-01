#ifndef CLASS_NCTHREADCOMMANDS
#define CLASS_NCTHREADCOMMANDS

#include "ncThread.h"
#include "ncServiceLocator.h"

/// Thread pool command interface
class ncIThreadCommand
{
 public:
	virtual ~ncIThreadCommand() { }

	virtual void Execute() = 0;
};

/// A dummy thread command for testing
class ncDummyCommand : public ncIThreadCommand
{
 public:
	ncDummyCommand(unsigned int uRequestCode) : m_uRequestCode(uRequestCode) { }

	void Execute();

 private:
	unsigned int m_uRequestCode;
};

inline void ncDummyCommand::Execute()
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncDummyCommand::Execute - worker thread %u got request code %u",
		ncThread::Self(), m_uRequestCode);
}

#endif
