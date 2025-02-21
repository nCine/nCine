#include "common_macros.h"
#include "FrameTimer.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! \note Defined here to avoid emitting its vtable in every translation unit */
IFrameTimer::~IFrameTimer()
{
}

/*! Constructs a timer which calculates average FPS every `averageInterval`
 *  seconds and writes to the log every `loggingInterval` seconds. */
FrameTimer::FrameTimer(float averageInterval, float loggingInterval)
    : averageInterval_(averageInterval), loggingInterval_(loggingInterval),
      frameTime_(0.0f),
      totNumFrames_(0L), avgNumFrames_(0L), logNumFrames_(0L),
      avgFps_(0.0f), avgFrameTime_(0.0f), logLevel_(ILogger::LogLevel::INFO)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool FrameTimer::loggingEnabled() const
{
	return (loggingInterval_ > 0.0f && logLevel_ != ILogger::LogLevel::OFF);
}

void FrameTimer::setAverageInterval(float averageInterval)
{
	if (averageInterval == averageInterval_)
		return;

	if (averageInterval <= 0.0f)
	{
		avgNumFrames_ = 0L;
		averageInterval_ = 0.0f;
	}
	else
	{
		if (averageInterval < averageInterval_ || averageInterval_ <= 0.0f)
		{
			avgNumFrames_ = 0L;
			lastAvgUpdate_ = TimeStamp::now();
		}
		averageInterval_ = averageInterval;
	}
}

void FrameTimer::setLoggingInterval(float loggingInterval)
{
	if (loggingInterval == loggingInterval_)
		return;

	if (loggingInterval <= 0.0f)
	{
		// Stop logging
		logNumFrames_ = 0L;
	}
	else
	{
		// Resume logging
		if (loggingInterval < loggingInterval_ || loggingInterval_ <= 0.0f)
		{
			logNumFrames_ = 0L;
			lastLogUpdate_ = TimeStamp::now();
		}
	}

	loggingInterval_ = (loggingInterval >= 0.0f) ? loggingInterval : 0.0f;
}

void FrameTimer::setLogLevel(ILogger::LogLevel logLevel)
{
	if (logLevel == logLevel_)
		return;

	if (logLevel == ILogger::LogLevel::OFF)
	{
		// Stop logging
		logNumFrames_ = 0L;
	}
	else
	{
		// Resume logging
		if (loggingInterval_ > 0.0f)
		{
			logNumFrames_ = 0L;
			lastLogUpdate_ = TimeStamp::now();
		}
	}

	logLevel_ = logLevel;
}

void FrameTimer::addFrame()
{
	frameTime_ = frameStart_.secondsSince();

	// Start counting for the next frame time
	frameStart_ = TimeStamp::now();

	totNumFrames_++;
	avgNumFrames_++;
	logNumFrames_++;

	// Update the FPS average calculation every `avgInterval_` seconds
	const float secsSinceLastAvgUpdate = (frameStart_ - lastAvgUpdate_).seconds();
	if (averageEnabled() && secsSinceLastAvgUpdate > averageInterval_)
	{
		avgFps_ = static_cast<float>(avgNumFrames_) / secsSinceLastAvgUpdate;
		avgFrameTime_ = secsSinceLastAvgUpdate / static_cast<float>(avgNumFrames_);

		avgNumFrames_ = 0L;
		lastAvgUpdate_ = TimeStamp::now();
	}

	const float secsSinceLastLogUpdate = (frameStart_ - lastLogUpdate_).seconds();
	// Log number of frames and FPS every `logInterval_` seconds
	if (loggingEnabled() && avgNumFrames_ != 0 && secsSinceLastLogUpdate > loggingInterval_)
	{
		const float fps = static_cast<float>(logNumFrames_) / secsSinceLastLogUpdate;
		const float frameTime = secsSinceLastLogUpdate / static_cast<float>(logNumFrames_);
		LOG_X(logLevel_, "%lu frames in %.2f seconds = %f FPS (%.3f ms per frame)", logNumFrames_, secsSinceLastLogUpdate, fps, frameTime * 1000.0f);

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
