#ifndef CLASS_NCINE_TIMESTAMP
#define CLASS_NCINE_TIMESTAMP

#include <cstdint>
#include "common_defines.h"

namespace ncine {

/// A class representing a point in time or a duration
class DLL_PUBLIC TimeStamp
{
  public:
	/// Constructs a new time stamp initialized to the current clock value
	TimeStamp();

	/// Returns a new time stamp initialized to the current clock value
	inline static TimeStamp now() { return TimeStamp(); }

	/// Sets the time stamp to the current clock value
	void toNow();

	bool operator>(const TimeStamp &other) const;
	bool operator<(const TimeStamp &other) const;
	TimeStamp &operator+=(const TimeStamp &other);
	TimeStamp &operator-=(const TimeStamp &other);
	TimeStamp operator+(const TimeStamp &other) const;
	TimeStamp operator-(const TimeStamp &other) const;

	/// Returns a new time stamp with the time elapsed since this one
	TimeStamp timeSince() const;

	/// Returns the time elapsed since the timestamp, as seconds in a `float` number
	float secondsSince() const;
	/// Returns the time elapsed since the timestamp, as seconds in a `double` number
	double secondsDoubleSince() const;

	/// Returns the time elapsed since the timestamp, as milliseconds in a `float` number
	float millisecondsSince() const;
	/// Returns the time elapsed since the timestamp, as milliseconds in a `double` number
	double millisecondsDoubleSince() const;

	/// Returns the time elapsed since the timestamp, as microseconds in a `float` number
	float microsecondsSince() const;
	/// Returns the time elapsed since the timestamp, as microseconds in a `double` number
	double microsecondsDoubleSince() const;

	/// Returns the time elapsed since the timestamp, as seconds in a `nanoseconds` number
	float nanosecondsSince() const;
	/// Returns the time elapsed since the timestamp, as seconds in a `nanoseconds` number
	double nanosecondsDoubleSince() const;

	/// Returns the timestamp counter value (the number of ticks elapsed until now)
	inline uint64_t ticks() const { return counter_; }

	/// Returns the timestamp counter value as seconds in a `float` number
	float seconds() const;
	/// Returns the timestamp counter value as seconds in a `double` number
	double secondsDouble() const;

	/// Returns the timestamp counter value as milliseconds in a `float` number
	float milliseconds() const;
	/// Returns the timestamp counter value as milliseconds in a `double` number
	double millisecondsDouble() const;

	/// Returns the timestamp counter value as microseconds in a `float` number
	float microseconds() const;
	/// Returns the timestamp counter value as microseconds in a `double` number
	double microsecondsDouble() const;

	/// Returns the timestamp counter value as nanoseconds in a `float` number
	float nanoseconds() const;
	/// Returns the timestamp counter value as nanoseconds in a `double` number
	double nanosecondsDouble() const;

  private:
	uint64_t counter_;

	explicit TimeStamp(uint64_t counter)
	    : counter_(counter) {}

#ifdef WITH_LUA
	friend class LuaTimeStamp;
#endif
};

}

#endif
