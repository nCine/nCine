#ifndef CLASS_NCINE_VECTOR4
#define CLASS_NCINE_VECTOR4

#include "Vector2.h"
#include "Vector3.h"

namespace ncine {

/// A four component vector based on templates
template <class T>
class Vector4
{
  public:
	T x, y, z, w;

	Vector4() { }
	explicit Vector4(T s) : x(s), y(s), z(s), w(s) { }
	Vector4(T xx, T yy, T zz, T ww) : x(xx), y(yy), z(zz), w(ww) { }

	void set(T xx, T yy, T zz, T ww);

	T *data();
	const T *data() const;

	T &operator[](unsigned int index);
	const T &operator[](unsigned int index) const;

	bool operator==(const Vector4 &v) const;
	Vector4 operator-() const;

	Vector4 &operator+=(const Vector4 &v);
	Vector4 &operator-=(const Vector4 &v);
	Vector4 &operator*=(const Vector4 &v);
	Vector4 &operator/=(const Vector4 &v);

	Vector4 &operator+=(T s);
	Vector4 &operator-=(T s);
	Vector4 &operator*=(T s);
	Vector4 &operator/=(T s);

	Vector4 operator+(const Vector4 &v) const;
	Vector4 operator-(const Vector4 &v) const;
	Vector4 operator*(const Vector4 &v) const;
	Vector4 operator/(const Vector4 &v) const;

	Vector4 operator+(T s) const;
	Vector4 operator-(T s) const;
	Vector4 operator*(T s) const;
	Vector4 operator/(T s) const;

	T length() const;
	T sqrLength() const;
	Vector4 normalized() const;
	Vector4 &normalize();

	Vector2<T> toVector2() const;
	Vector3<T> toVector3() const;

	template <class S> friend S dot(const Vector4<S> &v1, const Vector4<S> &v2);

	/// A vector with all zero elements
	static const Vector4 Zero;
	/// A unit vector on the X axis
	static const Vector4 XAxis;
	/// A unit vector on the Y axis
	static const Vector4 YAxis;
	/// A unit vector on the Z axis
	static const Vector4 ZAxis;
	/// A unit vector on the W axis
	static const Vector4 WAxis;
};

typedef Vector4<float> Vector4f;
typedef Vector4<int> Vector4i;

template <class T>
inline void Vector4<T>::set(T xx, T yy, T zz, T ww)
{
	x = xx;
	y = yy;
	z = zz;
	w = ww;
}

template <class T>
T *Vector4<T>::data()
{
	return &x;
}

template <class T>
const T *Vector4<T>::data() const
{
	return &x;
}

template <class T>
inline T &Vector4<T>::operator[](unsigned int index)
{
	ASSERT(index < 4);
	return (&x)[index];
}

template <class T>
inline const T &Vector4<T>::operator[](unsigned int index) const
{
	ASSERT(index < 4);
	return (&x)[index];
}

template <class T>
inline bool Vector4<T>::operator==(const Vector4 &v) const
{
	return (x == v.x && y == v.y && z == v.z && w == v.w);
}

template <class T>
inline Vector4<T> Vector4<T>::operator-() const
{
	return Vector4(-x, -y, -z, -w);
}

template <class T>
inline Vector4<T> &Vector4<T>::operator+=(const Vector4 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;

	return *this;
}

template <class T>
inline Vector4<T> &Vector4<T>::operator-=(const Vector4 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;

	return *this;
}

template <class T>
inline Vector4<T> &Vector4<T>::operator*=(const Vector4 &v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;

	return *this;
}

template <class T>
inline Vector4<T> &Vector4<T>::operator/=(const Vector4 &v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;

	return *this;
}

template <class T>
inline Vector4<T> &Vector4<T>::operator+=(T s)
{
	x += s;
	y += s;
	z += s;
	w += s;

	return *this;
}

template <class T>
inline Vector4<T> &Vector4<T>::operator-=(T s)
{
	x -= s;
	y -= s;
	z -= s;
	w -= s;

	return *this;
}

template <class T>
inline Vector4<T> &Vector4<T>::operator*=(T s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;

	return *this;
}

template <class T>
inline Vector4<T> &Vector4<T>::operator/=(T s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;

	return *this;
}

template <class T>
inline Vector4<T> Vector4<T>::operator+(const Vector4 &v) const
{
	return Vector4(x + v.x,
	               y + v.y,
	               z + v.z,
	               w + v.w);
}

template <class T>
inline Vector4<T> Vector4<T>::operator-(const Vector4 &v) const
{
	return Vector4(x - v.x,
	               y - v.y,
	               z - v.z,
	               w - v.w);
}

template <class T>
inline Vector4<T> Vector4<T>::operator*(const Vector4 &v) const
{
	return Vector4(x * v.x,
	               y * v.y,
	               z * v.z,
	               w * v.w);
}

template <class T>
inline Vector4<T> Vector4<T>::operator/(const Vector4 &v) const
{
	return Vector4(x / v.x,
	               y / v.y,
	               z / v.z,
	               w / v.w);
}

template <class T>
inline Vector4<T> Vector4<T>::operator+(T s) const
{
	return Vector4(x + s,
	               y + s,
	               z + s,
	               w + s);
}

template <class T>
inline Vector4<T> Vector4<T>::operator-(T s) const
{
	return Vector4(x - s,
	               y - s,
	               z - s,
	               w - s);
}

template <class T>
inline Vector4<T> Vector4<T>::operator*(T s) const
{
	return Vector4(x * s,
	               y * s,
	               z * s,
	               w * s);
}

template <class T>
inline Vector4<T> Vector4<T>::operator/(T s) const
{
	return Vector4(x / s,
	               y / s,
	               z / s,
	               w / s);
}

template <class T>
inline T Vector4<T>::length() const
{
	return sqrt(x * x + y * y + z * z + w * w);
}

template <class T>
inline T Vector4<T>::sqrLength() const
{
	return x * x + y * y + z * z + w * w;
}

template <class T>
inline Vector4<T> Vector4<T>::normalized() const
{
	T len = length();
	return Vector4(x / len, y / len, z / len, w / len);
}

template <class T>
inline Vector4<T> &Vector4<T>::normalize()
{
	T len = length();

	x /= len;
	y /= len;
	z /= len;
	w /= len;

	return *this;
}

template <class T>
inline Vector2<T> Vector4<T>::toVector2() const
{
	return Vector2<T>(x, y);
}

template <class T>
inline Vector3<T> Vector4<T>::toVector3() const
{
	return Vector3<T>(x, y, z);
}

template <class S>
inline S dot(const Vector4<S> &v1, const Vector4<S> &v2)
{
	return static_cast<S>(v1.x * v2.x +
	                      v1.y * v2.y +
	                      v1.z * v2.z +
	                      v1.w * v2.w);
}

template <class T> const Vector4<T> Vector4<T>::Zero(0, 0, 0, 0);
template <class T> const Vector4<T> Vector4<T>::XAxis(1, 0, 0, 0);
template <class T> const Vector4<T> Vector4<T>::YAxis(0, 1, 0, 0);
template <class T> const Vector4<T> Vector4<T>::ZAxis(0, 0, 1, 0);
template <class T> const Vector4<T> Vector4<T>::WAxis(0, 0, 0, 1);

}

#endif
