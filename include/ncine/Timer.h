#ifndef CLASS_NCINE_TIMER
#define CLASS_NCINE_TIMER

#include <cstdint>
#include "common_defines.h"

namespace ncine {

/// Basic timer and synchronization class
class DLL_PUBLIC Timer
{
  public:
	Timer();

	/// Starts the timer
	void start();
	/// Stops the timer
	void stop();
	/// Resets the accumulated time
	inline void reset() { accumulatedTime_ = 0ULL; }
	/// Returns `true` if the timer is running
	inline bool isRunning() const { return isRunning_; }

	/// Returns elapsed time in seconds since last starting the timer
	float interval() const;
	/// Returns total accumulated time in seconds
	float total() const;

	/// Puts the current thread to sleep for the specified number of seconds
	static void sleep(float seconds);

  private:
	bool isRunning_;
	/// Start time mark
	uint64_t startTime_;
	/// Accumulated time ticks over multiple start and stop
	uint64_t accumulatedTime_;
};

}

#endif
