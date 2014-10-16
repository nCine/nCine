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
	bool ncTimer::hasPerfCounter_ = false;
#elif !defined(__APPLE__)
	bool ncTimer::hasMonotonicClock_ = false;
#endif

bool ncTimer::isInitialized_ = false;
unsigned long int ncTimer::frequency_ = 0L;
unsigned long long int ncTimer::baseCount_ = 0LL;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Empty constructor
ncTimer::ncTimer()
	: startTime_(0LL)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns elapsed time in seconds since base time
float ncTimer::now()
{
	return float(counter() - baseCount_) / frequency_;
}

/// Puts the current thread to sleep for the specified number of seconds
void ncTimer::sleep(float seconds)
{
	// From seconds to milliseconds
	unsigned int milliseconds = seconds * 1000;

#if defined(_WIN32)
	SleepEx(milliseconds, FALSE);
#else
	usleep(milliseconds);
#endif
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes the static fields
void ncTimer::init()
{
#ifdef _WIN32
	if (QueryPerformanceFrequency((LARGE_INTEGER *) &frequency_))
	{
		hasPerfCounter_ = true;
	}
	else
	{
		frequency_ = 1000L;
	}
#elif __APPLE__
	mach_timebase_info_data_t info;
	mach_timebase_info(&info);

	frequency_ = (info.denom * 1.0e9L) / info.numer;
#else
	struct timespec resolution;
	if (clock_getres(CLOCK_MONOTONIC, &resolution) == 0)
	{
		frequency_ = 1.0e9L;
		hasMonotonicClock_ = true;
	}
	else
	{
		frequency_ = 1.0e6L;
	}
#endif

	// Counter() must be called after setting the flag
	isInitialized_ = true;
	baseCount_ = counter();
}

/// Returns current value of the counter
unsigned long long int ncTimer::counter()
{
	if (isInitialized_ == false)
	{
		init();
	}

	unsigned long long int counter = 0LL;

#ifdef _WIN32
	if (hasPerfCounter_)
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &counter);
	}
	else
	{
		counter = GetTickCount();
	}
#elif __APPLE__
	counter = mach_absolute_time();
#else
	if (hasMonotonicClock_)
	{
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		counter = (unsigned long long int)now.tv_sec * frequency_ + (unsigned long long int)now.tv_nsec;
	}
	else
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		counter = now.tv_sec * frequency_ + now.tv_usec;
	}
#endif

	return counter;
}
