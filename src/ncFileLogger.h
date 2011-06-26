#ifndef CLASS_NCFILELOGGER
#define CLASS_NCFILELOGGER

#include <cstdio>
#include "ncILogger.h"

class ncFileLogger : public ncILogger
{
private:
	FILE *m_fp;
	int m_iConsoleLevel;
	int m_iFileLevel;
public:
	ncFileLogger(const char *filename, int iConsoleLevel, int iFileLevel);
	~ncFileLogger();
	virtual void Write(int iLevel, const char* fmt, ...);
};

#endif
