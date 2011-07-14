#ifndef CLASS_NCTIMER
#define CLASS_NCTIMER

/// Basic timer and synchronization class
class ncTimer
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
	ncTimer() : m_bRunning(false), m_uStartTime(0), m_uStopTime(0), m_uTotal(0) {};
	// Reset the timer
	void Reset();
	// Start the timer
	void Start();
	// Stop the timer without resetting it
	void Stop();
	// Resume the timer
	void Continue();
	// Return (stop - start) or (now-start) time interval
	unsigned long int Interval();
	// Return total elapsed time
	unsigned long int Total();
	// Return current time
	unsigned long int Now();
};

#endif
