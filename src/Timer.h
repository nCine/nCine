#ifndef CLASS_TIMER
#define CLASS_TIMER

/// Basic timer and synchronization class
class Timer
{
protected:
	/// Is the timer running?
	bool m_bRunning;
	/// Start time mark
	unsigned long int m_uStartTime;
	/// Stop time mark
	unsigned long int m_uStopTime;
	/// Total elapsed time
	unsigned long int m_uTotal;
public:
	/// Empty constructor
	Timer() : m_bRunning(false), m_uStartTime(0), m_uStopTime(0), m_uTotal(0) {};
	// Reset the timer
	void Reset();
	// Start the timer
	void Start();
	// Stop the timer without resetting it
	void Stop();
	// Resume the timer
	void Continue();
	// Return (stop - start) or (now-start) time interval
	unsigned long int GetInterval();
	// Return total elapsed time
	unsigned long int GetTotal();
	// Return current time
	unsigned long int GetNow();
};

#endif
