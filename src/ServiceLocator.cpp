#include "ServiceLocator.h"

NullLogger ServiceLocator::m_nullLogger;
ILogger* ServiceLocator::m_pLoggerService = &ServiceLocator::m_nullLogger;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ServiceLocator::RegisterLogger(ILogger* service)
{
	if (service == 0)
		m_pLoggerService = &m_nullLogger;
	else
		m_pLoggerService = service;
}
