#ifndef CLASS_NCSERVICELOCATOR
#define CLASS_NCSERVICELOCATOR

#include "ncIIndexer.h"
#include "ncILogger.h"

/// Provides base services to requesting classes
class ncServiceLocator
{
public:
	static ncIIndexer& GetIndexer() { return *m_pIndexerService; }
	static void RegisterIndexer(ncIIndexer* service);

	static ncILogger& GetLogger() { return *m_pLoggerService; }
	static void RegisterLogger(ncILogger* service);

private:
	static ncIIndexer* m_pIndexerService;
	static ncNullIndexer m_nullIndexer;

	static ncILogger* m_pLoggerService;
	static ncNullLogger m_nullLogger;
};

#endif
