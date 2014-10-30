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
#include "GfxCapabilities.h"

namespace ncine {

/// Provides base services to requesting classes
/*! It has memory ownership on service classes */
class ServiceLocator
{
  public:
	/// Returns a pointer to the current indexer provider instance
	static IIndexer& indexer() { return *indexerService_; }
	/// Registers an indexer service provider
	static void registerIndexer(IIndexer* service);

	/// Returns a pointer to the current logger provider instance
	static ILogger& logger() { return *loggerService_; }
	/// Registers a logger service provider
	static void registerLogger(ILogger* service);

	/// Returns a pointer to the current audio device instance
	static IAudioDevice& audioDevice() { return *audioDevice_; }
	/// Registers an audio device provider
	static void registerAudioDevice(IAudioDevice* service);

	/// Returns a pointer to the current thread pool instance
	static IThreadPool& threadPool() { return *threadPool_; }
	/// Registers an audio device provider
	static void registerThreadPool(IThreadPool* service);

	/// Returns a constant reference to the graphics capabilities class
	/*! There is no registering process for the GfxCapabilities class, no
	 *  interface and no null version: it is unique and no-op before initialization.
	 */
	static const GfxCapabilities& gfxCapabilities() { return gfxCapabilities_; }

	// Deletes every registered service reestablishing null ones
	static void unregisterAll();

  private:
	static IIndexer* indexerService_;
	static NullIndexer nullIndexer_;

	static ILogger* loggerService_;
	static NullLogger nullLogger_;

	static IAudioDevice* audioDevice_;
	static NullAudioDevice nullAudioDevice_;

	static IThreadPool* threadPool_;
	static NullThreadPool nullThreadPool_;

	static GfxCapabilities gfxCapabilities_;
};

}

#ifdef __GNUC__
	#define FUNCTION __PRETTY_FUNCTION__
#elif _MSC_VER
		#define FUNCTION __FUNCTION__
#else
		#define FUNCTION __func__
#endif

#define LOGV_X(fmt, ...) ncine::ServiceLocator::logger().write(ncine::ILogger::LOG_VERBOSE, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGD_X(fmt, ...) ncine::ServiceLocator::logger().write(ncine::ILogger::LOG_DEBUG, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGI_X(fmt, ...) ncine::ServiceLocator::logger().write(ncine::ILogger::LOG_INFO, static_cast<const char *>("%s, -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGW_X(fmt, ...) ncine::ServiceLocator::logger().write(ncine::ILogger::LOG_WARN, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGE_X(fmt, ...) ncine::ServiceLocator::logger().write(ncine::ILogger::LOG_ERROR, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGF_X(fmt, ...) ncine::ServiceLocator::logger().write(ncine::ILogger::LOG_FATAL, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)

#define LOGV(fmt) ncine::ServiceLocator::logger().write(ncine::ILogger::LOG_VERBOSE, static_cast<const char *>("%s -> " fmt), FUNCTION)
#define LOGD(fmt) ncine::ServiceLocator::logger().write(ncine::ILogger::LOG_DEBUG, static_cast<const char *>("%s -> " fmt), FUNCTION)
#define LOGI(fmt) ncine::ServiceLocator::logger().write(ncine::ILogger::LOG_INFO, static_cast<const char *>("%s, -> " fmt), FUNCTION)
#define LOGW(fmt) ncine::ServiceLocator::logger().write(ncine::ILogger::LOG_WARN, static_cast<const char *>("%s -> " fmt), FUNCTION)
#define LOGE(fmt) ncine::ServiceLocator::logger().write(ncine::ILogger::LOG_ERROR, static_cast<const char *>("%s -> " fmt), FUNCTION)
#define LOGF(fmt) ncine::ServiceLocator::logger().write(ncine::ILogger::LOG_FATAL, static_cast<const char *>("%s -> " fmt), FUNCTION)

#endif
