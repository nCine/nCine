#ifndef CLASS_NCINE_PROFILEVARIABLE
#define CLASS_NCINE_PROFILEVARIABLE

#include "Timer.h"

namespace ncine {

/// A class to store profiling values to be plotted or saved
class ProfileVariable
{
  public:
	ProfileVariable(unsigned int numValues, float rejectDelay);
	~ProfileVariable();

	/// Adds a new value for the variable
	bool addValue(float value);
	/// Returns the number of values that can be stored
	inline unsigned int numValues() const { return numValues_; }
	/// Returns the next index in the circular array of values
	inline unsigned int nextIndex() const { return nextIndex_; }
	/// Returns a recorded value with the specified index
	inline float value(unsigned int index) const { return values_[index]; }
	/// Returns the minimum value ever recorded
	inline float min() const { return min_; }
	/// Returns the maximum value ever recorded
	inline float max() const { return max_; }
	/// Returns the mathematical mean of the values
	inline float mean() const { return mean_; }
	/// Returns the value at the specified index normalized between `min` and `max`
	float normBetweenValue(unsigned int index, float min, float max) const;
	/// Returns the mathematical mean normalized between `min` and `max`
	float normBetweenMean(float min, float max) const;

	/// Returns a normalized version of the value with the specified index
	float normValue(unsigned int index) const { return normBetweenValue(index, min_, max_); }
	/// Returns a normalized mathematical mean
	float normMean() const { return normBetweenMean(min_, max_); }
	/// Returns the pointer to values
	inline const float *values() const { return values_; }
	/// Returns the delay in seconds for value rejection
	inline float delay() const { return rejectDelay_; }
	/// Set the delay in seconds for value rejection
	/*! A value added before the delay will be discarded. */
	inline void setDelay(float rejectDelay) { rejectDelay_ = rejectDelay; }

  private:
	unsigned int numValues_;
	float rejectDelay_;
	unsigned int nextIndex_;

	float min_;
	float max_;
	float mean_;
	bool isFirstValue_;

	float *values_;
	Timer timer_;

	/// Private copy constructor
	ProfileVariable(const ProfileVariable &);
	/// Private assignment operator
	ProfileVariable &operator=(const ProfileVariable &);
};

}

#endif
