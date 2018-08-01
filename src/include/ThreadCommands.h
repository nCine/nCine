#ifndef CLASS_NCINE_THREADCOMMANDS
#define CLASS_NCINE_THREADCOMMANDS

#include "common_macros.h"
#include "IThreadCommand.h"

namespace ncine {

/// A dummy thread command for testing
class DummyCommand : public IThreadCommand
{
  public:
	explicit DummyCommand(unsigned int requestCode) : requestCode_(requestCode) { }

	inline void execute() override { LOGI_X("worker thread got request code %u", requestCode_); }

  private:
	unsigned int requestCode_;
};

}

#endif
