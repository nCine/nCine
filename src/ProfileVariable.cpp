#include "common_macros.h"
#include "ProfileVariable.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ProfileVariable::ProfileVariable(unsigned int numValues, float rejectDelay)
	: numValues_(numValues), rejectDelay_(rejectDelay), nextIndex_(0),
	  min_(0.0f), max_(0.0f), mean_(0.0f), isFirstValue_(true),
	  values_(numValues, nctl::ArrayMode::FIXED_CAPACITY)
{
	FATAL_ASSERT_MSG(numValues > 0, "Array size should be greater than zero");

	for (unsigned int i = 0; i < numValues_; i++)
		values_[i] = 0.0f;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/*! \return A boolean indicating wether the value has been registered or not (rejected) */
bool ProfileVariable::addValue(float value)
{
	if (isFirstValue_)
	{
		min_ = value;
		max_ = value;
		isFirstValue_ = false;

		if (rejectDelay_ > 0.0f)
			timer_.start();
	}

	bool registerValue = false;
	if (rejectDelay_ > 0.0f)
	{
		// Timer expired
		if (timer_.interval() > rejectDelay_)
		{
			registerValue = true;
			timer_.start();
		}
	}
	else
		registerValue = true;

	// A new value has to be registered in the array
	if (registerValue && numValues_ > 0)
	{
		if (value > max_)
			max_ = value;
		else if (value < min_)
			min_ = value;

		values_[nextIndex_] = value;
		mean_ = 0.0f;
		for (unsigned int i = 0; i < numValues_; i++)
			mean_ += values_[(nextIndex_ - i) % numValues_];
		mean_ *= 1.0f / numValues_;

		nextIndex_ = (nextIndex_ + 1) % numValues_;
	}

	// HACK: Looking for max and min across all the values, to prevent random spikes
	max_ = 0.0f;
	min_ = 0.0f;

	for (unsigned int i = 0; i < numValues_; i++)
	{
		if (values_[i] > max_)
			max_ = values_[i];
		else if (values_[i] < min_)
			min_ = values_[i];
	}

	return registerValue;
}

float ProfileVariable::normBetweenValue(unsigned int index, float min, float max) const
{
	float value = 0.0f;

	if (index < numValues_ && (max - min > 0.0f))
		value = (values_[index] - min) / (max - min);

	return value;
}

float ProfileVariable::normBetweenMean(float min, float max) const
{
	float value = 0.0f;

	if (max - min > 0.0f)
		value = (mean_ - min) / (max - min);

	return value;
}

}
