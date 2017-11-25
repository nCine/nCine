#ifndef CLASS_NCINE_ILOGGER
#define CLASS_NCINE_ILOGGER

#include "common_defines.h"

namespace ncine {

/// The interface class for loggers
class DLL_PUBLIC ILogger
{
  public:
	/// Log levels, from less to more severe
	enum class LogLevel
	{
		UNKNOWN = 0,
		VERBOSE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		OFF
	};

	virtual ~ILogger() = 0;

	/// Logs a message with a specified level of severity
	virtual void write(LogLevel level, const char *fmt, ...) = 0;
};

inline ILogger::~ILogger() { }


/// A fake logger which doesn't log anything
class DLL_PUBLIC NullLogger : public ILogger
{
  public:
	void write(LogLevel level, const char *fmt, ...) override { }
};

}

#endif
