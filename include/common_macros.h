#ifndef NCINE_COMMON_MACROS
#define NCINE_COMMON_MACROS

#include <cstdlib> // for abort()
#include "ServiceLocator.h"

#ifdef __GNUC__
	#define FUNCTION __PRETTY_FUNCTION__
#elif _MSC_VER
	#define FUNCTION __FUNCTION__
#else
	#define FUNCTION __func__
#endif

#define LOGV_X(fmt, ...) ncine::theServiceLocator().logger().write(ncine::ILogger::LogLevel::VERBOSE, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGD_X(fmt, ...) ncine::theServiceLocator().logger().write(ncine::ILogger::LogLevel::DEBUG, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGI_X(fmt, ...) ncine::theServiceLocator().logger().write(ncine::ILogger::LogLevel::INFO, static_cast<const char *>("%s, -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGW_X(fmt, ...) ncine::theServiceLocator().logger().write(ncine::ILogger::LogLevel::WARN, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGE_X(fmt, ...) ncine::theServiceLocator().logger().write(ncine::ILogger::LogLevel::ERROR, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)
#define LOGF_X(fmt, ...) ncine::theServiceLocator().logger().write(ncine::ILogger::LogLevel::FATAL, static_cast<const char *>("%s -> " fmt), FUNCTION, ## __VA_ARGS__)

#define LOGV(fmt) ncine::theServiceLocator().logger().write(ncine::ILogger::LogLevel::VERBOSE, static_cast<const char *>("%s -> " fmt), FUNCTION)
#define LOGD(fmt) ncine::theServiceLocator().logger().write(ncine::ILogger::LogLevel::DEBUG, static_cast<const char *>("%s -> " fmt), FUNCTION)
#define LOGI(fmt) ncine::theServiceLocator().logger().write(ncine::ILogger::LogLevel::INFO, static_cast<const char *>("%s, -> " fmt), FUNCTION)
#define LOGW(fmt) ncine::theServiceLocator().logger().write(ncine::ILogger::LogLevel::WARN, static_cast<const char *>("%s -> " fmt), FUNCTION)
#define LOGE(fmt) ncine::theServiceLocator().logger().write(ncine::ILogger::LogLevel::ERROR, static_cast<const char *>("%s -> " fmt), FUNCTION)
#define LOGF(fmt) ncine::theServiceLocator().logger().write(ncine::ILogger::LogLevel::FATAL, static_cast<const char *>("%s -> " fmt), FUNCTION)

#ifdef NCINE_DEBUG
	#ifdef _MSC_VER
		#define BREAK() __debugbreak()
	#else
		#ifndef __has_builtin
			#define __has_builtin(x) 0
		#endif

		#if __has_builtin(__builtin_trap)
			#define BREAK() __builtin_trap()
		#else
			#define BREAK() ::abort()
		#endif
	#endif
#else
	#define BREAK() ::exit(EXIT_FAILURE)
#endif

// Fatal assert macros
#define FATAL_ASSERT_MSG_X(x, fmt, ...) \
do { \
	if (!(x)) \
	{ \
		LOGF_X(fmt, ## __VA_ARGS__); \
		BREAK(); \
	} \
} while (false)

#define FATAL_ASSERT_MSG(x, fmt) \
do { \
	if (!(x)) \
	{ \
		LOGF(fmt); \
		BREAK(); \
	} \
} while (false)

#define FATAL_ASSERT(x) \
do { \
	if (!(x)) \
	{ \
		LOGF("FATAL_ASSERT("#x")"); \
		BREAK(); \
	} \
} while (false)

// Fatal macros
#define FATAL_MSG_X(fmt, ...) \
do { \
	LOGF_X(fmt, ## __VA_ARGS__); \
	BREAK(); \
} while (false)

#define FATAL_MSG(fmt) \
do { \
	LOGF(fmt); \
	BREAK(); \
} while (false)

#define FATAL() \
do { \
	BREAK(); \
} while (false)

// Non-fatal assert macros
#ifdef NCINE_DEBUG
	#define ASSERT_MSG_X(x, fmt, ...) \
	do { \
		if (!(x)) \
		{ \
			LOGE_X(fmt, ## __VA_ARGS__); \
		} \
	} while (false)

	#define ASSERT_MSG(x, fmt) \
	do { \
		if (!(x)) \
		{ \
			LOGE(fmt); \
		} \
	} while (false)

	#define ASSERT(x) \
	do { \
		if (!(x)) \
		{ \
			LOGE("ASSERT("#x")"); \
		} \
	} while (false)
#else
	#define ASSERT_MSG_X(x, fmt, ...) do { (void)sizeof(x); } while (false)
	#define ASSERT_MSG(x, fmt) do { (void)sizeof(x); } while (false)
	#define ASSERT(x) do { (void)sizeof(x); } while (false)
#endif

#endif
