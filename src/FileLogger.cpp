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
	: fileHandle_(NULL), consoleLevel_(consoleLevel), fileLevel_(fileLevel)
{
	fileHandle_ = IFile::createFileHandle(filename);

	if (fileLevel_ != LOG_OFF)
	{
		fileHandle_->setExitOnFailToOpen(false);
		fileHandle_->open(IFile::MODE_WRITE);

		if (fileHandle_->isOpened() == false)
		{
			if (consoleLevel_ > fileLevel_)
			{
				// Promoting console level logging to file level logging
				consoleLevel_ = fileLevel_;
			}

			// Disabling file logging if the log file cannot be opened
			fileLevel_ = LOG_OFF;
		}
	}

	if (consoleLevel_ != LOG_OFF)
	{
		setvbuf(stdout, NULL, _IONBF, 0);
	}
}

FileLogger::~FileLogger()
{
	write(LOG_VERBOSE, "FileLogger::~FileLogger -> End of the log");
	if (fileHandle_)
	{
		delete fileHandle_;
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#ifndef __ANDROID__
void FileLogger::write(LogLevel level, const char *fmt, ...)
{
	time_t now;
	struct tm *ts;
	const unsigned int bufferSize = 80;
	char buffer[bufferSize];

	now = time(0);
	ts = localtime(&now);
//	strftime(buffer, sizeof(char)*bufferSize, "%a %Y-%m-%d %H:%M:%S %Z", ts);
	strftime(buffer, sizeof(char)*bufferSize, "%H:%M:%S", ts);

	if (consoleLevel_ != LOG_OFF && int(level) >= int(consoleLevel_))
	{
		printf("- %s [L%d] - ", buffer, int(level));

		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);

		printf("\n");
	}

	if (fileLevel_ != LOG_OFF && int(level) >= int(fileLevel_))
	{
		fprintf(fileHandle_->ptr(), "- %s [L%d] - ", buffer, int(level));

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
	time_t now;
	struct tm *ts;
	const unsigned int bufferSize = 80;
	char buffer[bufferSize];

	now = time(0);
	ts = localtime(&now);
	strftime(buffer, sizeof(char)*bufferSize, "%H:%M:%S", ts);

	android_LogPriority priority;

	if (consoleLevel_ != LOG_OFF && int(level) >= int(consoleLevel_))
	{
		switch (level)
		{
			case LOG_FATAL:
				priority = ANDROID_LOG_FATAL;
				break;
			case LOG_ERROR:
				priority = ANDROID_LOG_ERROR;
				break;
			case LOG_WARN:
				priority = ANDROID_LOG_WARN;
				break;
			case LOG_INFO:
				priority = ANDROID_LOG_INFO;
				break;
			case LOG_DEBUG:
				priority = ANDROID_LOG_DEBUG;
				break;
			case LOG_VERBOSE:
				priority = ANDROID_LOG_VERBOSE;
				break;
			case LOG_UNKNOWN:
			default:
				priority = ANDROID_LOG_UNKNOWN;
				break;
		}

		va_list args;
		va_start(args, fmt);
		__android_log_vprint(priority, "nCine", fmt, args);
		va_end(args);
	}

	if (fileLevel_ != LOG_OFF && int(level) >= int(fileLevel_))
	{
		fprintf(fileHandle_->ptr(), "- %s [L%d] -> ", buffer, int(level));

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
