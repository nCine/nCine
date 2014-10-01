#include <cstdlib> // for exit()
#include "ncProfileVariable.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncProfileVariable::ncProfileVariable(unsigned int uNumValues, float fRejectDelay)
	: m_uNumValues(uNumValues), m_fRejectDelay(fRejectDelay), m_uNextIndex(0),
	  m_fMin(0.0f), m_fMax(0.0f), m_fMean(0.0f), m_bFirstValue(true)
{
	if (m_uNumValues > 0)
	{
		m_fValues = new float[m_uNumValues];
		for (unsigned int i = 0; i < m_uNumValues; i++)
		{
			m_fValues[i] = 0.0f;
		}
	}
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncProfileVariable::ncProfileVariable - Array size should be greater than zero");
		exit(EXIT_FAILURE);
	}
}

ncProfileVariable::~ncProfileVariable()
{
	if (m_fValues)
	{
		delete[] m_fValues;
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Adds a new value for the variable
/*! /return A bool indicating wether the value has been registered or not (rejected) */
bool ncProfileVariable::AddValue(float fValue)
{
	if (m_bFirstValue)
	{
		m_fMin = fValue;
		m_fMax = fValue;
		m_bFirstValue = false;

		if (m_fRejectDelay > 0.0f)
		{
			m_timer.Start();
		}
	}

	bool bRegisterValue = false;
	if (m_fRejectDelay > 0.0f)
	{
		// Timer expired
		if (m_timer.Interval() > m_fRejectDelay)
		{
			bRegisterValue = true;
			m_timer.Start();
		}
	}
	else
	{
		bRegisterValue = true;
	}

	// A new value has to be registered in the array
	if (bRegisterValue)
	{
		if (fValue > m_fMax)
		{
			m_fMax = fValue;
		}
		else if (fValue < m_fMin)
		{
			m_fMin = fValue;
		}

		m_fValues[m_uNextIndex] = fValue;
		m_fMean = 0.0f;
		for (unsigned int i = 0; i < m_uNumValues; i++)
		{
			m_fMean += m_fValues[(m_uNextIndex - i) % m_uNumValues];
		}
		m_fMean *= 1.0f / m_uNumValues;

		m_uNextIndex = (m_uNextIndex + 1) % m_uNumValues;
	}

	// HACK: Looking for max and min across all the values, to prevent random spikes
	m_fMax = 0.0f;
	m_fMin = 0.0f;

	for (unsigned int i = 0; i < m_uNumValues; i++)
	{
		if (m_fValues[i] > m_fMax)
		{
			m_fMax = m_fValues[i];
		}
		else if (m_fValues[i] < m_fMin)
		{
			m_fMin = m_fValues[i];
		}
	}

	return bRegisterValue;
}

/// Returns the value at the specified index normalized between the two numbers provided
float ncProfileVariable::NormBetweenValue(unsigned int uIndex, float fMin, float fMax) const
{
	float fValue = 0.0f;

	if (uIndex < m_uNumValues && (fMax - fMin > 0.0f))
	{
		fValue = (m_fValues[uIndex] - fMin) / (fMax - fMin);
	}

	return fValue;
}

/// Returns the mathematical mean normalized between the two numbers provided
float ncProfileVariable::NormBetweenMean(float fMin, float fMax) const
{
	float fValue = 0.0f;

	if (fMax - fMin > 0.0f)
	{
		fValue = (m_fMean - fMin) / (fMax - fMin);
	}

	return fValue;
}
