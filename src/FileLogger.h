#ifndef CLASS_FILELOGGER
#define CLASS_FILELOGGER

#include <cstdio>
#include "ILogger.h"

class FileLogger : public ILogger
{
private:
	FILE *m_fp;
	int m_iConsoleLevel;
	int m_iFileLevel;
public:
	FileLogger(const char *filename, int iConsoleLevel, int iFileLevel);
	~FileLogger();
	virtual void Write(int iLevel, const char* fmt, ...);
};

#endif
