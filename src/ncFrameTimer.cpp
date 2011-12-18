#include <cstdio>
#include "ncFrameTimer.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructs a timer which updates its fps counter every
/// m_uUpdateInterval milliseconds and prints on console
/// every m_uPrintInterval seconds
///
/// @param uPrintInterval console printing interval in seconds
/// @param uUpdateInterval fps average calculation interval in milliseconds
ncFrameTimer::ncFrameTimer(unsigned int uPrintInterval, unsigned int uUpdateInterval)
	: m_ulNFrames(0), m_ulPrintNFrames(0), m_fFps(0)
{
	Reset();
	m_uPrintInterval = uPrintInterval;
	m_uUpdateInterval = uUpdateInterval;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Adds a frame to the counter and calculate the interval with the previous one
void ncFrameTimer::AddFrame()
{
	Stop();
	m_uFrameInterval = ncTimer::Interval();

	m_ulNFrames++;
	m_ulPrintNFrames++;

	// Self-implementation of partial timing
	if (m_uUpdateInterval != 0 && (Now() - m_uLastUpdate > m_uUpdateInterval)) {
		m_fFps = float(m_ulNFrames)/(float(Now() - m_uLastUpdate) / 1000.0f);

		m_ulNFrames = 0;
		m_uLastUpdate = Now();
	}


	// Using partial timing of the Timer class (using GetTotal())
	if (m_uPrintInterval != 0 && m_ulNFrames != 0 && (Total() > m_uPrintInterval * 1000))	{
		m_fFps = float(m_ulPrintNFrames)/float(m_uPrintInterval);
		float fTpf = (m_uPrintInterval*1000.0f)/float(m_ulPrintNFrames);
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_VERBOSE, (const char *)"ncFrameTimer::AddFrame - %lu frames in %u seconds = %f FPS (%fms per frame)",  m_ulPrintNFrames, m_uPrintInterval, m_fFps, fTpf);

		m_ulPrintNFrames = 0;
		ncTimer::Reset();
	}

	Continue();
}

/// Resets timer and counters
void ncFrameTimer::Reset()
{
	ncTimer::Reset();

	m_ulNFrames = 0;
	m_ulPrintNFrames = 0;
	m_fFps = 0;

	m_uLastUpdate = 0;
}
