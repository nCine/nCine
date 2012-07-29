#ifndef CLASS_NCVECTOR2F
#define CLASS_NCVECTOR2F

#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include <cstring>

/// A two dimensional vector
class ncVector2f
{
public:
	/// X coordinate as a public property
	float x;
	/// Y coordinate as a public property
	float y;
	/// Default constructor, all zeros
	ncVector2f() : x(0.0f), y(0.0f) { }
	/// Construct from components
	ncVector2f(float const fX, float const fY) : x(fX), y(fY) { }

	/// Sets vector elements
	inline void Set(float const fX, float const fY) {
		x = fX;
		y = fY;
	}

	/// Calculates vector length
	inline float Length() const {
		return sqrt(x*x + y*y);
	}
	// Vector normalization
	ncVector2f& Normalize();
	// Dot product
	float Dot(const ncVector2f& vec) const;

	// Operators
	bool operator==(const ncVector2f& vec) const;
	ncVector2f operator+(const ncVector2f& vec) const;
	ncVector2f& operator+=(const ncVector2f& vec);
	ncVector2f operator-(const ncVector2f& vec) const;
	// Unary negation
	ncVector2f operator-() const;
	ncVector2f& operator-=(const ncVector2f& vec);
	// Multiplication by a constant scalar
	ncVector2f operator*(float fC) const;
	ncVector2f operator*=(float fC);
};

#endif
