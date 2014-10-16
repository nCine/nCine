#ifndef CLASS_NCSERVICELOCATOR
#define CLASS_NCSERVICELOCATOR

#include "ncIIndexer.h"
#include "ncILogger.h"
#include "ncIAudioDevice.h"
#include "ncIThreadPool.h"
#include "ncGfxCapabilities.h"

/// Provides base services to requesting classes
/*! It has memory ownership on service classes */
class ncServiceLocator
{
  public:
	/// Returns a pointer to the current indexer provider instance
	static ncIIndexer& indexer() { return *indexerService_; }
	/// Registers an indexer service provider
	static void registerIndexer(ncIIndexer* service);

	/// Returns a pointer to the current logger provider instance
	static ncILogger& logger() { return *loggerService_; }
	/// Registers a logger service provider
	static void registerLogger(ncILogger* service);

	/// Returns a pointer to the current audio device instance
	static ncIAudioDevice& audioDevice() { return *audioDevice_; }
	/// Registers an audio device provider
	static void registerAudioDevice(ncIAudioDevice* service);

	/// Returns a pointer to the current thread pool instance
	static ncIThreadPool& threadPool() { return *threadPool_; }
	/// Registers an audio device provider
	static void registerThreadPool(ncIThreadPool* service);

	/// Returns a constant reference to the graphics capabilities class
	/*! There is no registering process for the ncGfxCapabilities class, no
	 *  interface and no null version: it is unique and no-op before initialization.
	 */
	static const ncGfxCapabilities& gfxCapabilities() { return gfxCapabilities_; }

	// Deletes every registered service reestablishing null ones
	static void unregisterAll();

  private:
	static ncIIndexer* indexerService_;
	static ncNullIndexer nullIndexer_;

	static ncILogger* loggerService_;
	static ncNullLogger nullLogger_;

	static ncIAudioDevice* audioDevice_;
	static ncNullAudioDevice nullAudioDevice_;

	static ncIThreadPool* threadPool_;
	static ncNullThreadPool nullThreadPool_;

	static ncGfxCapabilities gfxCapabilities_;
};

#endif
