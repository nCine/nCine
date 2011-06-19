#include <cstdio>
#include "FrameTimer.h"
#include "ServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Construct a timer which updates its fps counter every 
/// m_uiUpdateInterval milliseconds and prints on console 
/// every m_uiPrintInterval seconds
///
/// @param uiPrintInterval console printing interval in seconds
/// @param uiUpdateInterval fps average calculation interval in milliseconds
FrameTimer::FrameTimer(unsigned int uiPrintInterval, unsigned int uiUpdateInterval)
	: m_ulNFrames(0), m_ulPrintNFrames(0), m_fFps(0)
{
	Reset();
	m_uiPrintInterval = uiPrintInterval;
	m_uiUpdateInterval = uiUpdateInterval;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Add a frame to the counter and calculate the interval with the previous one
void FrameTimer::AddFrame()
{
	Stop();
	m_uiFrameInterval = Timer::GetInterval();

	m_ulNFrames++;
	m_ulPrintNFrames++;

	// Self-implementation of partial timing
	if (m_uiUpdateInterval != 0 && (GetNow() - m_uiLastUpdate > m_uiUpdateInterval)) {
		m_fFps = float(m_ulNFrames)/(float(GetNow() - m_uiLastUpdate) / 1000.0f);

		m_ulNFrames = 0;
		m_uiLastUpdate = GetNow();
	}


	// Using partial timing of the Timer class (using GetTotal())
	if (m_uiPrintInterval != 0 && m_ulNFrames != 0 && (GetTotal() > m_uiPrintInterval * 1000))	{
		float m_fFps = float(m_ulPrintNFrames)/float(m_uiPrintInterval);
		ServiceLocator::GetLogger().Write(5, (char *)"%lu frames in %u seconds = %f FPS",  m_ulPrintNFrames, m_uiPrintInterval, m_fFps);

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

	m_uiLastUpdate = 0;
}
