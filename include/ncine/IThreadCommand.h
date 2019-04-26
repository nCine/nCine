#ifndef CLASS_NCINE_ITHREADCOMMAND
#define CLASS_NCINE_ITHREADCOMMAND

#include "common_defines.h"

namespace ncine {

/// Thread pool command interface
class DLL_PUBLIC IThreadCommand
{
  public:
	virtual ~IThreadCommand() {}

	/// Executes the command on a worker thread
	virtual void execute() = 0;
};

}

#endif
