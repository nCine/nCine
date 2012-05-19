#include <cstdio>
#include "ncFrameTimer.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! Constructs a timer which calculates average FPS every uAvgInterval
	milliseconds and prints in the log every uLogInterval seconds
*/
ncFrameTimer::ncFrameTimer(unsigned int uLogInterval, unsigned int uAvgInterval)
	: m_uLogInterval(uLogInterval), m_uAvgInterval(uAvgInterval),
	  m_ulFrameInterval(0L), m_ulAvgNFrames(0L), m_ulLogNFrames(0L),
	  m_ulLastAvgUpdate(0L), m_ulLastLogUpdate(0L), m_fFps(0.0f)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Adds a frame to the counter and calculates the interval since the previous one
void ncFrameTimer::AddFrame()
{
	m_ulFrameInterval = ncTimer::Interval();

	m_ulAvgNFrames++;
	m_ulLogNFrames++;

	// Update the FPS average calculation every m_uAvgInterval milliseconds
	if (m_uAvgInterval != 0 && (Now() - m_ulLastAvgUpdate > m_uAvgInterval))
	{
		m_fFps = float(m_ulAvgNFrames) / (float(Now() - m_ulLastAvgUpdate)/1000.0f);

		m_ulAvgNFrames = 0L;
		m_ulLastAvgUpdate = Now();
	}


	// Log number of frames and FPS every m_uLogInterval seconds
	if (m_uLogInterval != 0 && m_ulAvgNFrames != 0 && (Now() - m_ulLastLogUpdate > m_uLogInterval * 1000))
	{
		m_fFps = float(m_ulLogNFrames) / float(m_uLogInterval);
		// Milliseconds per frame
		float fMSPF = (m_uLogInterval * 1000.0f) / float(m_ulLogNFrames);
		ncServiceLocator::Logger().Write(ncILogger::LOG_VERBOSE, (const char *)"ncFrameTimer::AddFrame - %lu frames in %u seconds = %f FPS (%fms per frame)",  m_ulLogNFrames, m_uLogInterval, m_fFps, fMSPF);

		m_ulLogNFrames = 0L;
		m_ulLastLogUpdate = Now();
	}

	// Start counting for the next frame interval
	Start();
}
