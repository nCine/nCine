#ifndef CLASS_NCPROFILEVARIABLE
#define CLASS_NCPROFILEVARIABLE

#include "ncTimer.h"

/// A class to store profiling values to be plotted or saved
class ncProfileVariable
{
 public:
	ncProfileVariable(unsigned int uNumValues, float fRejectDelay);
	~ncProfileVariable();

	// Adds a new value for the variable
	bool AddValue(float fValue);
	/// Returns the number of values that can be stored
	inline unsigned int NumValues() const { return m_uNumValues; }
	/// Returns the next index in the circular array of values
	inline unsigned int NextIndex() const { return m_uNextIndex; }
	/// Returns a recorded value with the specified index
	inline float Value(unsigned int uIndex) const { return m_fValues[uIndex]; }
	/// Returns the minimum value ever recorded
	inline float Min() const { return m_fMin; }
	/// Returns the maximum value ever recorded
	inline float Max() const { return m_fMax; }
	/// Returns the mathematical mean of the values
	inline float Mean() const { return m_fMean; }
	// Returns the value at the specified index normalized between the two numbers provided
	float NormBetweenValue(unsigned int uIndex, float fMin, float fMax) const;
	// Returns the mathematical mean normalized between the two numbers provided
	float NormBetweenMean(float fMin, float fMax) const;

	/// Returns a normalized version of the value with the specified index
	float NormValue(unsigned int uIndex) const { return NormBetweenValue(uIndex, m_fMin, m_fMax); }
	/// Returns a normalized mathematical mean
	float NormMean() const { return NormBetweenMean(m_fMin, m_fMax); }
	/// Returns the pointer to values
	inline const float* Values() const { return m_fValues; }
	/// Returns the delay in seconds for value rejection
	inline float Delay() const { return m_fRejectDelay; }
	/// Set the delay in seconds for value rejection
	/** If you try to add a value before the delay, it gets discarded. */
	inline void SetDelay(float fRejectDelay) { m_fRejectDelay = fRejectDelay; }

 private:
	/// Preventing construction by copy
	ncProfileVariable(const ncProfileVariable& rOther);

	unsigned int m_uNumValues;
	float m_fRejectDelay;
	unsigned int m_uNextIndex;

	float m_fMin;
	float m_fMax;
	float m_fMean;
	bool m_bFirstValue;

	float *m_fValues;
	ncTimer m_timer;
};

#endif
