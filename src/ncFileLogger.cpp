#include <cstdarg>
#include <ctime>
#include "ncFileLogger.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncFileLogger::ncFileLogger(const char *filename, int iConsoleLevel, int iFileLevel)
	: m_fp(0), m_iConsoleLevel(iConsoleLevel), m_iFileLevel(iFileLevel)
{
	if (m_iConsoleLevel > -1)
		 setbuf(stdout, NULL);
	if (m_iFileLevel > -1)
		m_fp = fopen(filename, "a");

	Write(6, "FileLogger instantiated");
}


ncFileLogger::~ncFileLogger()
{
	Write(6, "FileLogger destruction");

	if (m_fp)
		fclose(m_fp);
};

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncFileLogger::Write(int iLevel, const char *fmt, ...)
{
	time_t rawtime;
	time(&rawtime);

	time_t     now;
	struct tm  *ts;
	char       szBuffer[80];

	now = time(0);

	ts = localtime(&now);
//	strftime(szBuffer, sizeof(szBuffer), "%a %Y-%m-%d %H:%M:%S %Z", ts);
	strftime(szBuffer, sizeof(szBuffer), "%H:%M:%S", ts);

	if (m_iConsoleLevel > -1 && iLevel <= m_iConsoleLevel) {
		printf("- %s [L%i] -> ", szBuffer, iLevel);

		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);

		printf("\n");
	}

	if (m_iFileLevel > -1 && iLevel <= m_iFileLevel) {
		fprintf(m_fp, "- %s [L%i] -> ", szBuffer, iLevel);

		va_list args;
		va_start(args, fmt);
		vfprintf(m_fp, fmt, args);
		va_end(args);

		fprintf(m_fp, "\n");
	}
}
