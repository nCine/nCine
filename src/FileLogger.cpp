#ifdef __ANDROID__
	#include <stdarg.h>
	#include <android/log.h>
#else
	#include <cstdarg>
#endif
#include <ctime>
#include "FileLogger.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

FileLogger::FileLogger(const char *filename, LogLevel consoleLevel, LogLevel fileLevel)
	: consoleLevel_(consoleLevel), fileLevel_(fileLevel)
{
	fileHandle_ = IFile::createFileHandle(filename);

	if (fileLevel_ != LogLevel::OFF)
	{
		fileHandle_->setExitOnFailToOpen(false);
		fileHandle_->open(IFile::OpenMode::WRITE);

		if (fileHandle_->isOpened() == false)
		{
			if (consoleLevel_ > fileLevel_)
			{
				// Promoting console level logging to file level logging
				consoleLevel_ = fileLevel_;
			}

			// Disabling file logging if the log file cannot be opened
			fileLevel_ = LogLevel::OFF;
		}
	}

	if (consoleLevel_ != LogLevel::OFF)
		setvbuf(stdout, nullptr, _IONBF, 0);
}

FileLogger::~FileLogger()
{
	write(LogLevel::VERBOSE, "FileLogger::~FileLogger -> End of the log");
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#ifndef __ANDROID__
void FileLogger::write(LogLevel level, const char *fmt, ...)
{
	ASSERT(fmt);

	const int levelInt = static_cast<int>(level);
	const int consoleLevelInt = static_cast<int>(consoleLevel_);
	const int fileLevelInt = static_cast<int>(fileLevel_);

	time_t now;
	struct tm *ts;
	const unsigned int bufferSize = 80;
	char buffer[bufferSize];

	now = time(nullptr);
	ts = localtime(&now);
	// strftime(buffer, sizeof(char) * bufferSize, "%a %Y-%m-%d %H:%M:%S %Z", ts);
	strftime(buffer, sizeof(char) * bufferSize, "%H:%M:%S", ts);

	if (consoleLevel_ != LogLevel::OFF && levelInt >= consoleLevelInt)
	{
		printf("- %s [L%d] - ", buffer, levelInt);

		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);

		printf("\n");
	}

	if (fileLevel_ != LogLevel::OFF && levelInt >= fileLevelInt)
	{
		fprintf(fileHandle_->ptr(), "- %s [L%d] - ", buffer, levelInt);

		va_list args;
		va_start(args, fmt);
		vfprintf(fileHandle_->ptr(), fmt, args);
		va_end(args);

		fprintf(fileHandle_->ptr(), "\n");
		fflush(fileHandle_->ptr());
	}
}
#else
void FileLogger::write(LogLevel level, const char *fmt, ...)
{
	ASSERT(fmt);

	const int levelInt = static_cast<int>(level);
	const int consoleLevelInt = static_cast<int>(consoleLevel_);
	const int fileLevelInt = static_cast<int>(fileLevel_);

	time_t now;
	struct tm *ts;
	const unsigned int bufferSize = 80;
	char buffer[bufferSize];

	now = time(nullptr);
	ts = localtime(&now);
	strftime(buffer, sizeof(char) * bufferSize, "%H:%M:%S", ts);

	android_LogPriority priority;

	if (consoleLevel_ != LogLevel::OFF && levelInt >= consoleLevelInt)
	{
		switch (level)
		{
			case LogLevel::FATAL:		priority = ANDROID_LOG_FATAL;	break;
			case LogLevel::ERROR:		priority = ANDROID_LOG_ERROR;	break;
			case LogLevel::WARN:		priority = ANDROID_LOG_WARN; break;
			case LogLevel::INFO:		priority = ANDROID_LOG_INFO; break;
			case LogLevel::DEBUG:		priority = ANDROID_LOG_DEBUG;	break;
			case LogLevel::VERBOSE:		priority = ANDROID_LOG_VERBOSE; break;
			case LogLevel::UNKNOWN:		priority = ANDROID_LOG_UNKNOWN; break;
			default:					priority = ANDROID_LOG_UNKNOWN; break;
		}

		va_list args;
		va_start(args, fmt);
		__android_log_vprint(priority, "nCine", fmt, args);
		va_end(args);
	}

	if (fileLevel_ != LogLevel::OFF && levelInt >= fileLevelInt)
	{
		fprintf(fileHandle_->ptr(), "- %s [L%d] -> ", buffer, levelInt);

		va_list args;
		va_start(args, fmt);
		vfprintf(fileHandle_->ptr(), fmt, args);
		va_end(args);

		fprintf(fileHandle_->ptr(), "\n");
		fflush(fileHandle_->ptr());
	}
}
#endif

}
