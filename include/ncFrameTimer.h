#ifndef CLASS_NCFRAMETIMER
#define CLASS_NCFRAMETIMER

#include "ncTimer.h"

/// Frame interval and average FPS calculator class
class ncFrameTimer: public ncTimer
{
 public:
	// Constructor
	ncFrameTimer(float fLogInterval, float fAvgInterval);
	// Adds a frame to the counter and calculates the interval since the previous one
	void AddFrame();
	/// Returns the interval between two subsequent calls to AddFrame()
	inline float Interval() const { return m_fFrameInterval; }
	/// Returns the average FPS on the update interval
	inline float AverageFPS() const { return m_fFps; }

 private:
	/// Number of seconds between two log events (user defined)
	float m_fLogInterval;
	/// Number of seconds between two average FPS calculations (user defined)
	float m_fAvgInterval;

	/// Seconds elapsed since previous frame
	float m_fFrameInterval;

	/// Frame counter for average FPS calculation
	unsigned long int m_ulAvgNFrames;
	/// Frame counter for logging
	unsigned long int m_ulLogNFrames;

	/// Number of seconds since last average FPS calculation
	float m_fLastAvgUpdate;
	/// Number of seconds since last log event
	float m_fLastLogUpdate;

	/// Average FPS calulated during the specified interval
	float m_fFps;
};

#endif

