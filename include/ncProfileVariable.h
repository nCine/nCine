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

	void AddValue(float fValue);
	inline unsigned int NumValues() const { return m_uNumValues; }
	inline unsigned int NextIndex() const { return m_uNextIndex; }
	inline float Value(unsigned int uIndex) const { return m_fValues[uIndex]; }
	inline float Min() const { return m_fMin; }
	inline float Max() const { return m_fMax; }
	inline float Mean() const { return m_fMean; }
	inline float NormValue(unsigned int uIndex) const
	{
		if (uIndex < m_uNumValues)
			return (m_fValues[uIndex]-m_fMin)/(m_fMax-m_fMin);
		else
			return 0.0f;
	}
	inline float NormMean() const { return (m_fMean-m_fMin)/(m_fMax-m_fMin); }
	inline const float* Values() const { return m_fValues; }
	void SetDelay(unsigned int uRejectDelay) { m_uRejectDelay = uRejectDelay; }
};

#endif
