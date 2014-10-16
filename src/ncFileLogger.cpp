#ifdef __ANDROID__
	#include <stdarg.h>
	#include <android/log.h>
#else
	#include <cstdarg>
#endif
#include <ctime>
#include "ncFileLogger.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncFileLogger::ncFileLogger(const char *filename, LogLevel consoleLevel, LogLevel fileLevel)
	: fileHandle_(NULL), consoleLevel_(consoleLevel), fileLevel_(fileLevel)
{
	fileHandle_ = ncIFile::createFileHandle(filename);

	if (fileLevel_ < int(LOG_OFF))
	{
		fileHandle_->open(ncIFile::MODE_WRITE);
	}
	if (fileHandle_->isOpened() == false && consoleLevel_ > fileLevel_)
	{
		// Promoting console level logging to file level logging
		consoleLevel_ = fileLevel_;
	}

	if (consoleLevel_ < int(LOG_OFF))
	{
		setbuf(stdout, NULL);
	}
}


ncFileLogger::~ncFileLogger()
{
	write(LOG_VERBOSE, "ncFileLogger::~ncFileLogger - End of the log");
	if (fileHandle_)
	{
		delete fileHandle_;
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#ifndef __ANDROID__
void ncFileLogger::write(LogLevel level, const char *fmt, ...)
{
	time_t     now;
	struct tm  *ts;
	char       buffer[80];

	now = time(0);
	ts = localtime(&now);
//	strftime(buffer, sizeof(buffer), "%a %Y-%m-%d %H:%M:%S %Z", ts);
	strftime(buffer, sizeof(buffer), "%H:%M:%S", ts);

	if (consoleLevel_ < int(LOG_OFF) && int(level) >= int(consoleLevel_))
	{
		printf("- %s [L%d] -> ", buffer, int(level));

		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);

		printf("\n");
	}

	if (fileLevel_ < int(LOG_OFF) && int(level) >= int(fileLevel_))
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
#else
void ncFileLogger::write(LogLevel level, const char *fmt, ...)
{
	time_t     now;
	struct tm  *ts;
	char       buffer[80];

	now = time(0);
	ts = localtime(&now);
	strftime(buffer, sizeof(buffer), "%H:%M:%S", ts);

	android_LogPriority priority;

	if (consoleLevel_ < int(LOG_OFF) && int(level) >= int(consoleLevel_))
	{
		switch (consoleLevel_)
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

	if (fileLevel_ < int(LOG_OFF) && int(level) >= int(fileLevel_))
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
