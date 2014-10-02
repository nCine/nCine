#ifndef CLASS_NCTIMER
#define CLASS_NCTIMER

/// Basic timer and synchronization class
class ncTimer
{
  public:
	// Empty constructor
	ncTimer();
	/// Starts the timer
	inline void Start() { m_ullStartTime = Counter(); }
	// Returns now-start time interval in seconds
	float Interval() const;
	// Returns elapsed time in seconds since base time
	static float Now();
	// Puts the current thread to sleep for the specified number of seconds
	static void Sleep(float fS);

  protected:
	/// Start time mark
	unsigned long long int m_ullStartTime;

  private:
#ifdef _WIN32
	static bool s_bHasPerfCounter;
#elif !defined(__APPLE__)
	static bool s_bHasMonotonicClock;
#endif

	/// Have the static fields been initialized?
	static bool s_bIsInitialized;
	/// Counter frequency in counts per second
	static unsigned long int s_ulFrequency;
	/// Counter value at initialization time
	static unsigned long long int s_ullBaseCount;

	// Initializes the static fields
	static void Init();

	// Returns current value of the counter
	static unsigned long long int Counter();
};

/// Returns now-start time interval in seconds
inline float ncTimer::Interval() const
{
	return float(Counter() - m_ullStartTime) / s_ulFrequency;
}

#endif
