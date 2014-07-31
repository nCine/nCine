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

ncFileLogger::ncFileLogger(const char *pFilename, eLogLevel eConsoleLevel, eLogLevel eFileLevel)
	: m_pFileHandle(NULL), m_eConsoleLevel(eConsoleLevel), m_eFileLevel(eFileLevel)
{
	m_pFileHandle = ncIFile::CreateFileHandle(pFilename);

	if (m_eFileLevel < int(LOG_OFF))
		m_pFileHandle->Open(ncIFile::MODE_WRITE);
	if (m_pFileHandle->IsOpened() == false && m_eConsoleLevel > m_eFileLevel)
	{
		// Promoting console level logging to file level logging
		m_eConsoleLevel = m_eFileLevel;
	}

	if (m_eConsoleLevel < int(LOG_OFF))
		 setbuf(stdout, NULL);
}


ncFileLogger::~ncFileLogger()
{
	Write(LOG_VERBOSE, "ncFileLogger::~ncFileLogger - End of the log");
	if (m_pFileHandle)
		delete m_pFileHandle;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#ifndef __ANDROID__
void ncFileLogger::Write(eLogLevel eLevel, const char *fmt, ...)
{
	time_t     now;
	struct tm  *ts;
	char       szBuffer[80];

	now = time(0);
	ts = localtime(&now);
//	strftime(szBuffer, sizeof(szBuffer), "%a %Y-%m-%d %H:%M:%S %Z", ts);
	strftime(szBuffer, sizeof(szBuffer), "%H:%M:%S", ts);

	if (m_eConsoleLevel < int(LOG_OFF) && int(eLevel) >= int(m_eConsoleLevel))
	{
		printf("- %s [L%d] -> ", szBuffer, int(eLevel));

		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);

		printf("\n");
	}

	if (m_eFileLevel < int(LOG_OFF) && int(eLevel) >= int(m_eFileLevel))
	{
		fprintf(m_pFileHandle->Ptr(), "- %s [L%d] -> ", szBuffer, int(eLevel));

		va_list args;
		va_start(args, fmt);
		vfprintf(m_pFileHandle->Ptr(), fmt, args);
		va_end(args);

		fprintf(m_pFileHandle->Ptr(), "\n");
		fflush(m_pFileHandle->Ptr());
	}
}
#else
void ncFileLogger::Write(eLogLevel eLevel, const char *fmt, ...)
{
	time_t     now;
	struct tm  *ts;
	char       szBuffer[80];

	now = time(0);
	ts = localtime(&now);
	strftime(szBuffer, sizeof(szBuffer), "%H:%M:%S", ts);

	android_LogPriority priority;

	if (m_eConsoleLevel < int(LOG_OFF) && int(eLevel) >= int(m_eConsoleLevel))
	{
		switch(m_eConsoleLevel)
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

	if (m_eFileLevel < int(LOG_OFF) && int(eLevel) >= int(m_eFileLevel))
	{
		fprintf(m_pFileHandle->Ptr(), "- %s [L%d] -> ", szBuffer, int(eLevel));

		va_list args;
		va_start(args, fmt);
		vfprintf(m_pFileHandle->Ptr(), fmt, args);
		va_end(args);

		fprintf(m_pFileHandle->Ptr(), "\n");
		fflush(m_pFileHandle->Ptr());
	}
}
#endif
