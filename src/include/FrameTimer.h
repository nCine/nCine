#ifndef CLASS_NCINE_FRAMETIMER
#define CLASS_NCINE_FRAMETIMER

#include "Timer.h"

namespace ncine {

/// Frame interval and average FPS calculator class
class FrameTimer : public Timer
{
  public:
	/// Constructor
	FrameTimer(float logInterval, float avgInterval);

	/// Adds a frame to the counter and calculates the interval since the previous one
	void addFrame();
	/// Returns the total number of frames counted
	inline unsigned long int totalNumberFrames() const { return totNumFrames_; }
	/// Returns the interval between two subsequent calls to `addFrame()`
	inline float frameInterval() const { return frameInterval_; }
	/// Returns the average FPS during the update interval
	inline float averageFps() const { return fps_; }

  private:
	/// Number of seconds between two log events (user defined)
	float logInterval_;
	/// Number of seconds between two average FPS calculations (user defined)
	float avgInterval_;

	/// Seconds elapsed since previous frame
	float frameInterval_;

	/// Total number of frames counted
	unsigned long int totNumFrames_;
	/// Frame counter for average FPS calculation
	unsigned long int avgNumFrames_;
	/// Frame counter for logging
	unsigned long int logNumFrames_;

	/// Number of seconds since last average FPS calculation
	float lastAvgUpdate_;
	/// Number of seconds since last log event
	float lastLogUpdate_;

	/// Average FPS calulated during the specified interval
	float fps_;
};

}

#endif
