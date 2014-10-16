#include <cstdio>
#include "ncVector2f.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Vector normalization
ncVector2f& ncVector2f::normalize()
{
	float len = length();

	x /= len;
	y /= len;

	return *this;
}

/// Dot product
float ncVector2f::dot(const ncVector2f& vec) const
{
	float result = 0.0f;

	result += x * vec.x;
	result += y * vec.y;

	return result;
}

bool ncVector2f::operator==(const ncVector2f& vec) const
{
	return (x == vec.x && y == vec.y);
}

ncVector2f ncVector2f::operator+(const ncVector2f& vec) const
{
	ncVector2f result;

	result.x = x + vec.x;
	result.y = y + vec.y;

	return result;
}

ncVector2f& ncVector2f::operator+=(const ncVector2f& vec)
{
	x += vec.x;
	y += vec.y;

	return *this;
}

ncVector2f ncVector2f::operator-(const ncVector2f& vec) const
{
	ncVector2f result;

	result.x = x - vec.x;
	result.y = y - vec.y;

	return result;
}

/// Unary negation
ncVector2f ncVector2f::operator-() const
{
	ncVector2f result;

	result.x = -x;
	result.y = -y;

	return result;
}

ncVector2f& ncVector2f::operator-=(const ncVector2f& vec)
{
	x -= vec.x;
	y -= vec.y;

	return *this;
}

/// Multiplication by a constant scalar
ncVector2f ncVector2f::operator*(float scalar) const
{
	ncVector2f result;

	result.x = scalar * x;
	result.y = scalar * y;

	return result;
}

ncVector2f ncVector2f::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;

	return *this;
}
