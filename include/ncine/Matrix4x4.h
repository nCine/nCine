#ifndef CLASS_NCINE_MATRIX4X4
#define CLASS_NCINE_MATRIX4X4

#include "common_constants.h"
#include "Vector3.h"
#include "Vector4.h"

namespace ncine {

/// A four by four matrix based on templates
template <class T>
class Matrix4x4
{
  public:
	Matrix4x4()
	    : vecs_{ Vector4<T>(1, 0, 0, 0), Vector4<T>(0, 1, 0, 0), Vector4<T>(0, 0, 1, 0), Vector4<T>(0, 0, 0, 1) } {}
	Matrix4x4(const Vector4<T> &v0, const Vector4<T> &v1, const Vector4<T> &v2, const Vector4<T> &v3);

	void set(const Vector4<T> &v0, const Vector4<T> &v1, const Vector4<T> &v2, const Vector4<T> &v3);

	T *data();
	const T *data() const;

	Vector4<T> &operator[](unsigned int index);
	const Vector4<T> &operator[](unsigned int index) const;

	bool operator==(const Matrix4x4 &m) const;
	Matrix4x4 operator-() const;

	Matrix4x4 &operator+=(const Matrix4x4 &m);
	Matrix4x4 &operator-=(const Matrix4x4 &m);
	Matrix4x4 &operator*=(const Matrix4x4 &m);
	Matrix4x4 &operator/=(const Matrix4x4 &m);

	Matrix4x4 &operator+=(T s);
	Matrix4x4 &operator-=(T s);
	Matrix4x4 &operator*=(T s);
	Matrix4x4 &operator/=(T s);

	Vector4<T> operator*(const Vector4<T> &v) const;
	Vector3<T> operator*(const Vector3<T> &v) const;

	template <class S>
	friend Vector4<S> operator*(const Vector4<S> &v, const Matrix4x4<S> &m);
	template <class S>
	friend Vector3<S> operator*(const Vector3<S> &v, const Matrix4x4<S> &m);

	Matrix4x4 operator+(const Matrix4x4 &m) const;
	Matrix4x4 operator-(const Matrix4x4 &m) const;
	Matrix4x4 operator*(const Matrix4x4 &m) const;
	Matrix4x4 operator/(const Matrix4x4 &m) const;

	Matrix4x4 operator+(T s) const;
	Matrix4x4 operator-(T s) const;
	Matrix4x4 operator*(T s) const;
	Matrix4x4 operator/(T s) const;

	template <class S>
	friend Matrix4x4<S> operator*(S s, const Matrix4x4<S> &m);

	Matrix4x4 transposed() const;
	Matrix4x4 &transpose();
	Matrix4x4 inverse() const;

	Matrix4x4 &translate(T xx, T yy, T zz);
	Matrix4x4 &translate(const Vector3<T> &v);
	Matrix4x4 &rotateX(T degrees);
	Matrix4x4 &rotateY(T degrees);
	Matrix4x4 &rotateZ(T degrees);
	Matrix4x4 &scale(T xx, T yy, T zz);
	Matrix4x4 &scale(const Vector3<T> &v);
	Matrix4x4 &scale(T s);

	static Matrix4x4 translation(T xx, T yy, T zz);
	static Matrix4x4 translation(const Vector3<T> &v);
	static Matrix4x4 rotationX(T degrees);
	static Matrix4x4 rotationY(T degrees);
	static Matrix4x4 rotationZ(T degrees);
	static Matrix4x4 scaling(T xx, T yy, T zz);
	static Matrix4x4 scaling(const Vector3<T> &v);
	static Matrix4x4 scaling(T s);

	static Matrix4x4 ortho(T left, T right, T bottom, T top, T near, T far);
	static Matrix4x4 frustum(T left, T right, T bottom, T top, T near, T far);
	static Matrix4x4 perspective(T fovY, T aspect, T near, T far);

	/// A matrix with all zero elements
	static const Matrix4x4 Zero;
	/// An identity matrix
	static const Matrix4x4 Identity;

