//#include <sys/time.h>
#include <SDL/SDL_timer.h>
#include "Timer.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Reset the timer
void Timer::Reset()
{
	m_bRunning = false;
	m_uiStartTime = m_uiStopTime = 0;
	m_uiTotal = 0;
}

/// Start the timer
void Timer::Start()
{
	Reset();
	Continue();
}

/// Stop the timer without resetting it
void Timer::Stop()
{
	m_bRunning = false;
	m_uiStopTime = GetNow();
	m_uiTotal += m_uiStopTime - m_uiStartTime;
}

/// Resume the timer
void Timer::Continue()
{
	m_bRunning = true;
	m_uiStartTime = GetNow();
}

/// Return (stop - start) or (now-start) time interval
/// depending on whether the timer is currently running or not
unsigned long int Timer::GetInterval()
{
	if (m_bRunning) {
		return GetNow() - m_uiStartTime;
	}
	else {
		return m_uiStopTime - m_uiStartTime;
	}
}

/// Return total elapsed time
unsigned long int Timer::GetTotal()
{
	return m_uiTotal;
}

/// Return current time in milliseconds from epoch
unsigned long int Timer::GetNow()
{
/*
	timeval tp;

	gettimeofday(&tp, 0);
	unsigned long int now = (unsigned long int)(tp.tv_sec*1000.0 + tp.tv_usec/1000.0);

	return now;
*/

	return SDL_GetTicks();
}
