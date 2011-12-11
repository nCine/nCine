#ifndef CLASS_NCPROFILEVARIABLE
#define CLASS_NCPROFILEVARIABLE

#include "ncTimer.h"

/// A class to store profiling values to be plotted or saved
class ncProfileVariable
{
private:
	unsigned int m_uNumValues;
	unsigned int m_uRejectDelay;
	unsigned int m_uNextIndex;

	float m_fMin;
	float m_fMax;
	float m_fMean;
	bool m_bFirstValue;

	float *m_fValues;
	ncTimer m_timer;
public:
	ncProfileVariable(unsigned int uNumValues, unsigned int uRejectDelay);
	~ncProfileVariable();

	// Adds a new value for the variable
	void AddValue(float fValue);
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
	/// Returns the mathematic mean of the values
	inline float Mean() const { return m_fMean; }
	/// Returns a normalized version of the value with the specified index
	inline float NormValue(unsigned int uIndex) const
	{
		if (uIndex < m_uNumValues)
			return (m_fValues[uIndex]-m_fMin)/(m_fMax-m_fMin);
		else
			return 0.0f;
	}
	/// Returns a normalized mathematic mean
	inline float NormMean() const { return (m_fMean-m_fMin)/(m_fMax-m_fMin); }
	/// Returns the pointer to values
	inline const float* Values() const { return m_fValues; }
	/// Returns the delay in milliseconds for value rejection
	inline unsigned int Delay() const { return m_uRejectDelay; }
	/// Set the delay in millieconds for value rejection
	/** If you try to add a value before the delay, it gets discarded. */
	inline void SetDelay(unsigned int uRejectDelay) { m_uRejectDelay = uRejectDelay; }
};

#endif
