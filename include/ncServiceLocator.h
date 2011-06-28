#ifndef CLASS_NCSERVICELOCATOR
#define CLASS_NCSERVICELOCATOR

#include "ncILogger.h"

class ncServiceLocator
{
public:
	static ncILogger& GetLogger() { return *m_pLoggerService; }
	static void RegisterLogger(ncILogger* service);

private:
	static ncILogger* m_pLoggerService;
	static ncNullLogger m_nullLogger;
};

#endif
