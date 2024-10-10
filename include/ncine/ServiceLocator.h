#ifndef CLASS_NCINE_SERVICELOCATOR
#define CLASS_NCINE_SERVICELOCATOR

#ifdef __ANDROID__
	#include <stdarg.h>
	#include <android/log.h>
#else
	#include <cstdarg>
#endif

#include "IIndexer.h"
#include "ILogger.h"
#include "IAudioDevice.h"
#include "IThreadPool.h"
#include "IJobSystem.h"
#include "IGfxCapabilities.h"

namespace ncine {

/// Provides base services to requesting classes
/*! It has memory ownership on service classes. */
class DLL_PUBLIC ServiceLocator
{
  public:
	/// Returns a reference to the current indexer provider instance
	IIndexer &indexer() { return *indexerService_; }
	/// Registers an indexer service provider
	void registerIndexer(nctl::UniquePtr<IIndexer> service);
	/// Unregisters the index service provider and reinstates the null one
	void unregisterIndexer();

	/// Returns a reference to the current logger provider instance
	ILogger &logger() { return *loggerService_; }
	/// Registers a logger service provider
	void registerLogger(nctl::UniquePtr<ILogger> service);
	/// Unregisters the logger service provider and reinstates the null one
	void unregisterLogger();

	/// Returns a reference to the current audio device instance
	IAudioDevice &audioDevice() { return *audioDevice_; }
	/// Registers an audio device provider
	void registerAudioDevice(nctl::UniquePtr<IAudioDevice> service);
	/// Unregisters the audio device provider and reinstates the null one
	void unregisterAudioDevice();

	/// Returns a reference to the current thread pool instance
	IThreadPool &threadPool() { return *threadPool_; }
	/// Registers a thread pool provider
	void registerThreadPool(nctl::UniquePtr<IThreadPool> service);
	/// Unregisters the thread pool provider and reinstates the null one
	void unregisterThreadPool();

	/// Returns a reference to the current job system instance
	IJobSystem &jobSystem() { return *jobSystem_; }
	/// Registers a tjob system provider
	void registerJobSystem(nctl::UniquePtr<IJobSystem> service);
	/// Unregisters the job system provider and reinstates the null one
	void unregisterJobSystem();

	/// Returns a reference to the current graphics capabilities instance
	const IGfxCapabilities &gfxCapabilities() { return *gfxCapabilities_; }
	/// Registers a graphics capabilities provider
	void registerGfxCapabilities(nctl::UniquePtr<IGfxCapabilities> service);
	/// Unregisters the graphics capabilitiesprovider and reinstates the null one
	void unregisterGfxCapabilities();

	/// Unregisters every registered service and reinstates null ones
	void unregisterAll();

  private:
	IIndexer *indexerService_;
	nctl::UniquePtr<IIndexer> registeredIndexer_;
	NullIndexer nullIndexer_;

	ILogger *loggerService_;
	nctl::UniquePtr<ILogger> registeredLogger_;
	NullLogger nullLogger_;

	IAudioDevice *audioDevice_;
	nctl::UniquePtr<IAudioDevice> registeredAudioDevice_;
	NullAudioDevice nullAudioDevice_;

	IThreadPool *threadPool_;
	nctl::UniquePtr<IThreadPool> registeredThreadPool_;
	NullThreadPool nullThreadPool_;

	IJobSystem *jobSystem_;
	nctl::UniquePtr<IJobSystem> registeredJobSystem_;
	nctl::UniquePtr<IJobSystem> serialJobSystem_;

	IGfxCapabilities *gfxCapabilities_;
	nctl::UniquePtr<IGfxCapabilities> registeredGfxCapabilities_;
	NullGfxCapabilities nullGfxCapabilities_;

	ServiceLocator();
	/// Deleted copy constructor
	ServiceLocator(const ServiceLocator &) = delete;
	/// Deleted assignment operator
	ServiceLocator &operator=(const ServiceLocator &) = delete;

	friend DLL_PUBLIC ServiceLocator &theServiceLocator();
};

/// Meyers' Singleton
DLL_PUBLIC ServiceLocator &theServiceLocator();

}

#endif
