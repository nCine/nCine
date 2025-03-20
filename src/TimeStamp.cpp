#include "common_macros.h"

#include "TimeStamp.h"
#include "Clock.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TimeStamp::TimeStamp()
    : counter_(clock().now())
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void TimeStamp::toNow()
{
	counter_ = clock().now();
}

bool TimeStamp::operator>(const TimeStamp &other) const
{
	return counter_ > other.counter_;
}

bool TimeStamp::operator<(const TimeStamp &other) const
{
	return counter_ < other.counter_;
}

TimeStamp &TimeStamp::operator+=(const TimeStamp &other)
{
	counter_ += other.counter_;
	return *this;
}

TimeStamp &TimeStamp::operator-=(const TimeStamp &other)
{
	ASSERT(counter_ > other.counter_);
	counter_ -= other.counter_;
	return *this;
}

TimeStamp TimeStamp::operator+(const TimeStamp &other) const
{
	return TimeStamp(counter_ + other.counter_);
}

TimeStamp TimeStamp::operator-(const TimeStamp &other) const
{
	ASSERT(counter_ > other.counter_);
	return TimeStamp(counter_ - other.counter_);
}

TimeStamp TimeStamp::timeSince() const
{
	return TimeStamp(clock().now() - counter_);
}

float TimeStamp::secondsSince() const
{
	return TimeStamp(clock().now() - counter_).seconds();
}

double TimeStamp::secondsDoubleSince() const
{
	return TimeStamp(clock().now() - counter_).secondsDouble();
}

float TimeStamp::millisecondsSince() const
{
	return TimeStamp(clock().now() - counter_).milliseconds();
}

double TimeStamp::millisecondsDoubleSince() const
{
	return TimeStamp(clock().now() - counter_).millisecondsDouble();
}

float TimeStamp::microsecondsSince() const
{
	return TimeStamp(clock().now() - counter_).microseconds();
}

double TimeStamp::microsecondsDoubleSince() const
{
	return TimeStamp(clock().now() - counter_).microsecondsDouble();
}

float TimeStamp::nanosecondsSince() const
{
	return TimeStamp(clock().now() - counter_).nanoseconds();
}

double TimeStamp::nanosecondsDoubleSince() const
{
	return TimeStamp(clock().now() - counter_).nanosecondsDouble();
}

float TimeStamp::seconds() const
{
	return static_cast<float>(counter_) / clock().frequency();
}

double TimeStamp::secondsDouble() const
{
	return static_cast<double>(counter_) / clock().frequency();
}

float TimeStamp::milliseconds() const
{
	return (static_cast<float>(counter_) / clock().frequency()) * 1000.0f;
}

double TimeStamp::millisecondsDouble() const
{
	return (static_cast<double>(counter_) / clock().frequency()) * 1000.0;
}

float TimeStamp::microseconds() const
{
	return (static_cast<float>(counter_) / clock().frequency()) * 1000000.0f;
}

double TimeStamp::microsecondsDouble() const
{
	return (static_cast<double>(counter_) / clock().frequency()) * 1000000.0;
}

float TimeStamp::nanoseconds() const
{
	return (static_cast<float>(counter_) / clock().frequency()) * 1000000000.0f;
}

double TimeStamp::nanosecondsDouble() const
{
	return (static_cast<double>(counter_) / clock().frequency()) * 1000000000.0;
}

}
