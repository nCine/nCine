#ifndef CLASS_NCSERVICELOCATOR
#define CLASS_NCSERVICELOCATOR

#include "ncIIndexer.h"
#include "ncILogger.h"
#include "ncIAudioDevice.h"

/// Provides base services to requesting classes
class ncServiceLocator
{
public:
	/// Returns a pointer of the current indexer provider instance
	static ncIIndexer& GetIndexer() { return *m_pIndexerService; }
	/// Registers an indexer service provider
	static void RegisterIndexer(ncIIndexer* service);

	/// Returns a pointer of the current logger provider instance
	static ncILogger& GetLogger() { return *m_pLoggerService; }
	/// Registers a logger service provider
	static void RegisterLogger(ncILogger* service);

	/// Returns a pointer of the current audio device instance
	static ncIAudioDevice& GetAudioDevice() { return *m_pAudioDevice; }
	/// Registers an audio device provider
	static void RegisterAudioDevice(ncIAudioDevice* service);

private:
	static ncIIndexer* m_pIndexerService;
	static ncNullIndexer m_nullIndexer;

	static ncILogger* m_pLoggerService;
	static ncNullLogger m_nullLogger;

	static ncIAudioDevice* m_pAudioDevice;
	static ncNullAudioDevice m_nullAudioDevice;
};

#endif
