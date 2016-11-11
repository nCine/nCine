#ifndef CLASS_NCINE_VECTOR3
#define CLASS_NCINE_VECTOR3

#define _USE_MATH_DEFINES // for M_PI on MSVC
#include <cmath>
#include "Vector2.h"

namespace ncine {

/// A three component vector based on templates
template <class T>
class Vector3
{
  public:
	T x, y, z;

	Vector3() { }
	explicit Vector3(T s) : x(s), y(s), z(s) { }
	Vector3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { }

	void set(T xx, T yy, T zz);

	T *data();
	const T *data() const;

	T &operator[](unsigned int index);
	const T &operator[](unsigned int index) const;

	bool operator==(const Vector3 &v) const;
	Vector3 operator-() const;

	Vector3 &operator+=(const Vector3 &v);
	Vector3 &operator-=(const Vector3 &v);
	Vector3 &operator*=(const Vector3 &v);
	Vector3 &operator/=(const Vector3 &v);

	Vector3 &operator+=(T s);
	Vector3 &operator-=(T s);
	Vector3 &operator*=(T s);
	Vector3 &operator/=(T s);

	Vector3 operator+(const Vector3 &v) const;
	Vector3 operator-(const Vector3 &v) const;
	Vector3 operator*(const Vector3 &v) const;
	Vector3 operator/(const Vector3 &v) const;

	Vector3 operator+(T s) const;
	Vector3 operator-(T s) const;
	Vector3 operator*(T s) const;
	Vector3 operator/(T s) const;

	T length() const;
	T sqrLength() const;
	Vector3 normalized() const;
	Vector3 &normalize();

	Vector2<T> toVector2() const;

	template <class S> friend S dot(const Vector3<S> &v1, const Vector3<S> &v2);
	template <class S> friend Vector3<S> cross(const Vector3<S> &v1, const Vector3<S> &v2);

	/// A vector with all zero elements
	static const Vector3 Zero;
	/// A unit vector on the X axis
	static const Vector3 XAxis;
	/// A unit vector on the Y axis
	static const Vector3 YAxis;
	/// A unit vector on the Z axis
	static const Vector3 ZAxis;
};

typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;

template <class T>
inline void Vector3<T>::set(T xx, T yy, T zz)
{
	x = xx;
	y = yy;
	z = zz;
}

template <class T>
T *Vector3<T>::data()
{
	return &x;
}

template <class T>
const T *Vector3<T>::data() const
{
	return &x;
}

template <class T>
inline T &Vector3<T>::operator[](unsigned int index)
{
	return (&x)[index];
}

template <class T>
inline const T &Vector3<T>::operator[](unsigned int index) const
{
	return (&x)[index];
}

template <class T>
inline bool Vector3<T>::operator==(const Vector3 &v) const
{
	return (x == v.x && y == v.y && z == v.z);
}

template <class T>
inline Vector3<T> Vector3<T>::operator-() const
{
	return Vector3(-x, -y, -z);
}

template <class T>
inline Vector3<T> &Vector3<T>::operator+=(const Vector3 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

template <class T>
inline Vector3<T> &Vector3<T>::operator-=(const Vector3 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

template <class T>
inline Vector3<T> &Vector3<T>::operator*=(const Vector3 &v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;

	return *this;
}

template <class T>
inline Vector3<T> &Vector3<T>::operator/=(const Vector3 &v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;

	return *this;
}

template <class T>
inline Vector3<T> &Vector3<T>::operator+=(T s)
{
	x += s;
	y += s;
	z += s;

	return *this;
}

template <class T>
inline Vector3<T> &Vector3<T>::operator-=(T s)
{
	x -= s;
	y -= s;
	z -= s;

	return *this;
}

template <class T>
inline Vector3<T> &Vector3<T>::operator*=(T s)
{
	x *= s;
	y *= s;
	z *= s;

	return *this;
}

template <class T>
inline Vector3<T> &Vector3<T>::operator/=(T s)
{
	x /= s;
	y /= s;
	z /= s;

	return *this;
}

template <class T>
inline Vector3<T> Vector3<T>::operator+(const Vector3 &v) const
{
	return Vector3(x + v.x,
	               y + v.y,
	               z + v.z);
}

template <class T>
inline Vector3<T> Vector3<T>::operator-(const Vector3 &v) const
{
	return Vector3(x - v.x,
	               y - v.y,
	               z - v.z);
}

template <class T>
inline Vector3<T> Vector3<T>::operator*(const Vector3 &v) const
{
	return Vector3(x * v.x,
	               y * v.y,
	               z * v.z);
}

template <class T>
inline Vector3<T> Vector3<T>::operator/(const Vector3 &v) const
{
	return Vector3(x / v.x,
	               y / v.y,
	               z / v.z);
}

template <class T>
inline Vector3<T> Vector3<T>::operator+(T s) const
{
	return Vector3(x + s,
	               y + s,
	               z + s);
}

template <class T>
inline Vector3<T> Vector3<T>::operator-(T s) const
{
	return Vector3(x - s,
	               y - s,
	               z - s);
}

template <class T>
inline Vector3<T> Vector3<T>::operator*(T s) const
{
	return Vector3(x * s,
	               y * s,
	               z * s);
}

template <class T>
inline Vector3<T> Vector3<T>::operator/(T s) const
{
	return Vector3(x / s,
	               y / s,
	               z / s);
}

template <class T>
inline T Vector3<T>::length() const
{
	return sqrt(x * x + y * y + z * z);
}

template <class T>
inline T Vector3<T>::sqrLength() const
{
	return x * x + y * y + z * z;
}

template <class T>
inline Vector3<T> Vector3<T>::normalized() const
{
	T len = length();
	return Vector3(x / len, y / len, z / len);
}

template <class T>
inline Vector3<T> &Vector3<T>::normalize()
{
	T len = length();

	x /= len;
	y /= len;
	z /= len;

	return *this;
}

template <class T>
inline Vector2<T> Vector3<T>::toVector2() const
{
	return Vector2<T>(x, y);
}

template <class S>
inline S dot(const Vector3<S> &v1, const Vector3<S> &v2)
{
	return static_cast<S>(v1.x * v2.x +
	                      v1.y * v2.y +
	                      v1.z * v2.z);
}

template <class S>
inline Vector3<S> cross(const Vector3<S> &v1, const Vector3<S> &v2)
{
	return Vector3<S>(v1.y * v2.z - v1.z * v2.y,
	                  v1.z * v2.x - v1.x * v2.z,
	                  v1.x * v2.y - v1.y * v2.x);
}

template <class T> const Vector3<T> Vector3<T>::Zero(0, 0, 0);
template <class T> const Vector3<T> Vector3<T>::XAxis(1, 0, 0);
template <class T> const Vector3<T> Vector3<T>::YAxis(0, 1, 0);
template <class T> const Vector3<T> Vector3<T>::ZAxis(0, 0, 1);

}

#endif
