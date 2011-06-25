#include <cstdio>
#include "FrameTimer.h"
#include "ServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Construct a timer which updates its fps counter every 
/// m_uUpdateInterval milliseconds and prints on console
/// every m_uPrintInterval seconds
///
/// @param uPrintInterval console printing interval in seconds
/// @param uUpdateInterval fps average calculation interval in milliseconds
FrameTimer::FrameTimer(unsigned int uPrintInterval, unsigned int uUpdateInterval)
	: m_ulNFrames(0), m_ulPrintNFrames(0), m_fFps(0)
{
	Reset();
	m_uPrintInterval = uPrintInterval;
	m_uUpdateInterval = uUpdateInterval;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Add a frame to the counter and calculate the interval with the previous one
void FrameTimer::AddFrame()
{
	Stop();
	m_uFrameInterval = Timer::GetInterval();

	m_ulNFrames++;
	m_ulPrintNFrames++;

	// Self-implementation of partial timing
	if (m_uUpdateInterval != 0 && (GetNow() - m_uLastUpdate > m_uUpdateInterval)) {
		m_fFps = float(m_ulNFrames)/(float(GetNow() - m_uLastUpdate) / 1000.0f);

		m_ulNFrames = 0;
		m_uLastUpdate = GetNow();
	}


	// Using partial timing of the Timer class (using GetTotal())
	if (m_uPrintInterval != 0 && m_ulNFrames != 0 && (GetTotal() > m_uPrintInterval * 1000))	{
		float m_fFps = float(m_ulPrintNFrames)/float(m_uPrintInterval);
		ServiceLocator::GetLogger().Write(5, (char *)"%lu frames in %u seconds = %f FPS",  m_ulPrintNFrames, m_uPrintInterval, m_fFps);

		m_ulPrintNFrames = 0;
		Timer::Reset();
	}

	Continue();
}

/// Reset timer and counters
void FrameTimer::Reset()
{
	Timer::Reset();

	m_ulNFrames = 0;
	m_ulPrintNFrames = 0;
	m_fFps = 0;

	m_uLastUpdate = 0;
}