  private:
	Vector4<T> vecs_[4];
};

using Matrix4x4f = Matrix4x4<float>;

template <class T>
inline Matrix4x4<T>::Matrix4x4(const Vector4<T> &v0, const Vector4<T> &v1, const Vector4<T> &v2, const Vector4<T> &v3)
{
	set(v0, v1, v2, v3);
}

template <class T>
inline void Matrix4x4<T>::set(const Vector4<T> &v0, const Vector4<T> &v1, const Vector4<T> &v2, const Vector4<T> &v3)
{
	vecs_[0] = v0;
	vecs_[1] = v1;
	vecs_[2] = v2;
	vecs_[3] = v3;
}

template <class T>
inline T *Matrix4x4<T>::data()
{
	return &vecs_[0][0];
}

template <class T>
inline const T *Matrix4x4<T>::data() const
{
	return &vecs_[0][0];
}

template <class T>
inline Vector4<T> &Matrix4x4<T>::operator[](unsigned int index)
{
	index = (index < 4) ? index : 3;
	return vecs_[index];
}

template <class T>
inline const Vector4<T> &Matrix4x4<T>::operator[](unsigned int index) const
{
	index = (index < 4) ? index : 3;
	return vecs_[index];
}

template <class T>
inline bool Matrix4x4<T>::operator==(const Matrix4x4 &m) const
{
	return (vecs_[0] == m[0] && vecs_[1] == m[1] && vecs_[2] == m[2] && vecs_[3] == m[3]);
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::operator-() const
{
	return Matrix4x4(-vecs_[0], -vecs_[1], -vecs_[2], -vecs_[3]);
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::operator+=(const Matrix4x4 &m)
{
	vecs_[0] += m[0];
	vecs_[1] += m[1];
	vecs_[2] += m[2];
	vecs_[3] += m[3];

	return *this;
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::operator-=(const Matrix4x4 &m)
{
	vecs_[0] -= m[0];
	vecs_[1] -= m[1];
	vecs_[2] -= m[2];
	vecs_[3] -= m[3];

	return *this;
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::operator*=(const Matrix4x4 &m)
{
	return (*this = *this * m);
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::operator/=(const Matrix4x4 &m)
{
	vecs_[0] /= m[0];
	vecs_[1] /= m[1];
	vecs_[2] /= m[2];
	vecs_[3] /= m[3];

	return *this;
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::operator+=(T s)
{
	vecs_[0] += s;
	vecs_[1] += s;
	vecs_[2] += s;
	vecs_[3] += s;

	return *this;
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::operator-=(T s)
{
	vecs_[0] -= s;
	vecs_[1] -= s;
	vecs_[2] -= s;
	vecs_[3] -= s;

	return *this;
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::operator*=(T s)
{
	vecs_[0] *= s;
	vecs_[1] *= s;
	vecs_[2] *= s;
	vecs_[3] *= s;

	return *this;
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::operator/=(T s)
{
	vecs_[0] /= s;
	vecs_[1] /= s;
	vecs_[2] /= s;
	vecs_[3] /= s;

	return *this;
}

template <class T>
inline Vector4<T> Matrix4x4<T>::operator*(const Vector4<T> &v) const
{
	const Matrix4x4 &m = *this;

	return Vector4<T>(m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3],
	                  m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3],
	                  m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3],
	                  m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3] * v[3]);
}

template <class T>
inline Vector3<T> Matrix4x4<T>::operator*(const Vector3<T> &v) const
{
	const Matrix4x4 &m = *this;

	return Vector3<T>(m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2],
	                  m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2],
	                  m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2]);
}

template <class S>
inline Vector4<S> operator*(const Vector4<S> &v, const Matrix4x4<S> &m)
{
	return Vector4<S>(m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3],
	                  m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3],
	                  m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3],
	                  m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3]);
}

