#include <cstdio>
#include "Vector2f.h"

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Vector normalization
Vector2f& Vector2f::normalize()
{
	float len = length();

	x /= len;
	y /= len;

	return *this;
}

/// Dot product
float Vector2f::dot(const Vector2f& vec) const
{
	float result = 0.0f;

	result += x * vec.x;
	result += y * vec.y;

	return result;
}

bool Vector2f::operator==(const Vector2f& vec) const
{
	return (x == vec.x && y == vec.y);
}

Vector2f Vector2f::operator+(const Vector2f& vec) const
{
	Vector2f result;

	result.x = x + vec.x;
	result.y = y + vec.y;

	return result;
}

Vector2f& Vector2f::operator+=(const Vector2f& vec)
{
	x += vec.x;
	y += vec.y;

	return *this;
}

Vector2f Vector2f::operator-(const Vector2f& vec) const
{
	Vector2f result;

	result.x = x - vec.x;
	result.y = y - vec.y;

	return result;
}

/// Unary negation
Vector2f Vector2f::operator-() const
{
	Vector2f result;

	result.x = -x;
	result.y = -y;

	return result;
}

Vector2f& Vector2f::operator-=(const Vector2f& vec)
{
	x -= vec.x;
	y -= vec.y;

	return *this;
}

/// Multiplication by a constant scalar
Vector2f Vector2f::operator*(float scalar) const
{
	Vector2f result;

	result.x = scalar * x;
	result.y = scalar * y;

	return result;
}

Vector2f Vector2f::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;

	return *this;
}

}
