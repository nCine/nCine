#ifndef CLASS_NCINE_ILOGGER
#define CLASS_NCINE_ILOGGER

#if defined(_WIN32) && defined(ERROR)
	#undef ERROR
#endif

#include "common_defines.h"

namespace ncine {

/// The interface class for loggers
class DLL_PUBLIC ILogger
{
  public:
	/// Log levels, from less to more severe
	enum class LogLevel
	{
		UNKNOWN = -1,
		OFF = 0,
		VERBOSE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL
	};

	virtual ~ILogger() = 0;

	/// Logs a message with a specified level of severity
	virtual unsigned int write(LogLevel level, const char *fmt, ...) = 0;

	/// Consumes the queue of entries from all threads (called by the main thread only)
	/*! \returns The number of queued entries */
	virtual unsigned int consumeQueue() = 0;

	/// Returns the log string with all the recorded log entries
	virtual const char *logString() const = 0;
	/// Clears the entries from the log string
	virtual void clearLogString() = 0;
	/// Returns the length of the log string
	virtual unsigned int logStringLength() const = 0;
	/// Returns the capacity of the log string
	virtual unsigned int logStringCapacity() const = 0;
};

inline ILogger::~ILogger() {}

/// A fake logger which doesn't log anything
class DLL_PUBLIC NullLogger : public ILogger
{
  public:
	inline unsigned int write(LogLevel level, const char *fmt, ...) override { return 0; }
	inline unsigned int consumeQueue() override { return 0; }
	inline const char *logString() const override { return nullptr; }
	inline void clearLogString() override {}
	inline unsigned int logStringLength() const override { return 0; }
	inline unsigned int logStringCapacity() const override { return 0; }
};

}

#endif
