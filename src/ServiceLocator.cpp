#include <cstdio> // for NULL
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

NullIndexer ServiceLocator::nullIndexer_;
IIndexer* ServiceLocator::indexerService_ = &ServiceLocator::nullIndexer_;

NullLogger ServiceLocator::nullLogger_;
ILogger* ServiceLocator::loggerService_ = &ServiceLocator::nullLogger_;

NullAudioDevice ServiceLocator::nullAudioDevice_;
IAudioDevice* ServiceLocator::audioDevice_ = &ServiceLocator::nullAudioDevice_;

NullThreadPool ServiceLocator::nullThreadPool_;
IThreadPool* ServiceLocator::threadPool_ = &ServiceLocator::nullThreadPool_;

GfxCapabilities ServiceLocator::gfxCapabilities_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ServiceLocator::registerIndexer(IIndexer* service)
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

void ServiceLocator::registerLogger(ILogger* service)
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

void ServiceLocator::registerAudioDevice(IAudioDevice* service)
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

void ServiceLocator::registerThreadPool(IThreadPool* service)
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
void ServiceLocator::unregisterAll()
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

}
