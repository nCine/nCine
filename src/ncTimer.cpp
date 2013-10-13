#include <cstdlib> // for NULL
#include "ncTimer.h"

#if defined(WITH_SDL)
	#include <SDL/SDL_timer.h>
#elif defined(WITH_GLFW)
	#if defined(__APPLE__)
		#include <GLFW/glfw.h>
	#else
		#include <GL/glfw.h>
	#endif
#endif

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

#if defined(__ANDROID__)
	#if defined(CLOCK_MONOTONIC)
	struct timespec ncTimer::m_initTs;
	#else
	struct timeval ncTimer::m_initTv;
	#endif
#endif

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTimer::ncTimer()
	: m_dStartTime(0.0)
{
#if defined(__ANDROID__)
	#if defined(CLOCK_MONOTONIC)
	clock_gettime(CLOCK_MONOTONIC, &m_initTs);
	#else
	gettimeofday(&m_initTv, NULL);
	#endif
#endif
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns elapsed time in milliseconds since base time (high-precision)
double ncTimer::PreciseNow()
{
#if defined(WITH_SDL)
	return SDL_GetTicks();
#elif defined(WITH_GLFW)
	return glfwGetTime() * 1000.0;
#elif defined(__ANDROID__)
	#if defined(CLOCK_MONOTONIC)
	struct timespec now;

	clock_gettime(CLOCK_MONOTONIC, &now);
	return (now.tv_sec  - m_initTs.tv_sec) * 1000.0 +
		(now.tv_nsec - m_initTs.tv_nsec)/1000000.0;
	#else
	struct timeval now;

	gettimeofday(&now, NULL);
	return (now.tv_sec - m_initTv.tv_sec) * 1000.0 +
		(now.tv_usec - m_initTv.tv_usec)/1000.0;
	#endif
#endif
}

/// Puts the current thread to sleep for the specified number of milliseconds
void ncTimer::Sleep(unsigned int uMs)
{
#if defined(WITH_SDL)
	SDL_Delay(uMs);
#elif defined(WITH_GLFW)
	glfwSleep(uMs * 0.001);
#elif defined(__ANDROID__)
	usleep(uMs);
#endif
}
