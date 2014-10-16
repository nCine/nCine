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
	ncVector2f(float const xx, float const yy) : x(xx), y(yy) { }

	// Sets vector elements
	void set(float const xx, float const yy);

	/// Calculates vector length
	inline float length() const { return sqrt(x * x + y * y); }
	// Vector normalization
	ncVector2f& normalize();
	// Dot product
	float dot(const ncVector2f& vec) const;

	// Operators
	bool operator==(const ncVector2f& vec) const;
	ncVector2f operator+(const ncVector2f& vec) const;
	ncVector2f& operator+=(const ncVector2f& vec);
	ncVector2f operator-(const ncVector2f& vec) const;
	// Unary negation
	ncVector2f operator-() const;
	ncVector2f& operator-=(const ncVector2f& vec);
	// Multiplication by a constant scalar
	ncVector2f operator*(float scalar) const;
	ncVector2f operator*=(float scalar);
};

/// Sets vector elements
inline void ncVector2f::set(float const xx, float const yy)
{
	x = xx;
	y = yy;
}

#endif
