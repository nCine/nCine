#ifdef __ANDROID__
	#include <stdarg.h>
	#include <android/log.h>
#else
	#include <cstdarg>
#endif
#include <ctime>
#include "FileLogger.h"
#include "common_macros.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

FileLogger::FileLogger(const char *filename, LogLevel consoleLevel, LogLevel fileLevel)
    : consoleLevel_(consoleLevel), fileLevel_(fileLevel)
#ifdef WITH_IMGUI
      ,
      logString_(LogStringCapacity)
#endif
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

#ifdef _WIN32
void writeOutputDebug(const char *logEntry);
#endif

unsigned int FileLogger::write(LogLevel level, const char *fmt, ...)
{
	// Early-out if logging is completely disabled
	if (consoleLevel_ == LogLevel::OFF && fileLevel_ == LogLevel::OFF)
		return 0;

	ASSERT(fmt);

	const int levelInt = static_cast<int>(level);
	const int consoleLevelInt = static_cast<int>(consoleLevel_);
	const int fileLevelInt = static_cast<int>(fileLevel_);

	time_t now;
	struct tm *ts;
	now = time(nullptr);
	ts = localtime(&now);

	logEntry_[0] = '\0';
	logEntry_[MaxEntryLength - 1] = '\0';
	unsigned int length = 0;

	//length += strftime(logEntry_ + length, MaxEntryLength - length - 1, "- %a %Y-%m-%d %H:%M:%S %Z ", ts);
	length += strftime(logEntry_ + length, MaxEntryLength - length - 1, "- %H:%M:%S ", ts);
	length += snprintf(logEntry_ + length, MaxEntryLength - length - 1, "[L%d] - ", levelInt);

	va_list args;
	va_start(args, fmt);
	length += vsnprintf(logEntry_ + length, MaxEntryLength - length - 1, fmt, args);
	va_end(args);

	if (length < MaxEntryLength - 2)
	{
		logEntry_[length++] = '\n';
		logEntry_[length] = '\0';
	}

	if (consoleLevel_ != LogLevel::OFF && levelInt >= consoleLevelInt)
	{
#ifndef __ANDROID__
		if (level == LogLevel::ERROR || level == LogLevel::FATAL)
			fputs(logEntry_, stderr);
		else
			fputs(logEntry_, stdout);

	#ifdef _WIN32
		writeOutputDebug(logEntry_);
	#endif

#else
		android_LogPriority priority;

		// clang-format off
		switch (level)
		{
			case LogLevel::FATAL:		priority = ANDROID_LOG_FATAL; break;
			case LogLevel::ERROR:		priority = ANDROID_LOG_ERROR; break;
			case LogLevel::WARN:		priority = ANDROID_LOG_WARN; break;
			case LogLevel::INFO:		priority = ANDROID_LOG_INFO; break;
			case LogLevel::DEBUG:		priority = ANDROID_LOG_DEBUG; break;
			case LogLevel::VERBOSE:		priority = ANDROID_LOG_VERBOSE; break;
			case LogLevel::UNKNOWN:		priority = ANDROID_LOG_UNKNOWN; break;
			default:					priority = ANDROID_LOG_UNKNOWN; break;
		}
		// clang-format on

		__android_log_write(priority, "nCine", logEntry_);
#endif
	}

	if (fileLevel_ != LogLevel::OFF && levelInt >= fileLevelInt)
	{
		fprintf(fileHandle_->ptr(), "%s", logEntry_);
		fflush(fileHandle_->ptr());
	}

#ifdef WITH_IMGUI
	if (levelInt >= consoleLevelInt || levelInt >= fileLevelInt)
	{
		if (length > logString_.capacity() - logString_.length() - 1)
			logString_.clear();

		logString_.append(logEntry_);
	}
#endif

#ifdef WITH_TRACY
	if (levelInt >= consoleLevelInt || levelInt >= fileLevelInt)
	{
		uint32_t color = 0x999999;
		// clang-format off
		switch (level)
		{
			case LogLevel::FATAL:		color = 0xec3e40; break;
			case LogLevel::ERROR:		color = 0xff9b2b; break;
			case LogLevel::WARN:		color = 0xf5d800; break;
			case LogLevel::INFO:		color = 0x01a46d; break;
			case LogLevel::DEBUG:		color = 0x377fc7; break;
			case LogLevel::VERBOSE:		color = 0x73A5D7; break;
			case LogLevel::UNKNOWN:		color = 0x999999; break;
			default:					color = 0x999999; break;
		}
		// clang-format on

		TracyMessageC(logEntry_, length, color);
	}
#endif

	return length;
}

}