template <class S>
inline Vector3<S> operator*(const Vector3<S> &v, const Matrix4x4<S> &m)
{
	return Vector3<S>(m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2],
	                  m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2],
	                  m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2]);
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4 &m) const
{
	return Matrix4x4(vecs_[0] + m[0],
	                 vecs_[1] + m[1],
	                 vecs_[2] + m[2],
	                 vecs_[3] + m[3]);
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4 &m) const
{
	return Matrix4x4(vecs_[0] - m[0],
	                 vecs_[1] - m[1],
	                 vecs_[2] - m[2],
	                 vecs_[3] - m[3]);
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4 &m2) const
{
	const Matrix4x4 &m1 = *this;
	Matrix4x4 result;

	result[0] = m1[0] * m2[0][0] + m1[1] * m2[0][1] + m1[2] * m2[0][2] + m1[3] * m2[0][3];
	result[1] = m1[0] * m2[1][0] + m1[1] * m2[1][1] + m1[2] * m2[1][2] + m1[3] * m2[1][3];
	result[2] = m1[0] * m2[2][0] + m1[1] * m2[2][1] + m1[2] * m2[2][2] + m1[3] * m2[2][3];
	result[3] = m1[0] * m2[3][0] + m1[1] * m2[3][1] + m1[2] * m2[3][2] + m1[3] * m2[3][3];

	return result;
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::operator/(const Matrix4x4 &m) const
{
	return Matrix4x4(vecs_[0] / m[0],
	                 vecs_[1] / m[1],
	                 vecs_[2] / m[2],
	                 vecs_[3] / m[3]);
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::operator+(T s) const
{
	return Matrix4x4(vecs_[0] + s,
	                 vecs_[1] + s,
	                 vecs_[2] + s,
	                 vecs_[3] + s);
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::operator-(T s) const
{
	return Matrix4x4(vecs_[0] - s,
	                 vecs_[1] - s,
	                 vecs_[2] - s,
	                 vecs_[3] - s);
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::operator*(T s) const
{
	return Matrix4x4(vecs_[0] * s,
	                 vecs_[1] * s,
	                 vecs_[2] * s,
	                 vecs_[3] * s);
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::operator/(T s) const
{
	return Matrix4x4(vecs_[0] / s,
	                 vecs_[1] / s,
	                 vecs_[2] / s,
	                 vecs_[3] / s);
}

template <class S>
inline Matrix4x4<S> operator*(S s, const Matrix4x4<S> &m)
{
	return Matrix4x4<S>(s * m.vecs_[0],
	                    s * m.vecs_[1],
	                    s * m.vecs_[2],
	                    s * m.vecs_[3]);
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::transposed() const
{
	const Matrix4x4 &m = *this;
	Matrix4x4 result;

	result[0][0] = m[0][0];
	result[0][1] = m[1][0];
	result[0][2] = m[2][0];
	result[0][3] = m[3][0];

	result[1][0] = m[0][1];
	result[1][1] = m[1][1];
	result[1][2] = m[2][1];
	result[1][3] = m[3][1];

	result[2][0] = m[0][2];
	result[2][1] = m[1][2];
	result[2][2] = m[2][2];
	result[2][3] = m[3][2];

	result[3][0] = m[0][3];
	result[3][1] = m[1][3];
	result[3][2] = m[2][3];
	result[3][3] = m[3][3];

	return result;
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::transpose()
{
	Matrix4x4 &m = *this;
	T x;

	// clang-format off
	x = m[0][1]; m[0][1] = m[1][0]; m[1][0] = x;
	x = m[0][2]; m[0][2] = m[2][0]; m[2][0] = x;
	x = m[0][3]; m[0][3] = m[3][0]; m[3][0] = x;
	x = m[1][2]; m[1][2] = m[2][1]; m[2][1] = x;
	x = m[1][3]; m[1][3] = m[3][1]; m[3][1] = x;
	x = m[2][3]; m[2][3] = m[3][2]; m[3][2] = x;
	// clang-format on

	return *this;
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::inverse() const
{
	const Matrix4x4 &m = *this;

	const T coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
	const T coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
	const T coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

	const T coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	const T coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
	const T coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

	const T coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	const T coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
	const T coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

	const T coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	const T coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
	const T coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

	const T coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	const T coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
	const T coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

	const T coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
	const T coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
	const T coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

	const Vector4<T> fac0(coef00, coef00, coef02, coef03);
	const Vector4<T> fac1(coef04, coef04, coef06, coef07);
	const Vector4<T> fac2(coef08, coef08, coef10, coef11);
	const Vector4<T> fac3(coef12, coef12, coef14, coef15);
	const Vector4<T> fac4(coef16, coef16, coef18, coef19);
	const Vector4<T> fac5(coef20, coef20, coef22, coef23);

	const Vector4<T> vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
	const Vector4<T> vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
	const Vector4<T> vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
	const Vector4<T> vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

	const Vector4<T> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
	const Vector4<T> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
	const Vector4<T> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
	const Vector4<T> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

	const Vector4<T> signA(+1, -1, +1, -1);
	const Vector4<T> signB(-1, +1, -1, +1);
	const Matrix4x4 inverse(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

	const Vector4<T> row0(inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]);

	const Vector4<T> dot0(m[0] * row0);
	const T dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

	const T oneOverDeterminant = 1 / dot1;

	return inverse * oneOverDeterminant;
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::translate(T xx, T yy, T zz)
{
	Matrix4x4 &m = *this;

	m[3][0] += xx * m[0][0] + yy * m[1][0] + zz * m[2][0];
	m[3][1] += xx * m[0][1] + yy * m[1][1] + zz * m[2][1];
	m[3][2] += xx * m[0][2] + yy * m[1][2] + zz * m[2][2];

	return *this;
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::translate(const Vector3<T> &v)
{
	return translate(v.x, v.y, v.z);
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::rotateX(T degrees)
{
	Matrix4x4 &m = *this;
	const T m10 = m[1][0];
	const T m20 = m[2][0];
	const T m11 = m[1][1];
	const T m21 = m[2][1];
	const T m12 = m[1][2];
	const T m22 = m[2][2];
	const T m13 = m[1][3];
	const T m23 = m[2][3];

	const T radians = degrees * (static_cast<T>(Pi) / 180);
	const T c = cos(radians);
	const T s = sin(radians);

	m[1][0] = c * m10 + s * m20;
	m[1][1] = c * m11 + s * m21;
	m[1][2] = c * m12 + s * m22;
	m[1][3] = c * m13 + s * m23;

	m[2][0] = -s * m10 + c * m20;
	m[2][1] = -s * m11 + c * m21;
	m[2][2] = -s * m12 + c * m22;
	m[2][3] = -s * m13 + c * m23;

	return *this;
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::rotateY(T degrees)
{
	Matrix4x4 &m = *this;
	const T m00 = m[0][0];
	const T m20 = m[2][0];
	const T m01 = m[0][1];
	const T m21 = m[2][1];
	const T m02 = m[0][2];
	const T m22 = m[2][2];
	const T m03 = m[0][3];
	const T m23 = m[2][3];

	const T radians = degrees * (static_cast<T>(Pi) / 180);
	const T c = cos(radians);
	const T s = sin(radians);

	m[0][0] = c * m00 - s * m20;
	m[0][1] = c * m01 - s * m21;
	m[0][2] = c * m02 - s * m22;
	m[0][3] = c * m03 - s * m23;

	m[2][0] = s * m00 + c * m20;
	m[2][1] = s * m01 + c * m21;
	m[2][2] = s * m02 + c * m22;
	m[2][3] = s * m03 + c * m23;

	return *this;
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::rotateZ(T degrees)
{
	Matrix4x4 &m = *this;
	const T m00 = m[0][0];
	const T m10 = m[1][0];
	const T m01 = m[0][1];
	const T m11 = m[1][1];
	const T m02 = m[0][2];
	const T m12 = m[1][2];
	const T m03 = m[0][3];
	const T m13 = m[1][3];

	const T radians = degrees * (static_cast<T>(Pi) / 180);
	const T c = cos(radians);
	const T s = sin(radians);

	m[0][0] = c * m00 + s * m10;
	m[0][1] = c * m01 + s * m11;
	m[0][2] = c * m02 + s * m12;
	m[0][3] = c * m03 + s * m13;

	m[1][0] = -s * m00 + c * m10;
	m[1][1] = -s * m01 + c * m11;
	m[1][2] = -s * m02 + c * m12;
	m[1][3] = -s * m03 + c * m13;

	return *this;
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::scale(T xx, T yy, T zz)
{
	Matrix4x4 &m = *this;

	m[0][0] *= xx;
	m[0][1] *= xx;
	m[0][2] *= xx;

	m[1][0] *= yy;
	m[1][1] *= yy;
	m[1][2] *= yy;

	m[2][0] *= zz;
	m[2][1] *= zz;
	m[2][2] *= zz;

	return *this;
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::scale(const Vector3<T> &v)
{
	return scale(v.x, v.y, v.z);
}

template <class T>
inline Matrix4x4<T> &Matrix4x4<T>::scale(T s)
{
	return scale(s, s, s);
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::translation(T xx, T yy, T zz)
{
	return Matrix4x4(Vector4<T>(1, 0, 0, 0),
	                 Vector4<T>(0, 1, 0, 0),
	                 Vector4<T>(0, 0, 1, 0),
	                 Vector4<T>(xx, yy, zz, 1));
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::translation(const Vector3<T> &v)
{
	return translation(v.x, v.y, v.z);
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::rotationX(T degrees)
{
	const T radians = degrees * (static_cast<T>(Pi) / 180);
	const T c = cos(radians);
	const T s = sin(radians);

	return Matrix4x4(Vector4<T>(1, 0, 0, 0),
	                 Vector4<T>(0, c, s, 0),
	                 Vector4<T>(0, -s, c, 0),
	                 Vector4<T>(0, 0, 0, 1));
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::rotationY(T degrees)
{
	const T radians = degrees * (static_cast<T>(Pi) / 180);
	const T c = cos(radians);
	const T s = sin(radians);

	return Matrix4x4(Vector4<T>(c, 0, -s, 0),
	                 Vector4<T>(0, 1, 0, 0),
	                 Vector4<T>(s, 0, c, 0),
	                 Vector4<T>(0, 0, 0, 1));
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::rotationZ(T degrees)
{
	const T radians = degrees * (static_cast<T>(Pi) / 180);
	const T c = cos(radians);
	const T s = sin(radians);

	return Matrix4x4(Vector4<T>(c, s, 0, 0),
	                 Vector4<T>(-s, c, 0, 0),
	                 Vector4<T>(0, 0, 1, 0),
	                 Vector4<T>(0, 0, 0, 1));
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::scaling(T xx, T yy, T zz)
{
	return Matrix4x4(Vector4<T>(xx, 0, 0, 0),
	                 Vector4<T>(0, yy, 0, 0),
	                 Vector4<T>(0, 0, zz, 0),
	                 Vector4<T>(0, 0, 0, 1));
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::scaling(const Vector3<T> &v)
{
	return scaling(v.x, v.y, v.z);
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::scaling(T s)
{
	return scaling(s, s, s);
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::ortho(T left, T right, T bottom, T top, T near, T far)
{
	return Matrix4x4(Vector4<T>(2 / (right - left), 0, 0, 0),
	                 Vector4<T>(0, 2 / (top - bottom), 0, 0),
	                 Vector4<T>(0, 0, -2 / (far - near), 0),
	                 Vector4<T>(-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1));
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::frustum(T left, T right, T bottom, T top, T near, T far)
{
	return Matrix4x4(Vector4<T>((2 * near) / (right - left), 0, 0, 0),
	                 Vector4<T>(0, (2 * near) / (top - bottom), 0, 0),
	                 Vector4<T>((right + left) / (right - left), (top + bottom) / (top - bottom), -(far + near) / (far - near), -1),
	                 Vector4<T>(0, 0, (-2 * far * near) / (far - near), 0));
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::perspective(T fovY, T aspect, T near, T far)
{
	const T yMax = near * tan(fovY * static_cast<T>(Pi) / 360);
	const T yMin = -yMax;
	const T xMin = yMin * aspect;
	const T xMax = yMax * aspect;

	return frustum(xMin, xMax, yMin, yMax, near, far);
}

template <class T>
const Matrix4x4<T> Matrix4x4<T>::Zero(Vector4<T>(0, 0, 0, 0), Vector4<T>(0, 0, 0, 0), Vector4<T>(0, 0, 0, 0), Vector4<T>(0, 0, 0, 0));
template <class T>
const Matrix4x4<T> Matrix4x4<T>::Identity(Vector4<T>(1, 0, 0, 0), Vector4<T>(0, 1, 0, 0), Vector4<T>(0, 0, 1, 0), Vector4<T>(0, 0, 0, 1));

}

#endif
