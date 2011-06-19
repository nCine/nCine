#ifndef CLASS_VECTOR
#define CLASS_VECTOR

#include <cstring>

/// A two dimensional vector
class Vector
{
protected:
	float m_x, m_y;
public:
	/// Default constructor, all zeros
	Vector() : m_x(0.0f), m_y(0.0f) { }
	/// Construct from components
	Vector(float const x, float const y) : m_x(x), m_y(y) { }

	// Property methods
	inline float X() const {return m_x;}
	inline void SetX(float x) {m_x = x;}
	inline float Y() const {return m_y;}
	inline void SetY(float y) {m_y = y;}

	/// Set null every element of the vector
	inline void SetNull() {
		m_x = 0.0f;
		m_y = 0.0f;
	};
	/// Set vector elements
	inline void Set(float const x, float const y) {
		m_x = x;
		m_y = y;
	}

	/// Calculate vector length
	inline float Length() const {
		return sqrt(m_x*m_x + m_y*m_y);
	}
	// Vector normalization
	inline Vector& Normalize();

	// Operators
	bool operator==(const Vector& vec) const;
	Vector operator+(const Vector& vec) const;
	Vector& operator+=(const Vector& vec);
	Vector operator-(const Vector& vec) const;
	// Unary negation
	Vector operator-() const;
	Vector& operator-=(const Vector& vec);
	// Multiplication by a constant scalar
	Vector operator*(float fC) const;

	friend void print(Vector& vec) {
		printf("%f %f", vec.m_x, vec.m_y);
	}
};

#endif
