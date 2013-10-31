#include <cstdio>
#include "ncFrameTimer.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! Constructs a timer which calculates average FPS every dAvgInterval
	seconds and prints in the log every dLogInterval seconds
*/
ncFrameTimer::ncFrameTimer(float fLogInterval, float fAvgInterval)
	: m_fLogInterval(fLogInterval), m_fAvgInterval(fAvgInterval),
	  m_fFrameInterval(0.0f), m_ulAvgNFrames(0L), m_ulLogNFrames(0L),
	  m_fLastAvgUpdate(0.0f), m_fLastLogUpdate(0.0f), m_fFps(0.0f)
{
	// To prevent the overflow of Interval() on the very first call of AddFrame()
	Start();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Adds a frame to the counter and calculates the interval since the previous one
void ncFrameTimer::AddFrame()
{
	m_fFrameInterval = ncTimer::Interval();

	m_ulAvgNFrames++;
	m_ulLogNFrames++;

	// Update the FPS average calculation every m_fAvgInterval seconds
	if (m_fAvgInterval > 0.0f && (Now() - m_fLastAvgUpdate > m_fAvgInterval))
	{
		m_fFps = float(m_ulAvgNFrames) / (Now() - m_fLastAvgUpdate);

		m_ulAvgNFrames = 0L;
		m_fLastAvgUpdate = Now();
	}


	// Log number of frames and FPS every m_fLogInterval seconds
	if (m_fLogInterval > 0.0f && m_ulAvgNFrames != 0 && (Now() - m_fLastLogUpdate > m_fLogInterval))
	{
		m_fFps = float(m_ulLogNFrames) / m_fLogInterval;
		// Milliseconds per frame
		float fMSPF = (m_fLogInterval * 1000.0f) / float(m_ulLogNFrames);
		ncServiceLocator::Logger().Write(ncILogger::LOG_VERBOSE, (const char *)"ncFrameTimer::AddFrame - %lu frames in %.0f seconds = %f FPS (%.3fms per frame)",  m_ulLogNFrames, m_fLogInterval, m_fFps, fMSPF);

		m_ulLogNFrames = 0L;
		m_fLastLogUpdate = Now();
	}

	// Start counting for the next frame interval
	Start();
}
