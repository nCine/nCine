#include <cstdio> // for NULL
#include "ncServiceLocator.h"

ncNullIndexer ncServiceLocator::m_nullIndexer;
ncIIndexer* ncServiceLocator::m_pIndexerService = &ncServiceLocator::m_nullIndexer;

ncNullLogger ncServiceLocator::m_nullLogger;
ncILogger* ncServiceLocator::m_pLoggerService = &ncServiceLocator::m_nullLogger;

ncNullAudioDevice ncServiceLocator::m_nullAudioDevice;
ncIAudioDevice* ncServiceLocator::m_pAudioDevice = &ncServiceLocator::m_nullAudioDevice;

ncNullThreadPool ncServiceLocator::m_nullThreadPool;
ncIThreadPool* ncServiceLocator::m_pThreadPool = &ncServiceLocator::m_nullThreadPool;

ncGfxCapabilities ncServiceLocator::m_gfxCapabilities;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncServiceLocator::RegisterIndexer(ncIIndexer* pService)
{
	if (pService == NULL)
		m_pIndexerService = &m_nullIndexer;
	else
		m_pIndexerService = pService;
}

void ncServiceLocator::RegisterLogger(ncILogger* pService)
{
	if (pService == NULL)
		m_pLoggerService = &m_nullLogger;
	else
		m_pLoggerService = pService;
}

void ncServiceLocator::RegisterAudioDevice(ncIAudioDevice* pService)
{
	if (pService == NULL)
		m_pAudioDevice = &m_nullAudioDevice;
	else
		m_pAudioDevice = pService;
}

void ncServiceLocator::RegisterThreadPool(ncIThreadPool* pService)
{
	if (pService == NULL)
		m_pThreadPool = &m_nullThreadPool;
	else
		m_pThreadPool = pService;
}

/// Deletes every registered service reestablishing null ones
void ncServiceLocator::UnregisterAll()
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncServiceLocator::UnregisterAll - Unregistering all services");

	if (m_pIndexerService != &m_nullIndexer)
	{
		delete m_pIndexerService;
		m_pIndexerService = &m_nullIndexer;
	}

	if (m_pAudioDevice != &m_nullAudioDevice)
	{
		delete m_pAudioDevice;
		m_pAudioDevice = &m_nullAudioDevice;
	}

	if (m_pThreadPool != &m_nullThreadPool)
	{
		delete m_pThreadPool;
		m_pThreadPool = &m_nullThreadPool;
	}

	// Logger unregistered at the end to give a last chance for logging
	if (m_pLoggerService != &m_nullLogger)
	{
		delete m_pLoggerService;
		m_pLoggerService = &m_nullLogger;
	}
}
