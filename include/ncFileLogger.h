#ifndef CLASS_NCFILELOGGER
#define CLASS_NCFILELOGGER

#include <cstdio>
#include "ncILogger.h"
#include "ncIFile.h"

/// The usual console and file logger
class ncFileLogger : public ncILogger
{
 public:
	ncFileLogger(const char *pFilename, eLogLevel eConsoleLevel, eLogLevel eFileLevel);
	~ncFileLogger();

	virtual void Write(eLogLevel eLevel, const char* fmt, ...);

 private:
	ncIFile *m_pFileHandle;
	eLogLevel m_eConsoleLevel;
	eLogLevel m_eFileLevel;
};

#endif
