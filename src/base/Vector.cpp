#include <cmath>
#include <cstdio>
#include "Vector.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Vector normalization
Vector& Vector::Normalize()
{
	float len = Length();

	m_x /= len;
	m_y /= len;

	return *this;
}


bool Vector::operator==(const Vector& vec) const
{
	return (m_x == vec.m_x &&
			m_y == vec.m_y);
}


Vector Vector::operator+(const Vector& vec) const
{
	Vector fResult;

	fResult.m_x = m_x + vec.m_x;
	fResult.m_y = m_y + vec.m_y;

	return fResult;
}


Vector& Vector::operator+=(const Vector& vec)
{
	m_x += vec.m_x;
	m_y += vec.m_y;

	return *this;
}


Vector Vector::operator-(const Vector& vec) const
{
	Vector fResult;

	fResult.m_x = m_x - vec.m_x;
	fResult.m_y = m_y - vec.m_y;

	return fResult;
}

/// Unary negation
Vector Vector::operator-() const
{
	Vector fResult;

	fResult.m_x = -m_x;
	fResult.m_y = -m_y;

	return fResult;
}


Vector& Vector::operator-=(const Vector& vec)
{
	m_x -= vec.m_x;
	m_y -= vec.m_y;

	return *this;
}

/// Multiplication by a constant scalar
Vector Vector::operator*(float fC) const
{
	Vector fResult;

	fResult.m_x = fC * m_x;
	fResult.m_y = fC * m_y;

	return fResult;
}
