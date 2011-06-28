#ifndef CLASS_NCILOGGER
#define CLASS_NCILOGGER

class ncILogger
{
public:

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

	virtual void Write(eLogLevel eLevel, const char *message, ...) = 0;
};

#endif

#ifndef CLASS_NCNULLLOGGER
#define CLASS_NCNULLLOGGER

class ncNullLogger : public ncILogger
{
public:
	virtual void Write(eLogLevel eLevel, const char *message, ...) { }
};

#endif
