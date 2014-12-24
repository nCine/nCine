#ifndef CLASS_NCILOGGER
#define CLASS_NCILOGGER

namespace ncine {

/// The interface for every logger
class ILogger
{
  public:
	/// Log levels, from less to more severe
	enum LogLevel
	{
		LOG_UNKNOWN = 0,
		LOG_VERBOSE,
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARN,
		LOG_ERROR,
		LOG_FATAL,
		LOG_OFF
	};

	virtual ~ILogger() = 0;

	/// Logs a message with a specified level of severity
	virtual void write(LogLevel level, const char *message, ...) = 0;
};

inline ILogger::~ILogger() { }

#endif

#ifndef CLASS_NCNULLLOGGER
#define CLASS_NCNULLLOGGER

/// A fake logger which doesn't log anything
class NullLogger : public ILogger
{
  public:
	virtual void write(LogLevel level, const char *message, ...) { }
};

}

#endif
