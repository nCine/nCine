#include <cmath>
#include <cstdio>
#include "ncVector2f.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Vector normalization
ncVector2f& ncVector2f::Normalize()
{
	float len = Length();

	x /= len;
	y /= len;

	return *this;
}

/// Dot product
float ncVector2f::Dot(const ncVector2f& vec) const
{
	float fResult = 0.0f;

	fResult += x * vec.x;
	fResult += y * vec.y;

	return fResult;
}

bool ncVector2f::operator==(const ncVector2f& vec) const
{
	return (x == vec.x && y == vec.y);
}

ncVector2f ncVector2f::operator+(const ncVector2f& vec) const
{
	ncVector2f fResult;

	fResult.x = x + vec.x;
	fResult.y = y + vec.y;

	return fResult;
}

ncVector2f& ncVector2f::operator+=(const ncVector2f& vec)
{
	x += vec.x;
	y += vec.y;

	return *this;
}

ncVector2f ncVector2f::operator-(const ncVector2f& vec) const
{
	ncVector2f fResult;

	fResult.x = x - vec.x;
	fResult.y = y - vec.y;

	return fResult;
}

/// Unary negation
ncVector2f ncVector2f::operator-() const
{
	ncVector2f fResult;

	fResult.x = -x;
	fResult.y = -y;

	return fResult;
}

ncVector2f& ncVector2f::operator-=(const ncVector2f& vec)
{
	x -= vec.x;
	y -= vec.y;

	return *this;
}

/// Multiplication by a constant scalar
ncVector2f ncVector2f::operator*(float fC) const
{
	ncVector2f fResult;

	fResult.x = fC * x;
	fResult.y = fC * y;

	return fResult;
}

ncVector2f ncVector2f::operator*=(float fC)
{
	x *= fC;
	y *= fC;

	return *this;
}
