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

#if WITH_JOBSYSTEM
	#include "IJobSystem.h"
#endif

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

#ifdef WITH_JOBSYSTEM
thread_local char FileLogger::logEntry_[MaxEntryLength];
#endif

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

FileLogger::FileLogger(LogLevel consoleLevel)
    : FileLogger(consoleLevel, LogLevel::OFF, nullptr)
{
}

FileLogger::FileLogger(LogLevel consoleLevel, LogLevel fileLevel, const char *filename)
    : consoleLevel_(LogLevel::OFF), fileLevel_(fileLevel), canUseColors_(true)
#ifdef WITH_IMGUI
      , logString_(LogStringCapacity)
#endif
{
	// The setter will create the console on Windows, if needed
	setConsoleLevel(consoleLevel);
	canUseColors_ &= theApplication().appConfiguration().withConsoleColors;

	openLogFile(filename);
	setvbuf(stdout, nullptr, _IONBF, 0);
	setvbuf(stderr, nullptr, _IONBF, 0);
}

FileLogger::~FileLogger()
{
	write(LogLevel::VERBOSE, "FileLogger::~FileLogger -> End of the log");

	// The setter will destroy the console on Windows, if needed
	setConsoleLevel(LogLevel::OFF);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void FileLogger::setConsoleLevel(LogLevel consoleLevel)
{
#ifdef _WIN32
	if (consoleLevel == consoleLevel_)
		return;

	if (consoleLevel_ == LogLevel::OFF) // from OFF to enabled
	{
		const char *termEnv = getenv("TERM");
		createConsole(termEnv);

		const bool hasVTProcessing = enableVirtualTerminalProcessing();
		// mintty supports color sequences but not the Windows console API calls
		canUseColors_ = (hasVTProcessing || termEnv);
	}
	else if (consoleLevel == LogLevel::OFF) // from enabled to OFF
	{
		const char *termEnv = getenv("TERM");
		// mintty does not need to send the "Enter" key
		destroyConsole(termEnv);
	}
#endif

	consoleLevel_ = consoleLevel;
}

bool FileLogger::openLogFile(const char *filename)
{
	if (fileLevel_ == LogLevel::OFF || filename == nullptr)
		return false;

	fileHandle_ = IFile::createFileHandle(filename);
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
	const int levelInt = static_cast<int>(level);
	const int consoleLevelInt = static_cast<int>(consoleLevel_);
	const int fileLevelInt = static_cast<int>(fileLevel_);

	const bool shouldLogToConsole = (consoleLevel_ != LogLevel::OFF && levelInt >= consoleLevelInt);
	const bool shouldLogToFile = (fileLevel_ != LogLevel::OFF && levelInt >= fileLevelInt);

	// Early-out if this message does not need to be logged
	if (shouldLogToConsole == false && shouldLogToFile == false)
		return 0;

	ASSERT(fmt);

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

	time_t now;
	struct tm *ts;
	now = time(nullptr);
	ts = localtime(&now);

	logEntry_[0] = '\0';
	logEntry_[MaxEntryLength - 1] = '\0';
	unsigned int length = 0;

	if (canUseColors_)
		length += snprintf(logEntry_ + length, MaxEntryLength - length - 1, "%s", Faint);

	length += strftime(logEntry_ + length, MaxEntryLength - length - 1, "- %H:%M:%S ", ts);

	if (canUseColors_)
		length += snprintf(logEntry_ + length, MaxEntryLength - length - 1, " %s", Reset);

#if WITH_JOBSYSTEM
	const unsigned char numThreads = theServiceLocator().jobSystem().numThreads();
	// Don't include the thread index in the log entry if there aren't at least two threads in total
	if (numThreads > 1)
		length += snprintf(logEntry_ + length, MaxEntryLength - length - 1, "[T%02u] ", IJobSystem::threadIndex());
#endif

	if (canUseColors_)
	{
		if (level == LogLevel::FATAL)
			length += snprintf(logEntry_ + length, MaxEntryLength - length - 1, "%s", BrightRedBg);

		length += snprintf(logEntry_ + length, MaxEntryLength - length - 1, "%s[L%d]%s", levelColor, levelInt, Reset);
		length += snprintf(logEntry_ + length, MaxEntryLength - length - 1, " %s- ", Faint);
		length += snprintf(logEntry_ + length, MaxEntryLength - length - 1, "%s", Reset);
	}
	else
		length += snprintf(logEntry_ + length, MaxEntryLength - length - 1, "[L%d] - ", levelInt);

	if (canUseColors_ && (level == LogLevel::WARN || level == LogLevel::ERROR || level == LogLevel::FATAL))
		length += snprintf(logEntry_ + length, MaxEntryLength - length - 1, "%s", Bold);

	va_list args;
	va_start(args, fmt);
	const unsigned int logMsgLength = vsnprintf(logEntry_ + length, MaxEntryLength - length - 1, fmt, args);
	va_end(args);
	length += logMsgLength;

	if (canUseColors_ && (level == LogLevel::WARN || level == LogLevel::ERROR || level == LogLevel::FATAL))
		length += snprintf(logEntry_ + length, MaxEntryLength - length - 1, "%s", Reset);

	if (length < MaxEntryLength - 2)
	{
		logEntry_[length++] = '\n';
		logEntry_[length] = '\0';
	}

#ifdef WITH_TRACY
	// Tracy messages can come from all threads, pushing to the queue is not needed
	uint32_t tracyMsgColor = 0x999999;
	// clang-format off
	switch (level)
	{
		case LogLevel::FATAL:       tracyMsgColor = 0xec3e40; break;
		case LogLevel::ERROR:       tracyMsgColor = 0xff9b2b; break;
		case LogLevel::WARN:        tracyMsgColor = 0xf5d800; break;
		case LogLevel::INFO:        tracyMsgColor = 0x01a46d; break;
		case LogLevel::DEBUG:       tracyMsgColor = 0x377fc7; break;
		case LogLevel::VERBOSE:     tracyMsgColor = 0x73a5d7; break;
		case LogLevel::UNKNOWN:     tracyMsgColor = 0x999999; break;
		default:                    tracyMsgColor = 0x999999; break;
	}
	// clang-format on

	TracyMessageC(logEntry_, length, tracyMsgColor);
#endif

#if WITH_JOBSYSTEM
	// Only enqueue if there are more than two threads in total and if the message is not coming from the main thread
	if (numThreads > 1 && IJobSystem::isMainThread() == false)
		logEntryQueue_.enqueue(logEntry_, length);
	else
		writeOut(level, logEntry_, length);
#else
	writeOut(level, logEntry_, length);
#endif

	return length;
}

unsigned int FileLogger::consumeQueue()
{
	unsigned int numEntries = 0;

#if WITH_JOBSYSTEM
	FATAL_ASSERT_MSG(IJobSystem::isMainThread() == true, "This method should be called by the main thread only");
	while (unsigned int length = logEntryQueue_.dequeue(queueEntry_, MaxEntryLength))
	{
		const char *offset = strstr(queueEntry_, "[L");
		int logLevelInt = static_cast<int>(LogLevel::OFF);
		if (offset != nullptr)
			sscanf(offset, "[L%d]", &logLevelInt);

		writeOut(static_cast<LogLevel>(logLevelInt), queueEntry_, length);
		numEntries++;
	}
#endif

	return numEntries;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void FileLogger::writeOut(LogLevel level, const char *logEntry, unsigned int length)
{
	const int levelInt = static_cast<int>(level);
	const int consoleLevelInt = static_cast<int>(consoleLevel_);
	const int fileLevelInt = static_cast<int>(fileLevel_);

	const bool shouldLogToConsole = (consoleLevel_ != LogLevel::OFF && levelInt >= consoleLevelInt);
	const bool shouldLogToFile = (fileLevel_ != LogLevel::OFF && levelInt >= fileLevelInt);

	if (shouldLogToConsole)
	{
#ifndef __ANDROID__
		if (level == LogLevel::ERROR || level == LogLevel::FATAL)
			fputs(logEntry, stderr);
		else
			fputs(logEntry, stdout);

	#ifdef _WIN32
		writeOutputDebug(logEntry);
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

		__android_log_write(priority, "nCine", logEntry);
#endif
	}

	if (shouldLogToFile && fileHandle_ != nullptr && fileHandle_->isOpened())
	{
		fprintf(fileHandle_->ptr(), "%s", logEntry);
		fflush(fileHandle_->ptr());
	}

#ifdef WITH_IMGUI
	if (shouldLogToConsole || shouldLogToFile)
	{
		if (length > logString_.capacity() - logString_.length() - 1)
			logString_.clear();

		logString_.append(logEntry);
	}
#endif
}

}
