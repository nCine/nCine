#ifdef __ANDROID__
	#include <sys/time.h>
#else
	#include <SDL/SDL_timer.h>
#endif
#include "ncTimer.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Reset the timer
void ncTimer::Reset()
{
	m_bRunning = false;
	m_uStartTime = m_uStopTime = 0;
	m_uTotal = 0;
}

/// Start the timer
void ncTimer::Start()
{
	Reset();
	Continue();
}

/// Stop the timer without resetting it
void ncTimer::Stop()
{
	m_bRunning = false;
	m_uStopTime = GetNow();
	m_uTotal += m_uStopTime - m_uStartTime;
}

/// Resume the timer
void ncTimer::Continue()
{
	m_bRunning = true;
	m_uStartTime = GetNow();
}

/// Return (stop - start) or (now-start) time interval
/// depending on whether the timer is currently running or not
unsigned long int ncTimer::GetInterval()
{
	if (m_bRunning) {
		return GetNow() - m_uStartTime;
	}
	else {
		return m_uStopTime - m_uStartTime;
	}
}

/// Return total elapsed time
unsigned long int ncTimer::GetTotal()
{
	return m_uTotal;
}

/// Return current time in milliseconds from epoch
unsigned long int ncTimer::GetNow()
{
#ifdef __ANDROID__
	struct timeval tv;

	gettimeofday(&tv, NULL);
	unsigned long int now = (unsigned long int)(tv.tv_sec*1000 + tv.tv_usec/1000.0);

	return now;
#else
	return SDL_GetTicks();
#endif
}
