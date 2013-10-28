#ifndef CLASS_NCTIMER
#define CLASS_NCTIMER

#if !defined(_WIN32) && !defined(__APPLE__)
	#include <time.h> // for clock_gettime()
	#include <sys/time.h> // for gettimeofday()
	#include <unistd.h>
#endif

/// Basic timer and synchronization class
class ncTimer
{
private:
#if !defined(_WIN32) && !defined(__APPLE__)
	/// Base time mark structures
	/*! They are needed to prevent overflowing when returning running time since epoch */
	static struct timespec s_initTs;
	static struct timeval s_initTv;

	static bool s_bIsMonotonic;
#endif

	/// Have the static fields been initialized?
	static bool s_bIsInitialized;
	/// Timer frequency in counts per second
	static unsigned long int s_ulFrequency;

	/// Initializes the static fields
	static void Init();

protected:
	/// Start time mark
	float m_fStartTime;

public:
	/// Empty constructor
	ncTimer() : m_fStartTime(0.0f) { }
	/// Starts the timer
	inline void Start() { m_fStartTime = Now(); }
	/// Returns now-start time interval in seconds
	inline float Interval() const { return Now() - m_fStartTime; }
	// Returns elapsed time in seconds since base time
	static float Now();
	// Puts the current thread to sleep for the specified number of seconds
	static void Sleep(float fS);
};

#endif
