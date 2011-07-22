#ifndef CLASS_NCVECTOR2F
#define CLASS_NCVECTOR2F

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

	/// Set vector elements
	inline void Set(float const fX, float const fY) {
		x = fX;
		y = fY;
	}

	/// Calculate vector length
	inline float Length() const {
		return sqrt(x*x + y*y);
	}
	// Vector normalization
	inline ncVector2f& Normalize();

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

	inline void Print() const {
		printf("(%f, %f)", x, y);
	}
};

#endif
