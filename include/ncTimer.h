#ifndef CLASS_NCTIMER
#define CLASS_NCTIMER

#ifdef __ANDROID__
	#include <time.h>
	#include <unistd.h>
#endif

/// Basic timer and synchronization class
class ncTimer
{
private:
#if defined(__ANDROID__)
	/// Base time mark structure
	/*! It is needed in order not to overflow when returning running time since epoch */
	#ifdef CLOCK_MONOTONIC
	static struct timespec m_initTs;
	#else
	static struct timeval m_initTv;
	#endif
#endif

protected:
	/// Start time mark
	double m_dStartTime;
public:
	/// Empty constructor
	ncTimer();
	/// Starts the timer
	inline void Start() { m_dStartTime = Now(); }
	/// Returns now-start time interval
	inline unsigned long int Interval() const { return static_cast<unsigned long int>(PreciseInterval()); }
	/// Returns now-start time interval (high precision)
	inline float PreciseInterval() const { return Now() - m_dStartTime; }
	/// Returns elapsed time in milliseconds since base time
	static inline unsigned long int Now() { return static_cast<unsigned long int>(PreciseNow()); }
	// Returns elapsed time in milliseconds since base time (high precision)
	static double PreciseNow();
	// Put the current thread to sleep for the specified number of milliseconds
	static void Sleep(unsigned int uMs);
};

#endif
