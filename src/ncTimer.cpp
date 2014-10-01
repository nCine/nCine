#include <cstdlib> // for NULL
#include "ncTimer.h"

#ifdef _WIN32
	#include <winsync.h>
	#include <profileapi.h>
#elif __APPLE__
	#include <mach/mach_time.h>
#else
	#include <time.h> // for clock_gettime()
	#include <sys/time.h> // for gettimeofday()
	#include <unistd.h>
#endif

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

#ifdef _WIN32
	bool ncTimer::s_bHasPerfCounter = false;
#elif !defined(__APPLE__)
	bool ncTimer::s_bHasMonotonicClock = false;
#endif

bool ncTimer::s_bIsInitialized = false;
unsigned long int ncTimer::s_ulFrequency = 0L;
unsigned long long int ncTimer::s_ullBaseCount = 0LL;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Empty constructor
ncTimer::ncTimer()
	: m_ullStartTime(0LL)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns elapsed time in seconds since base time
float ncTimer::Now()
{
	return float(Counter() - s_ullBaseCount) / s_ulFrequency;
}

/// Puts the current thread to sleep for the specified number of seconds
void ncTimer::Sleep(float fS)
{
	// From seconds to milliseconds
	unsigned int uMs = fS * 1000;

#if defined(_WIN32)
	SleepEx(uMs, FALSE);
#else
	usleep(uMs);
#endif
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes the static fields
void ncTimer::Init()
{
#ifdef _WIN32
	if (QueryPerformanceFrequency((LARGE_INTEGER *) &s_ulFrequency))
	{
		s_bHasPerfCounter = true;
	}
	else
	{
		s_ulFrequency = 1000L;
	}
#elif __APPLE__
	mach_timebase_info_data_t info;
	mach_timebase_info(&info);

	s_ulFrequency = (info.denom * 1.0e9L) / info.numer;
#else
	struct timespec resolution;
	if (clock_getres(CLOCK_MONOTONIC, &resolution) == 0)
	{
		s_ulFrequency = 1.0e9L;
		s_bHasMonotonicClock = true;
	}
	else
	{
		s_ulFrequency = 1.0e6L;
	}
#endif

	// Counter() must be called after setting the flag
	s_bIsInitialized = true;
	s_ullBaseCount = Counter();
}

/// Returns current value of the counter
unsigned long long int ncTimer::Counter()
{
	if (s_bIsInitialized == false)
	{
		Init();
	}

	unsigned long long int ullCounter = 0LL;

#ifdef _WIN32
	if (s_bHasPerfCounter)
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &ullCounter);
	}
	else
	{
		ullCounter = GetTickCount();
	}
#elif __APPLE__
	ullCounter = mach_absolute_time();
#else
	if (s_bHasMonotonicClock)
	{
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		ullCounter = (unsigned long long int)now.tv_sec * s_ulFrequency + (unsigned long long int)now.tv_nsec;
	}
	else
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		ullCounter = now.tv_sec * s_ulFrequency + now.tv_usec;
	}
#endif

	return ullCounter;
}
