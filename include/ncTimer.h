#ifndef CLASS_NCTIMER
#define CLASS_NCTIMER

/// Basic timer and synchronization class
class ncTimer
{
  public:
	// Empty constructor
	ncTimer();
	/// Starts the timer
	inline void start() { startTime_ = counter(); }
	// Returns now-start time interval in seconds
	float interval() const;
	// Returns elapsed time in seconds since base time
	static float now();
	// Puts the current thread to sleep for the specified number of seconds
	static void sleep(float seconds);

  protected:
	/// Start time mark
	unsigned long long int startTime_;

  private:
#ifdef _WIN32
	static bool hasPerfCounter_;
#elif !defined(__APPLE__)
	static bool hasMonotonicClock_;
#endif

	/// Have the static fields been initialized?
	static bool isInitialized_;
	/// Counter frequency in counts per second
	static unsigned long int frequency_;
	/// Counter value at initialization time
	static unsigned long long int baseCount_;

	// Initializes the static fields
	static void init();

	// Returns current value of the counter
	static unsigned long long int counter();
};

/// Returns now-start time interval in seconds
inline float ncTimer::interval() const
{
	return float(counter() - startTime_) / frequency_;
}

#endif
