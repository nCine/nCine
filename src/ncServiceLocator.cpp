#include <cstdio> // for NULL
#include "ncServiceLocator.h"

ncNullIndexer ncServiceLocator::nullIndexer_;
ncIIndexer* ncServiceLocator::indexerService_ = &ncServiceLocator::nullIndexer_;

ncNullLogger ncServiceLocator::nullLogger_;
ncILogger* ncServiceLocator::loggerService_ = &ncServiceLocator::nullLogger_;

ncNullAudioDevice ncServiceLocator::nullAudioDevice_;
ncIAudioDevice* ncServiceLocator::audioDevice_ = &ncServiceLocator::nullAudioDevice_;

ncNullThreadPool ncServiceLocator::nullThreadPool_;
ncIThreadPool* ncServiceLocator::threadPool_ = &ncServiceLocator::nullThreadPool_;

ncGfxCapabilities ncServiceLocator::gfxCapabilities_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncServiceLocator::registerIndexer(ncIIndexer* service)
{
	if (service == NULL)
	{
		indexerService_ = &nullIndexer_;
	}
	else
	{
		indexerService_ = service;
	}
}

void ncServiceLocator::registerLogger(ncILogger* service)
{
	if (service == NULL)
	{
		loggerService_ = &nullLogger_;
	}
	else
	{
		loggerService_ = service;
	}
}

void ncServiceLocator::registerAudioDevice(ncIAudioDevice* service)
{
	if (service == NULL)
	{
		audioDevice_ = &nullAudioDevice_;
	}
	else
	{
		audioDevice_ = service;
	}
}

void ncServiceLocator::registerThreadPool(ncIThreadPool* service)
{
	if (service == NULL)
	{
		threadPool_ = &nullThreadPool_;
	}
	else
	{
		threadPool_ = service;
	}
}

/// Deletes every registered service reestablishing null ones
void ncServiceLocator::unregisterAll()
{
	LOGI("Unregistering all services");

	if (indexerService_ != &nullIndexer_)
	{
		delete indexerService_;
		indexerService_ = &nullIndexer_;
	}

	if (audioDevice_ != &nullAudioDevice_)
	{
		delete audioDevice_;
		audioDevice_ = &nullAudioDevice_;
	}

	if (threadPool_ != &nullThreadPool_)
	{
		delete threadPool_;
		threadPool_ = &nullThreadPool_;
	}

	// Logger unregistered at the end to give a last chance for logging
	if (loggerService_ != &nullLogger_)
	{
		delete loggerService_;
		loggerService_ = &nullLogger_;
	}
}
