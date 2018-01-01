#ifndef CLASS_NCINE_QUATERNION
#define CLASS_NCINE_QUATERNION

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
	explicit Quaternion(const Vector4<T> &v) : x(v.x), y(v.y), z(v.z), w(v.w) { }

	void set(T xx, T yy, T zz, T ww);

	T *data();
	const T *data() const;

	T &operator[](unsigned int index);
	const T &operator[](unsigned int index) const;

	bool operator==(const Quaternion &q) const;
	Quaternion operator-() const;

	Quaternion &operator+=(const Quaternion &q);
	Quaternion &operator-=(const Quaternion &q);
	Quaternion &operator*=(const Quaternion &q);

	Quaternion &operator*=(T s);
	Quaternion &operator/=(T s);

	Quaternion operator+(const Quaternion &q) const;
	Quaternion operator-(const Quaternion &q) const;
	Quaternion operator*(const Quaternion &q) const;

	Quaternion operator*(T s) const;
	Quaternion operator/(T s) const;

	T magnitude() const;
	T sqrMagnitude() const;
	Quaternion normalized() const;
	Quaternion &normalize();
	Quaternion conjugated() const;
	Quaternion &conjugate();

	Matrix4x4<T> toMatrix4x4() const;
	static Quaternion fromAxisAngle(T xx, T yy, T zz, T degrees);
	static Quaternion fromAxisAngle(const Vector3<T> &axis, T degrees);
	static Quaternion fromXAxisAngle(T degrees);
	static Quaternion fromYAxisAngle(T degrees);
	static Quaternion fromZAxisAngle(T degrees);

	/// A quaternion with all zero elements
	static const Quaternion Zero;
	/// An identity quaternion
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
T *Quaternion<T>::data()
{
	return &x;
}

template <class T>
const T *Quaternion<T>::data() const
{
	return &x;
}

template <class T>
inline T &Quaternion<T>::operator[](unsigned int index)
{
	ASSERT(index < 4);
	return (&x)[index];
}

template <class T>
inline const T &Quaternion<T>::operator[](unsigned int index) const
{
	ASSERT(index < 4);
	return (&x)[index];
}

template <class T>
inline bool Quaternion<T>::operator==(const Quaternion &q) const
{
	return (x == q.x && y == q.y && z == q.z && w == q.w);
}

template <class T>
inline Quaternion<T> Quaternion<T>::operator-() const
{
	return Quaternion(-x, -y, -z, w);
}

template <class T>
inline Quaternion<T> &Quaternion<T>::operator+=(const Quaternion &q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;

	return *this;
}

template <class T>
inline Quaternion<T> &Quaternion<T>::operator-=(const Quaternion &q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;

	return *this;
}

template <class T>
inline Quaternion<T> &Quaternion<T>::operator*=(const Quaternion &q)
{
	const Quaternion<T> q0 = *this;

	x = q0.w * q.x + q0.x * q.w + q0.y * q.z - q0.z * q.y;
	y = q0.w * q.y + q0.y * q.w + q0.z * q.x - q0.x * q.z;
	z = q0.w * q.z + q0.z * q.w + q0.x * q.y - q0.y * q.x;
	w = q0.w * q.w - q0.x * q.x - q0.y * q.y - q0.z * q.z;

	return *this;
}

template <class T>
inline Quaternion<T> &Quaternion<T>::operator*=(T s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;

	return *this;
}

template <class T>
inline Quaternion<T> &Quaternion<T>::operator/=(T s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;

	return *this;
}

template <class T>
inline Quaternion<T> Quaternion<T>::operator+(const Quaternion &q) const
{
	return Quaternion(x + q.x,
	                  y + q.y,
	                  z + q.z,
	                  w + q.w);
}

template <class T>
inline Quaternion<T> Quaternion<T>::operator-(const Quaternion &q) const
{
	return Quaternion(x - q.x,
	                  y - q.y,
	                  z - q.z,
	                  w - q.w);
}

