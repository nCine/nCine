#ifndef CLASS_NCINE_FRAMETIMER
#define CLASS_NCINE_FRAMETIMER

#include "IFrameTimer.h"
#include "TimeStamp.h"

namespace ncine {

/// Frame interval and average FPS calculator implementation class
class FrameTimer : public IFrameTimer
{
  public:
	/// Constructor
	FrameTimer(float averageInterval, float loggingInterval);

	/// Returns the total number of frames counted
	inline unsigned long int totalNumberFrames() const override { return totNumFrames_; }
	/// Returns the duration in seconds between the last two subsequent calls to `addFrame()`
	inline float lastFrameTime() const override { return frameTime_; }
	/// Returns the elapsed time in seconds since the last call to `addFrame()`
	inline float currentFrameTime() const override { return frameStart_.secondsSince(); }

	inline float averageFps() const override { return avgFps_; }
	inline float averageFrameTime() const override { return avgFrameTime_; }

	inline bool averageEnabled() const override { return averageInterval_ > 0.0f; }
	bool loggingEnabled() const override;

	inline float averageInterval() const override { return averageInterval_; }
	void setAverageInterval(float averageInterval) override;

	inline float loggingInterval() const override { return loggingInterval_; }
	void setLoggingInterval(float loggingInterval) override;

	inline ILogger::LogLevel logLevel() const override { return logLevel_; }
	void setLogLevel(ILogger::LogLevel logLevel) override;

	/// Adds a frame to the counter and calculates the interval since the previous one
	void addFrame();

	/// Starts counting the suspension time
	void suspend();
	/// Drifts timers by the duration of last suspension
	/*! \return A timestamp with last suspension duration */
	TimeStamp resume();

  private:
	/// Number of seconds between two average FPS calculations (user defined)
	float averageInterval_;
	/// Number of seconds between two logging events (user defined)
	float loggingInterval_;

	/// Timestamp at the beginning of a frame
	TimeStamp frameStart_;
	/// Last frame time in seconds
	float frameTime_;
	/// Timestamp at the begininng of application suspension
	TimeStamp suspensionStart_;

	/// Total number of frames counted
	unsigned long int totNumFrames_;
	/// Frame counter for average FPS calculation
	unsigned long int avgNumFrames_;
	/// Frame counter for logging
	unsigned long int logNumFrames_;

	/// Average FPS calulated during the specified interval
	float avgFps_;
	/// Average frame time calulated during the specified interval
	float avgFrameTime_;

	/// Timestamp at last average FPS calculation
	TimeStamp lastAvgUpdate_;
	/// Timestamp at last logging event
	TimeStamp lastLogUpdate_;

	ILogger::LogLevel logLevel_;
};

}

#endif
