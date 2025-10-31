#ifndef CLASS_STATISTICS
#define CLASS_STATISTICS

#include <nctl/UniquePtr.h>

class Statistics
{
  public:
	explicit Statistics(unsigned int capacity);

	inline unsigned int capacity() const { return capacity_; }
	inline bool isEmpty() const { return size_ == 0; }
	inline unsigned int size() const { return size_; }

	bool addValue(float value);
	void addValueWrap(float value);

	float value(unsigned int index) const;
	inline const float *values() const { return values(false); }
	const float *values(bool sorted) const;

	void clearValues();

	void resetStats();
	void calculateStats();

	inline float sum() const { return sum_; }
	inline float minimum() const { return minimum_; }
	inline float maximum() const { return maximum_; }
	inline float mean() const { return mean_; }
	inline float mode() const { return mode_; }
	inline float sigma() const { return sigma_; }
	inline float relativeSigma() const { return relativeSigma_; }
	inline float range() const { return maximum_ - minimum_; }

	float median() const;
	float percentile(float p) const;

  private:
	unsigned int capacity_ = 0;
	unsigned int size_ = 0;
	/// Next index to write a value with circular wrapping
	unsigned int writeIndex_ = 0;
	/// If values have changed, statistics need to be recalculated
	bool dirty = true;

	float sum_ = 0.0f;
	float minimum_ = 0.0f;
	float maximum_ = 0.0f;
	float mean_ = 0.0f;
	float mode_ = 0.0f;
	float sigma_ = 0.0f;
	float relativeSigma_ = 0.0f;

	nctl::UniquePtr<float[]> values_;
	nctl::UniquePtr<float[]> sortedValues_;
};

#endif
