#include "Timer.h"
#include "Clock.h"

#if defined(_WIN32)
	#include "common_windefines.h"
	#include <synchapi.h>
#else
	#include <unistd.h>
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Timer::Timer()
    : isRunning_(false), startTime_(clock().now()), accumulatedTime_(0ULL)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Timer::start()
{
	isRunning_ = true;
	startTime_ = clock().counter();
}

void Timer::stop()
{
	accumulatedTime_ += clock().counter() - startTime_;
	isRunning_ = false;
}

float Timer::interval() const
{
	return static_cast<float>(clock().counter() - startTime_) / clock().frequency();
}

float Timer::total() const
{
	return (isRunning_)
	           ? static_cast<float>(accumulatedTime_ + clock().counter() - startTime_) / clock().frequency()
	           : static_cast<float>(accumulatedTime_) / clock().frequency();
}

void Timer::sleep(float seconds)
{
#if defined(_WIN32)
	const unsigned int milliseconds = static_cast<unsigned int>(seconds) * 1000;
	SleepEx(milliseconds, FALSE);
#else
	const unsigned int microseconds = static_cast<unsigned int>(seconds) * 1000000;
	usleep(microseconds);
#endif
}

}
