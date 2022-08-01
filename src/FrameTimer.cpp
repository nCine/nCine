#include "common_macros.h"
#include "FrameTimer.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! Constructs a timer which calculates average FPS every `avgInterval`
 *  seconds and writes to the log every `logInterval` seconds. */
FrameTimer::FrameTimer(float logInterval, float avgInterval)
    : logInterval_(logInterval), avgInterval_(avgInterval),
      totNumFrames_(0L), avgNumFrames_(0L), logNumFrames_(0L), fps_(0.0f)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void FrameTimer::addFrame()
{
	frameInterval_ = frameStart_.secondsSince();

	// Start counting for the next frame interval
	frameStart_ = TimeStamp::now();

	totNumFrames_++;
	avgNumFrames_++;
	logNumFrames_++;

	// Update the FPS average calculation every `avgInterval_` seconds
	const float secsSinceLastAvgUpdate = (frameStart_ - lastAvgUpdate_).seconds();
	if (avgInterval_ > 0.0f && secsSinceLastAvgUpdate > avgInterval_)
	{
		fps_ = static_cast<float>(avgNumFrames_) / secsSinceLastAvgUpdate;

		avgNumFrames_ = 0L;
		lastAvgUpdate_ = TimeStamp::now();
	}

	const float secsSinceLastLogUpdate = (frameStart_ - lastLogUpdate_).seconds();
	// Log number of frames and FPS every `logInterval_` seconds
	if (logInterval_ > 0.0f && avgNumFrames_ != 0 && secsSinceLastLogUpdate > logInterval_)
	{
		fps_ = static_cast<float>(logNumFrames_) / logInterval_;
		const float msPerFrame = (logInterval_ * 1000.0f) / static_cast<float>(logNumFrames_);
		LOGV_X("%lu frames in %.0f seconds = %f FPS (%.3f ms per frame)", logNumFrames_, logInterval_, fps_, msPerFrame);

		logNumFrames_ = 0L;
		lastLogUpdate_ = TimeStamp::now();
	}
}

void FrameTimer::suspend()
{
	suspensionStart_ = TimeStamp::now();
}

TimeStamp FrameTimer::resume()
{
	const TimeStamp suspensionDuration = suspensionStart_.timeSince();
	frameStart_ += suspensionDuration;
	lastAvgUpdate_ += suspensionDuration;
	lastLogUpdate_ += suspensionDuration;

	return suspensionDuration;
}

}
