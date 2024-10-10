#include "common_macros.h"
#include "SerialJobSystem.h"

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
	serialJobSystem_ = nctl::makeUnique<SerialJobSystem>();
	jobSystem_ = serialJobSystem_.get();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ServiceLocator::registerIndexer(nctl::UniquePtr<IIndexer> service)
{
	registeredIndexer_ = nctl::move(service);
	indexerService_ = registeredIndexer_.get();
}

void ServiceLocator::unregisterIndexer()
{
	registeredIndexer_.reset(nullptr);
	indexerService_ = &nullIndexer_;
}

void ServiceLocator::registerLogger(nctl::UniquePtr<ILogger> service)
{
	registeredLogger_ = nctl::move(service);
	loggerService_ = registeredLogger_.get();
}

void ServiceLocator::unregisterLogger()
{
	registeredLogger_.reset(nullptr);
	loggerService_ = &nullLogger_;
}

void ServiceLocator::registerAudioDevice(nctl::UniquePtr<IAudioDevice> service)
{
	registeredAudioDevice_ = nctl::move(service);
	audioDevice_ = registeredAudioDevice_.get();
}

void ServiceLocator::unregisterAudioDevice()
{
	registeredAudioDevice_.reset(nullptr);
	audioDevice_ = &nullAudioDevice_;
}

void ServiceLocator::registerThreadPool(nctl::UniquePtr<IThreadPool> service)
{
	registeredThreadPool_ = nctl::move(service);
	threadPool_ = registeredThreadPool_.get();
}

void ServiceLocator::unregisterThreadPool()
{
	registeredThreadPool_.reset(nullptr);
	threadPool_ = &nullThreadPool_;
}

void ServiceLocator::registerJobSystem(nctl::UniquePtr<ncine::IJobSystem> service)
{
	registeredJobSystem_ = nctl::move(service);
	jobSystem_ = registeredJobSystem_.get();
}

void ServiceLocator::unregisterJobSystem()
{
	registeredJobSystem_.reset(nullptr);
	jobSystem_ = serialJobSystem_.get();
}

void ServiceLocator::registerGfxCapabilities(nctl::UniquePtr<ncine::IGfxCapabilities> service)
{
	registeredGfxCapabilities_ = nctl::move(service);
	gfxCapabilities_ = registeredGfxCapabilities_.get();
}

void ServiceLocator::unregisterGfxCapabilities()
{
	registeredGfxCapabilities_.reset(nullptr);
	gfxCapabilities_ = &nullGfxCapabilities_;
}

void ServiceLocator::unregisterAll()
{
	LOGI("Unregistering all services");

	registeredIndexer_.reset(nullptr);
	indexerService_ = &nullIndexer_;

	registeredAudioDevice_.reset(nullptr);
	audioDevice_ = &nullAudioDevice_;

	registeredThreadPool_.reset(nullptr);
	threadPool_ = &nullThreadPool_;

	registeredJobSystem_.reset(nullptr);
	jobSystem_ = serialJobSystem_.get();

	registeredGfxCapabilities_.reset(nullptr);
	gfxCapabilities_ = &nullGfxCapabilities_;

	// Logger unregistered at the end to give a last chance for logging
	registeredLogger_.reset(nullptr);
	loggerService_ = &nullLogger_;
}

}
