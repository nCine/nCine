#include "ncServiceLocator.h"

ncNullLogger ncServiceLocator::m_nullLogger;
ncILogger* ncServiceLocator::m_pLoggerService = &ncServiceLocator::m_nullLogger;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncServiceLocator::RegisterLogger(ncILogger* service)
{
	if (service == 0)
		m_pLoggerService = &m_nullLogger;
	else
		m_pLoggerService = service;
}
