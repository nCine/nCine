#ifndef CLASS_NCINE_QUATERNION
#define CLASS_NCINE_QUATERNION

#define _USE_MATH_DEFINES // for M_PI on MSVC
#include <cmath>
#include "Matrix4x4.h"

namespace ncine {

/// A quaternion class based on templates
template <class T>
class Quaternion
{
  public:
	T x, y, z, w;

	Quaternion() { }
	Quaternion(T xx, T yy, T zz, T ww) : x(xx), y(yy), z(zz), w(ww) { }
	explicit Quaternion(const Vector4<T>& v) : x(v.x), y(v.y), z(v.z), w(v.w) { }

	void set(T xx, T yy, T zz, T ww);

	T& operator[](unsigned int index);
	const T& operator[](unsigned int index) const;

	bool operator==(const Quaternion& q) const;
	Quaternion operator-() const;

	Quaternion& operator+=(const Quaternion& q);
	Quaternion& operator-=(const Quaternion& q);
	Quaternion& operator*=(const Quaternion& q);

	Quaternion& operator*=(T s);
	Quaternion& operator/=(T s);

	Quaternion operator+(const Quaternion& q) const;
	Quaternion operator-(const Quaternion& q) const;
	Quaternion operator*(const Quaternion& q) const;

	Quaternion operator*(T s) const;
	Quaternion operator/(T s) const;

	T magnitude() const;
	T sqrMagnitude() const;
	Quaternion normalized() const;
	Quaternion& normalize();
	Quaternion conjugated() const;
	Quaternion& conjugate();

	Matrix4x4<T> toMatrix4x4();
	static Quaternion fromAxisAngle(T xx, T yy, T zz, T degrees);
	static Quaternion fromAxisAngle(const Vector3<T>& axis, T degrees);

	static const Quaternion Zero;
	static const Quaternion Identity;
};

typedef Quaternion<float> Quaternionf;

template <class T>
inline void Quaternion<T>::set(T xx, T yy, T zz, T ww)
{
	x = xx;
	y = yy;
	z = zz;
	w = ww;
}

template <class T>
inline T& Quaternion<T>::operator[](unsigned int index)
{
	return (&x)[index];
}

template <class T>
inline const T& Quaternion<T>::operator[](unsigned int index) const
{
	return (&x)[index];
}

template <class T>
inline bool Quaternion<T>::operator==(const Quaternion& q) const
{
	return (x == q.x && y == q.y && z == q.z && w == q.w);
}

template <class T>
inline Quaternion<T> Quaternion<T>::operator-() const
{
	return Quaternion(-x, -y, -z, w);
}

template <class T>
inline Quaternion<T>& Quaternion<T>::operator+=(const Quaternion& q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;

	return *this;
}

template <class T>
inline Quaternion<T>& Quaternion<T>::operator-=(const Quaternion& q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;

	return *this;
}

template <class T>
inline Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& q)
{
	x = w*q.x + x*q.w + y*q.z - z*q.y;
	y = w*q.y + y*q.w + z*q.x - x*q.z;
	z = w*q.z + z*q.w + x*q.y - y*q.x;
	w = w*q.w - x*q.x - y*q.y - z*q.z;

	return *this;
}

template <class T>
inline Quaternion<T>& Quaternion<T>::operator*=(T s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;

	return *this;
}

template <class T>
inline Quaternion<T>& Quaternion<T>::operator/=(T s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;

	return *this;
}

template <class T>
inline Quaternion<T> Quaternion<T>::operator+(const Quaternion& q) const
{
	return Quaternion(x + q.x,
					  y + q.y,
					  z + q.z,
					  w + q.w);
}

template <class T>
inline Quaternion<T> Quaternion<T>::operator-(const Quaternion& q) const
{
	return Quaternion(x - q.x,
					  y - q.y,
					  z - q.z,
					  w - q.w);
}

template <class T>
inline Quaternion<T> Quaternion<T>::operator*(const Quaternion& q) const
{
	return Quaternion(w*q.x + x*q.w + y*q.z - z*q.y,
					  w*q.y + y*q.w + z*q.x - x*q.z,
					  w*q.z + z*q.w + x*q.y - y*q.x,
					  w*q.w - x*q.x - y*q.y - z*q.z);
}

template <class T>
inline Quaternion<T> Quaternion<T>::operator*(T s) const
{
	return Quaternion(x * s,
					  y * s,
					  z * s,
					  w * s);
}

template <class T>
inline Quaternion<T> Quaternion<T>::operator/(T s) const
{
	return Quaternion(x / s,
					  y / s,
					  z / s,
					  w / s);
}

template <class T>
inline T Quaternion<T>::magnitude() const
{
	return sqrt(x*x + y*y + z*z + w*w);
}

template <class T>
inline T Quaternion<T>::sqrMagnitude() const
{
	return x*x + y*y + z*z + w*w;
}

template <class T>
inline Quaternion<T> Quaternion<T>::normalized() const
{
	T mag = magnitude();
	return Quaternion(x / mag, y / mag , z / mag, w / mag);
}

template <class T>
inline Quaternion<T>& Quaternion<T>::normalize()
{
	T mag = magnitude();

	x /= mag;
	y /= mag;
	z /= mag;
	w /= mag;

	return *this;
}

template <class T>
inline Quaternion<T> Quaternion<T>::conjugated() const
{
	return Quaternion(-x, -y , -z, w);
}

template <class T>
inline Quaternion<T>& Quaternion<T>::conjugate()
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

template <class T>
inline Matrix4x4<T> Quaternion<T>::toMatrix4x4()
{
	const T xx = x * x;
	const T xy = x * y;
	const T xz = x * z;
	const T xw = x * w;

	const T yy = y * y;
	const T yz = y * z;
	const T yw = y * w;

	const T zz = z * z;
	const T zw = z * w;


	return Matrix4x4<T>(Vector4<T>(1 - 2*(yy + zz), 2*(xy - zw), 2*(xz + yw), 0),
						Vector4<T>(2*(xy + zw), 1 - 2*(xx + zz), 2*(yz - xw), 0),
						Vector4<T>(2*(xz - yw), 2*(yz + xw), 1 - 2*(xx + yy), 0),
						Vector4<T>(0, 0, 0, 1));
}

template <class T>
inline Quaternion<T> Quaternion<T>::fromAxisAngle(T xx, T yy, T zz, T degrees)
{
	T radians = degrees * (static_cast<T>(M_PI) / 180);
	T sinus = sin(radians / 2);

	return Quaternion<T>(xx * sinus,
						 yy * sinus,
						 zz * sinus,
						 cos(radians / 2));
}

template <class T>
inline Quaternion<T> Quaternion<T>::fromAxisAngle(const Vector3<T>& axis, T degrees)
{
	return fromAxisAngle(axis.x, axis.y, axis.z, degrees);
}

template <class T> const Quaternion<T> Quaternion<T>::Zero(0, 0, 0, 0);
template <class T> const Quaternion<T> Quaternion<T>::Identity(0, 0, 0, 1);

}

#endif
