#ifndef CLASS_NCFRAMETIMER
#define CLASS_NCFRAMETIMER

#include "ncTimer.h"

/// FPS timer and synchronization class
class ncFrameTimer: public ncTimer
{
private:
	/// Frame rendered
	unsigned long int m_ulNFrames;
	/// Frames rendered since last print
	unsigned long int m_ulPrintNFrames;
	/// Milliseconds elapsed since the previous frame
	unsigned long int m_uFrameInterval;
	/// Number of seconds between two prints
	unsigned long int m_uPrintInterval;
	/// Number of milliseconds since the last average FPS calculation
	unsigned long int m_uLastUpdate;
	/// Number of milliseconds between two average FPS calculation
	unsigned long int m_uUpdateInterval;
	/// Average frame per seconds during the interval
	float m_fFps;
public:
	// Constructor
	ncFrameTimer(unsigned int uPrintInterval, unsigned int uUpdateInterval = 0);
	// Add a frame to the counter and calculate elapsed time since the previous one
	void AddFrame();
	// Reset timer and counters
	void Reset();
	/// Return current frame number
	inline unsigned long int FrameNum() { return m_ulNFrames; }
	/// Return the interval between two subsequent calls to AddFrame()
	inline unsigned long int Interval() { return m_uFrameInterval; }
	/// Return the average FPS on the update interval
	inline float AverageFPS() { return m_fFps; }
};

#endif

