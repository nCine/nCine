#ifndef CLASS_NCFILELOGGER
#define CLASS_NCFILELOGGER

#include <cstdio>
#include "ncILogger.h"
#include "ncFile.h"

/// The usual console and file logger
class ncFileLogger : public ncILogger
{
private:
	ncFile m_fileHandle;
	eLogLevel m_eConsoleLevel;
	eLogLevel m_eFileLevel;

public:
	ncFileLogger(const char *pFilename, eLogLevel eConsoleLevel, eLogLevel eFileLevel);
	~ncFileLogger();

	virtual void Write(eLogLevel eLevel, const char* fmt, ...);
};

#endif
