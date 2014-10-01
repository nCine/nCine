#ifndef CLASS_NCILOGGER
#define CLASS_NCILOGGER

/// The interface for every logger
class ncILogger
{
 public:
	/// Log levels, from less to more severe
	enum eLogLevel {
		LOG_UNKNOWN = 0,
    	LOG_VERBOSE,
    	LOG_DEBUG,
    	LOG_INFO,
    	LOG_WARN,
    	LOG_ERROR,
    	LOG_FATAL,
    	LOG_OFF    
	};

	virtual ~ncILogger() = 0;

	/// Logs a message with a specified level of severity
	virtual void Write(eLogLevel eLevel, const char *message, ...) = 0;
};

inline ncILogger::~ncILogger() { }

#endif

#ifndef CLASS_NCNULLLOGGER
#define CLASS_NCNULLLOGGER

/// A fake logger which doesn't log anything
class ncNullLogger : public ncILogger
{
 public:
	virtual void Write(eLogLevel eLevel, const char *message, ...) { }
};

#endif
