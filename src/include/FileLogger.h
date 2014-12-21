#ifndef CLASS_NCINE_FILELOGGER
#define CLASS_NCINE_FILELOGGER

#include <cstdio>
#include "ILogger.h"
#include "IFile.h"

namespace ncine {

/// The usual console and file logger
class FileLogger : public ILogger
{
  public:
	FileLogger(const char *filename, LogLevel consoleLevel, LogLevel fileLevel);
	~FileLogger();

	virtual void write(LogLevel level, const char* fmt, ...);

  private:
	IFile *fileHandle_;
	LogLevel consoleLevel_;
	LogLevel fileLevel_;

	/// Private copy constructor
	FileLogger(const FileLogger&);
	/// Private assignment operator
	FileLogger& operator=(const FileLogger&);
};

}

#endif
