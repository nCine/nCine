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
	static ncIIndexer& Indexer() { return *m_pIndexerService; }
	/// Registers an indexer service provider
	static void RegisterIndexer(ncIIndexer* pService);

	/// Returns a pointer to the current logger provider instance
	static ncILogger& Logger() { return *m_pLoggerService; }
	/// Registers a logger service provider
	static void RegisterLogger(ncILogger* pService);

	/// Returns a pointer to the current audio device instance
	static ncIAudioDevice& AudioDevice() { return *m_pAudioDevice; }
	/// Registers an audio device provider
	static void RegisterAudioDevice(ncIAudioDevice* pService);

	/// Returns a pointer to the current thread pool instance
	static ncIThreadPool& ThreadPool() { return *m_pThreadPool; }
	/// Registers an audio device provider
	static void RegisterThreadPool(ncIThreadPool* pService);

	/// Returns a constant reference to the graphics capabilities class
	/*! There is no registering process for the ncGfxCapabilities class, no
	 *  interface and no null version: it is unique and no-op before initialization.
	 */
	static const ncGfxCapabilities& GfxCapabilities() { return m_gfxCapabilities; }

	// Deletes every registered service reestablishing null ones
	static void UnregisterAll();

private:
	static ncIIndexer* m_pIndexerService;
	static ncNullIndexer m_nullIndexer;

	static ncILogger* m_pLoggerService;
	static ncNullLogger m_nullLogger;

	static ncIAudioDevice* m_pAudioDevice;
	static ncNullAudioDevice m_nullAudioDevice;

	static ncIThreadPool* m_pThreadPool;
	static ncNullThreadPool m_nullThreadPool;

	static ncGfxCapabilities m_gfxCapabilities;
};

#endif
