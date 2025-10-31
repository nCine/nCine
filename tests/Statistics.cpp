#include "Statistics.h"
#include <cmath>
#include <float.h>
#include <nctl/Array.h>
#include <nctl/algorithms.h>

Statistics::Statistics(unsigned int capacity)
	: capacity_(capacity), size_(0), writeIndex_(0),
		values_(nctl::makeUnique<float[]>(capacity)),
		sortedValues_(nctl::makeUnique<float[]>(capacity))
{
	clearValues();
}

bool Statistics::addValue(float value)
{
	if (size_ < capacity_)
	{
		addValueWrap(value);
		return true;
	}
	return false;
}

void Statistics::addValueWrap(float value)
{
	values_[writeIndex_] = value;
	writeIndex_ = (writeIndex_ + 1) % capacity_;
	dirty = true;

	if (size_ < capacity_)
		size_++;
}

float Statistics::value(unsigned int index) const
{
	if (index >= capacity_)
		return 0.0f;
	return values_[index];
}

const float *Statistics::values(bool sorted) const
{
	if (sorted)
		return sortedValues_.get();
	return values_.get();
}

void Statistics::clearValues()
{
	for (unsigned int i = 0; i < capacity_; i++)
	{
		values_[i] = 0.0f;
		sortedValues_[i] = 0.0f;
	}
	size_ = 0;
	writeIndex_ = 0;
	dirty = true;
}

void Statistics::resetStats()
{
	sum_ = 0.0f;
	minimum_ = 0.0f;
	maximum_ = 0.0f;
	mean_ = 0.0f;
	mode_ = 0.0f;
	sigma_ = 0.0f;
	relativeSigma_ = 0.0f;
	dirty = true;
}

void Statistics::calculateStats()
{
	if (dirty == false)
		return;

	resetStats();
	minimum_ = FLT_MAX;
	maximum_ = -FLT_MAX;

	double meanD = 0.0;
	double M2 = 0.0;
	// Welford's algorithm
	for (unsigned int i = 0; i < size_; ++i)
	{
		sum_ += values_[i];
		if (values_[i] < minimum_)
			minimum_ = values_[i];
		if (values_[i] > maximum_)
			maximum_ = values_[i];

		const double x = values_[i];
		const double delta = x - meanD;
		meanD += delta / (i + 1);
		M2 += delta * (x - meanD);
	}

	mean_ = static_cast<float>(meanD);
	sigma_ = (size_ > 1) ? static_cast<float>(sqrt(M2 / (size_ - 1))) : 0.0f;
	relativeSigma_ = (mean_ > 0.0f) ? 100.0f * sigma_ / mean_ : 0.0f;

	nctl::copy(values_.get(), values_.get() + size_, sortedValues_.get());
	nctl::sort(sortedValues_.get(), sortedValues_.get() + size_, [](float a, float b) { return a < b; });

	mode_ = maximum_;
	if (maximum_ == minimum_)
		return;

	// Adaptive calculation bin width for the mode (Freedman–Diaconis rule with fallback)
	const float Q1 = sortedValues_[size_ / 4]; // first quartile
	const float Q3 = sortedValues_[3 * size_ / 4]; // third quartile
	const float iqr = Q3 - Q1; // inter-quartile range
	float modeBinWidth = (iqr > 0.0f) ? (2.0f * iqr / cbrtf(size_)) : ((maximum_ - minimum_) / sqrtf(size_));
	modeBinWidth = nctl::max(modeBinWidth, 1e-6f * (maximum_ - minimum_));

	// Mode calculation (binned approximation)
	const unsigned int numBins = static_cast<unsigned int>(ceilf((maximum_ - minimum_) / modeBinWidth));
	nctl::Array<unsigned int> counts(numBins);
	for (unsigned int i = 0; i < numBins; i++)
		counts.pushBack(0);

	for (unsigned int i = 0; i < size_; i++)
	{
		const unsigned int binIndex = static_cast<unsigned int>((sortedValues_[i] - minimum_) / modeBinWidth);
		if (binIndex < numBins)
			counts[binIndex]++;
		else
			counts[numBins - 1]++;
	}

	unsigned int maxBin = 0;
	for (unsigned int i = 0; i < numBins; i++)
	{
		if (counts[i] > counts[maxBin])
			maxBin = i;
	}
	mode_ = minimum_ + (maxBin + 0.5f) * modeBinWidth;
	dirty = false;
}

float Statistics::median() const
{
	if (size_ == 0)
		return 0.0f;
	if (size_ == 1)
		return sortedValues_[0];

	return (size_ % 2 == 0)
				? 0.5f * (sortedValues_[size_ / 2 - 1] + sortedValues_[size_ / 2])
				: sortedValues_[size_ / 2];
}

float Statistics::percentile(float p) const
{
	if (size_ == 0)
		return 0.0f;
	if (p <= 0.0f)
		return sortedValues_[0];
	if (p >= 1.0f)
		return sortedValues_[size_ - 1];

	const float fidx = p * (size_ - 1);
	const unsigned int idx = static_cast<unsigned int>(fidx);
	const float frac = fidx - idx;

	// Linear interpolation between adjacent points
	if (idx + 1 < size_)
		return sortedValues_[idx] * (1.0f - frac) + sortedValues_[idx + 1] * frac;
	else
		return sortedValues_[idx];
}
