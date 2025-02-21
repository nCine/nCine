#ifndef CLASS_NCINE_IFRAMETIMER
#define CLASS_NCINE_IFRAMETIMER

#include "ILogger.h"

namespace ncine {

/// Frame time and average FPS calculator interface class
class IFrameTimer
{
  public:
	virtual ~IFrameTimer();

	/// Returns the total number of frames rendered since the application started
	virtual unsigned long int totalNumberFrames() const = 0;
	/// Returns the time in seconds that last frame took to complete
	//! \note Also called delta time. */
	virtual float lastFrameTime() const = 0;
	/// Returns the elapsed time in seconds since current frame started
	virtual float currentFrameTime() const = 0;

	/// Returns the average FPS during the update interval
	virtual float averageFps() const = 0;
	/// Returns the average frame time in seconds during the update interval
	virtual float averageFrameTime() const = 0;

	/// Returns true if the average FPS calculation is enabled
	virtual bool averageEnabled() const = 0;
	/// Returns true if logging of averages is enabled
	virtual bool loggingEnabled() const = 0;

	/// Returns the number of seconds between two average FPS calculations
	virtual float averageInterval() const = 0;
	/// Sets the number of seconds between two average FPS calculations
	virtual void setAverageInterval(float averageInterval) = 0;

	/// Returns the number of seconds between two logging events
	/*! \note A different and independent average is calculated over this time interval */
	virtual float loggingInterval() const = 0;
	/// Sets the number of seconds between two logging events
	virtual void setLoggingInterval(float loggingInterval) = 0;

	/// Returns the level for the logging events
	virtual ILogger::LogLevel logLevel() const = 0;
	/// Sets the level for the logging events
	virtual void setLogLevel(ILogger::LogLevel logLevel) = 0;
};

}

#endif
