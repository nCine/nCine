#ifndef CLASS_SERVICELOCATOR
#define CLASS_SERVICELOCATOR

#include "ILogger.h"

class ServiceLocator
{
public:
	static ILogger& GetLogger() { return *m_pLoggerService; }
	static void RegisterLogger(ILogger* service);

private:
	static ILogger* m_pLoggerService;
	static NullLogger m_nullLogger;
};

#endif
