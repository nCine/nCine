#include <cstdlib> // for NULL
#include "ncTimer.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTimer::ncTimer()
	: m_bRunning(false), m_uStartTime(0), m_uStopTime(0), m_uTotal(0)
{
	gettimeofday(&m_initTv, NULL);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Resets the timer
void ncTimer::Reset()
{
	m_bRunning = false;
	m_uStartTime = m_uStopTime = 0;
	m_uTotal = 0;
}

/// Starts the timer
void ncTimer::Start()
{
	Reset();
	Continue();
}

/// Stops the timer without resetting it
void ncTimer::Stop()
{
	m_bRunning = false;
	m_uStopTime = Now();
	m_uTotal += m_uStopTime - m_uStartTime;
}

/// Resumes the timer
void ncTimer::Continue()
{
	m_bRunning = true;
	m_uStartTime = Now();
}

/** Returns (stop-start) or (now-start) time interval
 depending on whether the timer is currently running or not */
unsigned long int ncTimer::Interval()
{
	if (m_bRunning)
		return Now() - m_uStartTime;
	else
		return m_uStopTime - m_uStartTime;
}

/// Returns total elapsed time
unsigned long int ncTimer::Total()
{
	return m_uTotal;
}

/// Returns current time in milliseconds from init time
unsigned long int ncTimer::Now()
{
	struct timeval now;

	gettimeofday(&now, NULL);
	unsigned long int ticks = (now.tv_sec - m_initTv.tv_sec) * 1000 +
		(now.tv_usec - m_initTv.tv_usec)/1000;

	return ticks;
}
