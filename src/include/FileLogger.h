#ifndef CLASS_NCINE_FILELOGGER
#define CLASS_NCINE_FILELOGGER

#include "ILogger.h"
#include "IFile.h"

#ifdef WITH_THREADS
	#include "LogEntryQueue.h"
#endif

namespace ncine {

/// The standard console and file logger
class FileLogger : public ILogger
{
  public:
	explicit FileLogger(LogLevel consoleLevel);
	FileLogger(LogLevel consoleLevel, LogLevel fileLevel, const char *filename);
	~FileLogger() override;

	void setConsoleLevel(LogLevel consoleLevel);
	inline void setFileLevel(LogLevel fileLevel) { fileLevel_ = fileLevel; }
	bool openLogFile(const char *filename);

	unsigned int write(LogLevel level, const char *fmt, ...) override;

	unsigned int consumeQueue() override;

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
	LogLevel consoleLevel_;
	LogLevel fileLevel_;
	bool canUseColors_;

#ifdef WITH_THREADS
	static const unsigned int MaxEntryLength = LogEntryQueue::MaxEntryLength;
	static thread_local char logEntry_[MaxEntryLength];
	LogEntryQueue logEntryQueue_;
	char queueEntry_[MaxEntryLength];
#else
	static const unsigned int MaxEntryLength = 1024;
	char logEntry_[MaxEntryLength];
#endif

#ifdef WITH_IMGUI
	static const unsigned int LogStringCapacity = 16 * 1024;
	nctl::String logString_;
#endif

	// Declared at the end to prevent a `heap-use-after-free` AddressSanitizer error
	nctl::UniquePtr<IFile> fileHandle_;

	/// The method that actually writes a message to the console and/or to the file
	void writeOut(LogLevel level, const char *logEntry, unsigned int length);

	/// Deleted copy constructor
	FileLogger(const FileLogger &) = delete;
	/// Deleted assignment operator
	FileLogger &operator=(const FileLogger &) = delete;
};

}

#endif
