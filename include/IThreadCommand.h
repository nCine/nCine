#ifndef CLASS_NCINE_ITHREADCOMMAND
#define CLASS_NCINE_ITHREADCOMMAND

namespace ncine {

/// Thread pool command interface
class IThreadCommand
{
  public:
	virtual ~IThreadCommand() { }

	virtual void execute() = 0;
};

}

#endif
