#ifndef CLASS_NCINE_VECTOR2F
#define CLASS_NCINE_VECTOR2F

#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include <cstring>

namespace ncine {

/// A two dimensional vector
class Vector2f
{
  public:
	/// X coordinate as a public property
	float x;
	/// Y coordinate as a public property
	float y;

	/// Default constructor, all zeros
	Vector2f() : x(0.0f), y(0.0f) { }
	/// Construct from components
	Vector2f(float const xx, float const yy) : x(xx), y(yy) { }

	// Sets vector elements
	void set(float const xx, float const yy);

	/// Calculates vector length
	inline float length() const { return sqrtf(x * x + y * y); }
	// Vector normalization
	Vector2f& normalize();
	// Dot product
	float dot(const Vector2f& vec) const;

	// Operators
	bool operator==(const Vector2f& vec) const;
	Vector2f operator+(const Vector2f& vec) const;
	Vector2f& operator+=(const Vector2f& vec);
	Vector2f operator-(const Vector2f& vec) const;
	// Unary negation
	Vector2f operator-() const;
	Vector2f& operator-=(const Vector2f& vec);
	// Multiplication by a constant scalar
	Vector2f operator*(float scalar) const;
	Vector2f operator*=(float scalar);
};

/// Sets vector elements
inline void Vector2f::set(float const xx, float const yy)
{
	x = xx;
	y = yy;
}

}

#endif
