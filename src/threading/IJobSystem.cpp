#include "IJobSystem.h"

namespace ncine {

namespace {
	/// The thread index number of the main thread
	const unsigned char MainThreadIndex = 0;

	/// The thread index for each thread
	/*! \note The main thread has always a thread index equal to `MainThreadIndex`. */
	thread_local unsigned char threadIndex_ = MainThreadIndex;
}

unsigned char IJobSystem::threadIndex()
{
	return threadIndex_;
}

unsigned char IJobSystem::mainThreadIndex()
{
	return MainThreadIndex;
}

void IJobSystem::setThreadIndex(unsigned char threadIndex)
{
	threadIndex_ = threadIndex;
}

bool IJobSystem::isMainThread()
{
	return threadIndex_ == MainThreadIndex;
}

}
