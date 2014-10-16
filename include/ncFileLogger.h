#ifndef CLASS_NCFILELOGGER
#define CLASS_NCFILELOGGER

#include <cstdio>
#include "ncILogger.h"
#include "ncIFile.h"

/// The usual console and file logger
class ncFileLogger : public ncILogger
{
  public:
	ncFileLogger(const char *filename, LogLevel consoleLevel, LogLevel fileLevel);
	~ncFileLogger();

	virtual void write(LogLevel level, const char* fmt, ...);

  private:
	ncIFile *fileHandle_;
	LogLevel consoleLevel_;
	LogLevel fileLevel_;
};

#endif
