#ifndef CLASS_NCFILELOGGER
#define CLASS_NCFILELOGGER

#include <cstdio>
#include "ncILogger.h"

/// The usual console and file logger
class ncFileLogger : public ncILogger
{
private:
	FILE *m_fp;
	eLogLevel m_eConsoleLevel;
	eLogLevel m_eFileLevel;

public:
	ncFileLogger(const char *filename, eLogLevel eConsoleLevel, eLogLevel eFileLevel);
	~ncFileLogger();

	virtual void Write(eLogLevel eLevel, const char* fmt, ...);
};

#endif
