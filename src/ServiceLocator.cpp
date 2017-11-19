#include "common_macros.h"

namespace ncine {

ServiceLocator &theServiceLocator()
{
	static ServiceLocator instance;
	return instance;
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
///////////////////////////////////////////////////////////

ServiceLocator::ServiceLocator()
	: indexerService_(&nullIndexer_), loggerService_(&nullLogger_),
	  audioDevice_(&nullAudioDevice_), threadPool_(&nullThreadPool_),
	  gfxCapabilities_(&nullGfxCapabilities_)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ServiceLocator::registerIndexer(IIndexer *service)
{
	if (service == nullptr)
		indexerService_ = &nullIndexer_;
	else
		indexerService_ = service;
}

void ServiceLocator::registerLogger(ILogger *service)
{
	if (service == nullptr)
		loggerService_ = &nullLogger_;
	else
		loggerService_ = service;
}

void ServiceLocator::registerAudioDevice(IAudioDevice *service)
{
	if (service == nullptr)
		audioDevice_ = &nullAudioDevice_;
	else
		audioDevice_ = service;
}

void ServiceLocator::registerThreadPool(IThreadPool *service)
{
	if (service == nullptr)
		threadPool_ = &nullThreadPool_;
	else
		threadPool_ = service;
}

void ServiceLocator::registerGfxCapabilities(IGfxCapabilities *service)
{
	if (service == nullptr)
		gfxCapabilities_ = &nullGfxCapabilities_;
	else
		gfxCapabilities_ = service;
}

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

	if (gfxCapabilities_ != &nullGfxCapabilities_)
	{
		delete gfxCapabilities_;
		gfxCapabilities_ = &nullGfxCapabilities_;
	}

	// Logger unregistered at the end to give a last chance for logging
	if (loggerService_ != &nullLogger_)
	{
		delete loggerService_;
		loggerService_ = &nullLogger_;
	}
}

}
