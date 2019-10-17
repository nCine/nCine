#ifndef CLASS_NCINE_FILELOGGER
#define CLASS_NCINE_FILELOGGER

#include <cstdio>
#include "ILogger.h"
#include "IFile.h"

namespace ncine {

/// The standard console and file logger
class FileLogger : public ILogger
{
  public:
	FileLogger(LogLevel consoleLevel);
	FileLogger(LogLevel consoleLevel, LogLevel fileLevel, const char *filename);
	~FileLogger() override;

	inline void setConsoleLevel(LogLevel consoleLevel) { consoleLevel_ = consoleLevel; }
	inline void setFileLevel(LogLevel fileLevel) { fileLevel_ = fileLevel; }
	bool openLogFile(const char *filename);

	unsigned int write(LogLevel level, const char *fmt, ...) override;

#ifdef WITH_IMGUI
	inline const char *logString() const override { return logString_.data(); }
	inline void clearLogString() override { logString_.clear(); }
	inline unsigned int logStringLength() const override { return logString_.length(); }
	inline unsigned int logStringCapacity() const override { return logString_.capacity(); }
#else
	inline const char *logString() const override { return nullptr; }
	inline void clearLogString() override {}
	inline unsigned int logStringLength() const override { return 0; }
	inline unsigned int logStringCapacity() const override { return 0; }
#endif

  private:
	nctl::UniquePtr<IFile> fileHandle_;
	LogLevel consoleLevel_;
	LogLevel fileLevel_;

	static const unsigned int MaxEntryLength = 512;
	char logEntry_[MaxEntryLength];

#ifdef WITH_IMGUI
	static const unsigned int LogStringCapacity = 16 * 1024;
	nctl::String logString_;
#endif

	/// Deleted copy constructor
	FileLogger(const FileLogger &) = delete;
	/// Deleted assignment operator
	FileLogger &operator=(const FileLogger &) = delete;
};

}

#endif
