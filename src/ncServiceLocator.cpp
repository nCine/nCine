#include <cstdio> // for NULL
#include "ncServiceLocator.h"

ncNullIndexer ncServiceLocator::m_nullIndexer;
ncIIndexer* ncServiceLocator::m_pIndexerService = &ncServiceLocator::m_nullIndexer;

ncNullLogger ncServiceLocator::m_nullLogger;
ncILogger* ncServiceLocator::m_pLoggerService = &ncServiceLocator::m_nullLogger;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncServiceLocator::RegisterIndexer(ncIIndexer* service)
{
	if (service == NULL)
		m_pIndexerService = &m_nullIndexer;
	else
		m_pIndexerService = service;
}

void ncServiceLocator::RegisterLogger(ncILogger* service)
{
	if (service == NULL)
		m_pLoggerService = &m_nullLogger;
	else
		m_pLoggerService = service;
}
