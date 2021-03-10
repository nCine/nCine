#ifdef __ANDROID__
	#include <stdarg.h>
	#include <android/log.h>
#else
	#include <cstdarg>
#endif

#include <ctime>
#include "FileLogger.h"
#include "common_macros.h"
#include <nctl/algorithms.h>
#include <nctl/CString.h>
#include "Application.h"
#include "tracy.h"

namespace {

const char *Reset = "\033[0m";
const char *Bold = "\033[1m";
const char *Faint = "\033[2m";
const char *Black = "\033[30m";
const char *BrightRed = "\033[91m";
const char *BrightGreen = "\033[92m";
const char *BrightYellow = "\033[93m";
const char *BrightRedBg = "\033[101m";

}

namespace ncine {

#ifdef _WIN32
bool createConsole(bool hasTermEnv);
void destroyConsole(bool hasTermEnv);
bool enableVirtualTerminalProcessing();
void writeOutputDebug(const char *logEntry);
#endif

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

FileLogger::FileLogger(LogLevel consoleLevel)
    : FileLogger(consoleLevel, LogLevel::OFF, nullptr)
{
}

FileLogger::FileLogger(LogLevel consoleLevel, LogLevel fileLevel, const char *filename)
    : consoleLevel_(consoleLevel), fileLevel_(fileLevel), canUseColors_(true)
#ifdef WITH_IMGUI
      ,
      logString_(LogStringCapacity)
#endif
{
#ifdef _WIN32
	if (consoleLevel != LogLevel::OFF)
	{
		const char *termEnv = getenv("TERM");
		createConsole(termEnv);

		const bool hasVTProcessing = enableVirtualTerminalProcessing();
		// mintty supports color sequences but not the Windows console API calls
		canUseColors_ = (hasVTProcessing || termEnv);
	}
#endif

	canUseColors_ &= theApplication().appConfiguration().withConsoleColors;

	openLogFile(filename);
	setvbuf(stdout, nullptr, _IONBF, 0);
	setvbuf(stderr, nullptr, _IONBF, 0);
}

FileLogger::~FileLogger()
{
	write(LogLevel::VERBOSE, "FileLogger::~FileLogger -> End of the log");

#ifdef _WIN32
	const char *termEnv = getenv("TERM");
	// mintty does not need to send the "Enter" key
	destroyConsole(termEnv);
#endif
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool FileLogger::openLogFile(const char *filename)
{
	if (fileLevel_ == LogLevel::OFF || filename == nullptr)
		return false;

	fileHandle_ = IFile::createFileHandle(filename);
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

		return false;
	}

	return true;
}

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

	const unsigned int timeMsgStart = length;
	const unsigned int timeMsgLength = strftime(logEntry_ + length, MaxEntryLength - length - 1, "- %H:%M:%S ", ts);
	length += timeMsgLength;

	length += snprintf(logEntry_ + length, MaxEntryLength - length - 1, "[L%d] - ", levelInt);

	const unsigned int logMsgStart = length;
	va_list args;
	va_start(args, fmt);
	const unsigned int logMsgLength = vsnprintf(logEntry_ + length, MaxEntryLength - length - 1, fmt, args);
	va_end(args);
	length += logMsgLength;

	if (length < MaxEntryLength - 2)
	{
		logEntry_[length++] = '\n';
		logEntry_[length] = '\0';
	}

	const char *consoleLogEntry = logEntry_;
	if (canUseColors_)
	{
		writeWithColors(level, logEntry_ + timeMsgStart, timeMsgLength, logEntry_ + logMsgStart, logMsgLength);
		consoleLogEntry = logEntryWithColors_;
	}

	if (consoleLevel_ != LogLevel::OFF && levelInt >= consoleLevelInt)
	{
#ifndef __ANDROID__
		if (level == LogLevel::ERROR || level == LogLevel::FATAL)
			fputs(consoleLogEntry, stderr);
		else
			fputs(consoleLogEntry, stdout);

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

	if (fileLevel_ != LogLevel::OFF && levelInt >= fileLevelInt &&
	    fileHandle_ != nullptr && fileHandle_->isOpened())
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
			case LogLevel::VERBOSE:		color = 0x73a5d7; break;
			case LogLevel::UNKNOWN:		color = 0x999999; break;
			default:					color = 0x999999; break;
		}
		// clang-format on

		TracyMessageC(logEntry_, length, color);
	}
#endif

	return length;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int FileLogger::writeWithColors(LogLevel level, const char *timeMsg, unsigned int timeMsgLength, const char *logMsg, unsigned int logMsgLength)
{
	const int levelInt = static_cast<int>(level);

	logEntryWithColors_[0] = '\0';
	logEntryWithColors_[MaxEntryLength - 1] = '\0';
	unsigned int length = 0;

	length += snprintf(logEntryWithColors_ + length, MaxEntryLength - length - 1, "%s", Faint);

	nctl::strncpy(logEntryWithColors_ + length, timeMsg, timeMsgLength);
	length += timeMsgLength;

	length += snprintf(logEntryWithColors_ + length, MaxEntryLength - length - 1, " %s", Reset);

	const char *levelColor = BrightGreen;
	switch (level)
	{

		case LogLevel::WARN:
			levelColor = BrightYellow;
			break;
		case LogLevel::ERROR:
			levelColor = BrightRed;
			break;
		case LogLevel::FATAL:
			levelColor = Black;
			break;
		default:
			levelColor = BrightGreen;
			break;
	}

	if (level == LogLevel::FATAL)
		length += snprintf(logEntryWithColors_ + length, MaxEntryLength - length - 1, "%s", BrightRedBg);

	length += snprintf(logEntryWithColors_ + length, MaxEntryLength - length - 1, "%s[L%d]%s", levelColor, levelInt, Reset);
	length += snprintf(logEntryWithColors_ + length, MaxEntryLength - length - 1, " %s- ", Faint);

	unsigned int logMsgFuncLength = 0;
	while (logMsg[logMsgFuncLength] != '>' && logMsg[logMsgFuncLength] != '\0')
		logMsgFuncLength++;
	logMsgFuncLength++; // skip '>' character

	nctl::strncpy(logEntryWithColors_ + length, logMsg, nctl::min(logMsgFuncLength, MaxEntryLength - length - 1));
	length += logMsgFuncLength;

	length += snprintf(logEntryWithColors_ + length, MaxEntryLength - length - 1, "%s", Reset);

	if (level == LogLevel::WARN || level == LogLevel::ERROR || level == LogLevel::FATAL)
		length += snprintf(logEntryWithColors_ + length, MaxEntryLength - length - 1, "%s", Bold);

	nctl::strncpy(logEntryWithColors_ + length, logMsg + logMsgFuncLength, nctl::min(logMsgLength - logMsgFuncLength, MaxEntryLength - length - 1));
	length += logMsgLength - logMsgFuncLength;

	if (level == LogLevel::WARN || level == LogLevel::ERROR || level == LogLevel::FATAL)
		length += snprintf(logEntryWithColors_ + length, MaxEntryLength - length - 1, "%s", Reset);

	if (length < MaxEntryLength - 2)
	{
		logEntryWithColors_[length++] = '\n';
		logEntryWithColors_[length] = '\0';
	}

	return length;
}
}
