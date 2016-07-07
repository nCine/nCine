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
#include "IGfxCapabilities.h"

namespace ncine {

/// Provides base services to requesting classes
/*! It has memory ownership on service classes. */
class DLL_PUBLIC ServiceLocator
{
  public:
	/// Returns a reference to the current indexer provider instance
	IIndexer& indexer() { return *indexerService_; }
	/// Registers an indexer service provider
	void registerIndexer(IIndexer* service);

	/// Returns a reference to the current logger provider instance
	ILogger& logger() { return *loggerService_; }
	/// Registers a logger service provider
	void registerLogger(ILogger* service);

	/// Returns a reference to the current audio device instance
	IAudioDevice& audioDevice() { return *audioDevice_; }
	/// Registers an audio device provider
	void registerAudioDevice(IAudioDevice* service);

	/// Returns a reference to the current thread pool instance
	IThreadPool& threadPool() { return *threadPool_; }
	/// Registers a thread pool provider
	void registerThreadPool(IThreadPool* service);

	/// Returns a reference to the current graphics capabilities instance
	const IGfxCapabilities& gfxCapabilities() { return *gfxCapabilities_; }
	/// Registers a graphics capabilities provider
	void registerGfxCapabilities(IGfxCapabilities* service);

	/// Deletes every registered service and reestablishes null ones
	void unregisterAll();

  private:
	IIndexer* indexerService_;
	NullIndexer nullIndexer_;

	ILogger* loggerService_;
	NullLogger nullLogger_;

	IAudioDevice* audioDevice_;
	NullAudioDevice nullAudioDevice_;

	IThreadPool* threadPool_;
	NullThreadPool nullThreadPool_;

	IGfxCapabilities* gfxCapabilities_;
	NullGfxCapabilities nullGfxCapabilities_;

	ServiceLocator();
	/// Private copy constructor
	ServiceLocator(const ServiceLocator&);
	/// Private assignment operator
	ServiceLocator& operator=(const ServiceLocator&);

	friend DLL_PUBLIC ServiceLocator& theServiceLocator();
};

/// Meyers' Singleton
DLL_PUBLIC ServiceLocator& theServiceLocator();

}

#ifdef __GNUC__
	#define FUNCTION __PRETTY_FUNCTION__
#elif _MSC_VER
	#define FUNCTION __FUNCTION__
#else
	#define FUNCTION __func__
#endif

#define LOGV_X(fmt, ...) ncine::theServiceLocator().logger().write(ncine::ILogger::LOG_VERBOSE, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGD_X(fmt, ...) ncine::theServiceLocator().logger().write(ncine::ILogger::LOG_DEBUG, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGI_X(fmt, ...) ncine::theServiceLocator().logger().write(ncine::ILogger::LOG_INFO, static_cast<const char *>("%s, -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGW_X(fmt, ...) ncine::theServiceLocator().logger().write(ncine::ILogger::LOG_WARN, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGE_X(fmt, ...) ncine::theServiceLocator().logger().write(ncine::ILogger::LOG_ERROR, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGF_X(fmt, ...) ncine::theServiceLocator().logger().write(ncine::ILogger::LOG_FATAL, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)

#define LOGV(fmt) ncine::theServiceLocator().logger().write(ncine::ILogger::LOG_VERBOSE, static_cast<const char *>("%s -> " fmt), FUNCTION)
#define LOGD(fmt) ncine::theServiceLocator().logger().write(ncine::ILogger::LOG_DEBUG, static_cast<const char *>("%s -> " fmt), FUNCTION)
#define LOGI(fmt) ncine::theServiceLocator().logger().write(ncine::ILogger::LOG_INFO, static_cast<const char *>("%s, -> " fmt), FUNCTION)
#define LOGW(fmt) ncine::theServiceLocator().logger().write(ncine::ILogger::LOG_WARN, static_cast<const char *>("%s -> " fmt), FUNCTION)
#define LOGE(fmt) ncine::theServiceLocator().logger().write(ncine::ILogger::LOG_ERROR, static_cast<const char *>("%s -> " fmt), FUNCTION)
#define LOGF(fmt) ncine::theServiceLocator().logger().write(ncine::ILogger::LOG_FATAL, static_cast<const char *>("%s -> " fmt), FUNCTION)

#endif
