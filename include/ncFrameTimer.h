#ifndef CLASS_NCFRAMETIMER
#define CLASS_NCFRAMETIMER

#include "ncTimer.h"

/// Frame interval and average FPS calculator class
class ncFrameTimer: public ncTimer
{
private:
	/// Number of seconds between two log events (user defined)
	unsigned int m_uLogInterval;
	/// Number of milliseconds between two average FPS calculations (user defined)
	unsigned int m_uAvgInterval;

	/// Milliseconds elapsed since the previous frame
	unsigned long int m_ulFrameInterval;

	/// Frame counter for average FPS calculation
	unsigned long int m_ulAvgNFrames;
	/// Frame counter for logging
	unsigned long int m_ulLogNFrames;

	/// Number of milliseconds since the last average FPS calculation
	unsigned long int m_ulLastAvgUpdate;
	/// Number of milliseconds since the last log event
	unsigned long int m_ulLastLogUpdate;

	/// Average FPS calulated during the specified interval
	float m_fFps;
public:
	// Constructor
	ncFrameTimer(unsigned int uPrintInterval, unsigned int uUpdateInterval);
	// Adds a frame to the counter and calculates the interval since the previous one
	void AddFrame();
	/// Returns the interval between two subsequent calls to AddFrame()
	inline float Interval() const { return m_ulFrameInterval; }
	/// Returns the average FPS on the update interval
	inline float AverageFPS() const { return m_fFps; }
};

#endif

