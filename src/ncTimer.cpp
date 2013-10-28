#include <cstdlib> // for NULL
#include "ncTimer.h"

#if defined(_WIN32) || defined(__APPLE__)
	#if defined(WITH_SDL)
		#include <SDL/SDL_timer.h>
	#elif defined(WITH_GLFW)
		#if defined(__APPLE__)
			#include <GLFW/glfw.h>
		#else
			#include <GL/glfw.h>
		#endif
	#endif
#endif

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

#if !defined(_WIN32) && !defined(__APPLE__)
	struct timespec ncTimer::s_initTs;
	struct timeval ncTimer::s_initTv;
	bool ncTimer::s_bIsMonotonic = false;
#endif

	bool ncTimer::s_bIsInitialized = false;
	unsigned long int ncTimer::s_ulFrequency = 0L;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns elapsed time in seconds since base time
float ncTimer::Now()
{
#if defined(_WIN32) || defined(__APPLE__)
	#if defined(WITH_SDL)
		return SDL_GetTicks() / 1000.0f;
	#elif defined(WITH_GLFW)
		return glfwGetTime();
	#endif
#else
	if (s_bIsInitialized == false)
		Init();

	unsigned long long int counter = 0L;
	if (s_bIsMonotonic)
	{
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		counter = (now.tv_sec - s_initTs.tv_sec) * s_ulFrequency + (now.tv_nsec - s_initTs.tv_nsec);
	}
	else
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		counter = (now.tv_sec - s_initTv.tv_sec) * s_ulFrequency + (now.tv_usec - s_initTv.tv_usec);
	}
	return float(counter) / s_ulFrequency;
#endif
}

/// Puts the current thread to sleep for the specified number of seconds
void ncTimer::Sleep(float fS)
{
	unsigned int uMs = fS * 1000;

#if defined(_WIN32) || defined(__APPLE__)
	#if defined(WITH_SDL)
		SDL_Delay(uMs);
	#elif defined(WITH_GLFW)
		glfwSleep(dF);
	#endif
#else
	usleep(uMs);
#endif
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncTimer::Init()
{
#if !defined(_WIN32) && !defined(__APPLE__)
	if (clock_gettime(CLOCK_MONOTONIC, &s_initTs) == 0)
	{
		s_ulFrequency = 1000000000L;
		s_bIsMonotonic = true;
	}
	else
	{
		gettimeofday(&s_initTv, NULL);
		s_ulFrequency = 1000000L;
	}
#endif

	s_bIsInitialized = true;
}