template <class T>
inline Quaternion<T> Quaternion<T>::operator*(const Quaternion &q) const
{
	return Quaternion(w * q.x + x * q.w + y * q.z - z * q.y,
	                  w * q.y + y * q.w + z * q.x - x * q.z,
	                  w * q.z + z * q.w + x * q.y - y * q.x,
	                  w * q.w - x * q.x - y * q.y - z * q.z);
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
	return sqrt(x * x + y * y + z * z + w * w);
}

template <class T>
inline T Quaternion<T>::sqrMagnitude() const
{
	return x * x + y * y + z * z + w * w;
}

template <class T>
inline Quaternion<T> Quaternion<T>::normalized() const
{
	const T mag = magnitude();
	return Quaternion(x / mag, y / mag, z / mag, w / mag);
}

template <class T>
inline Quaternion<T> &Quaternion<T>::normalize()
{
	const T mag = magnitude();

	x /= mag;
	y /= mag;
	z /= mag;
	w /= mag;

	return *this;
}

template <class T>
inline Quaternion<T> Quaternion<T>::conjugated() const
{
	return Quaternion(-x, -y, -z, w);
}

template <class T>
inline Quaternion<T> &Quaternion<T>::conjugate()
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

template <class T>
inline Matrix4x4<T> Quaternion<T>::toMatrix4x4() const
{
	const T x2 = x * 2;
	const T y2 = y * 2;
	const T z2 = z * 2;

	const T xx = x * x2;
	const T xy = x * y2;
	const T xz = x * z2;
	const T yy = y * y2;
	const T yz = y * z2;
	const T zz = z * z2;

	const T xw = w * x2;
	const T yw = w * y2;
	const T zw = w * z2;

	return Matrix4x4<T>(Vector4<T>(1 - (yy + zz), xy + zw, xz - yw, 0),
	                    Vector4<T>(xy - zw, 1 - (xx + zz), yz + xw, 0),
	                    Vector4<T>(xz + yw, yz - xw, 1 - (xx + yy), 0),
	                    Vector4<T>(0, 0, 0, 1));
}

template <class T>
inline Quaternion<T> Quaternion<T>::fromAxisAngle(T xx, T yy, T zz, T degrees)
{
	const T halfRadians = static_cast<T>(degrees * 0.5f) * (static_cast<T>(Pi) / 180);
	const T sinus = sin(halfRadians);

	return Quaternion<T>(xx * sinus,
	                     yy * sinus,
	                     zz * sinus,
	                     cos(halfRadians));
}

template <class T>
inline Quaternion<T> Quaternion<T>::fromAxisAngle(const Vector3<T> &axis, T degrees)
{
	return fromAxisAngle(axis.x, axis.y, axis.z, degrees);
}

template <class T>
inline Quaternion<T> Quaternion<T>::fromXAxisAngle(T degrees)
{
	const T halfRadians = static_cast<T>(degrees * 0.5f) * (static_cast<T>(Pi) / 180);
	return Quaternion<T>(sin(halfRadians), 0, 0, cos(halfRadians));
}

template <class T>
inline Quaternion<T> Quaternion<T>::fromYAxisAngle(T degrees)
{
	const T halfRadians = static_cast<T>(degrees * 0.5f) * (static_cast<T>(Pi) / 180);
	return Quaternion<T>(0, sin(halfRadians), 0, cos(halfRadians));
}

template <class T>
inline Quaternion<T> Quaternion<T>::fromZAxisAngle(T degrees)
{
	const T halfRadians = static_cast<T>(degrees * 0.5f) * (static_cast<T>(Pi) / 180);
	return Quaternion<T>(0, 0, sin(halfRadians), cos(halfRadians));
}

template <class T> const Quaternion<T> Quaternion<T>::Zero(0, 0, 0, 0);
template <class T> const Quaternion<T> Quaternion<T>::Identity(0, 0, 0, 1);

}

#endif
