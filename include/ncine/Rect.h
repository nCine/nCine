#ifndef CLASS_NCINE_RECT
#define CLASS_NCINE_RECT

#include "Vector2.h"

namespace ncine {

/// A template based rectangle in a two dimensional space
template <class T>
class Rect
{
  public:
	/// Top-left X coordinate as a public property
	T x;
	/// Top-left Y coordinate as a public property
	T y;
	/// Width as a public property
	T w;
	/// Height as a public property
	T h;

	/// Default constructor, all zeros
	Rect()
	    : x(0), y(0), w(0), h(0) {}
	/// Constructs from elements
	Rect(T xx, T yy, T ww, T hh)
	    : x(xx), y(yy), w(ww), h(hh) {}
	/// Constructs from base classes
	Rect(const Vector2<T> &p1, const Vector2<T> &p2)
	    : x(p1.x), y(p1.y), w(p2.x), h(p2.y) {}

	/// Creates a rectangle from center and size
	static Rect fromCenterAndSize(T xx, T yy, T ww, T hh);
	/// Creates a rectangle from center and size as two `Vector2`
	static Rect fromCenterAndSize(const Vector2<T> &center, const Vector2<T> &size);

	/// Calculates the center of the rectangle
	Vector2<T> center() const;
	/// Sets rectangle elements
	void set(T xx, T yy, T ww, T hh);
	/// Retains rectangle size but moves its center to another position
	void setCenter(float cx, float cy);
	/// Retains rectangle size but moves its center to another position with a `Vector2`
	void setCenter(const Vector2<T> &center);

	/// \returns True if the point is inside this rect
	bool contains(T px, T py) const;
	/// \returns True if the point vector is inside this rect
	bool contains(const Vector2<T> &p) const;

	/// \returns True if this rect surrounds the other rect
	bool contains(const Rect<T> &rect) const;
	/// \returns True if this rect does overlap the other rect in any way
	bool overlaps(const Rect<T> &rect) const;

	/// Eqality operator
	bool operator==(const Rect &rect) const;
};

using Rectf = Rect<float>;
using Recti = Rect<int>;

template <class T>
inline Rect<T> Rect<T>::fromCenterAndSize(T xx, T yy, T ww, T hh)
{
	return Rect(xx - static_cast<T>(ww * 0.5f),
	            yy - static_cast<T>(hh * 0.5f),
	            ww, hh);
}

template <class T>
inline Rect<T> Rect<T>::fromCenterAndSize(const Vector2<T> &center, const Vector2<T> &size)
{
	return Rect(center.x - static_cast<T>(size.x * 0.5f),
	            center.y - static_cast<T>(size.y * 0.5f),
	            size.x, size.y);
}

template <class T>
inline Vector2<T> Rect<T>::center() const
{
	return Vector2<T>(x + static_cast<T>(w * 0.5f), y + static_cast<T>(h * 0.5f));
}

template <class T>
inline void Rect<T>::set(T xx, T yy, T ww, T hh)
{
	x = xx;
	y = yy;
	w = ww;
	h = hh;
}

template <class T>
inline void Rect<T>::setCenter(float cx, float cy)
{
	x = cx - static_cast<T>(w * 0.5f);
	y = cy - static_cast<T>(h * 0.5f);
}

template <class T>
inline void Rect<T>::setCenter(const Vector2<T> &center)
{
	x = center.x - static_cast<T>(w * 0.5f);
	y = center.y - static_cast<T>(h * 0.5f);
}

template <class T>
inline bool Rect<T>::contains(T px, T py) const
{
	return (px >= x && py >= y &&
	        px <= x + w && py <= y + h);
}

template <class T>
inline bool Rect<T>::contains(const Vector2<T> &p) const
{
	return (p.x >= x && p.y >= y &&
	        p.x <= x + w && p.y <= y + h);
}

template <class T>
inline bool Rect<T>::contains(const Rect &rect) const
{
	return (x <= rect.x && y <= rect.y &&
	        x + w >= rect.x + rect.w && y + h >= rect.y + rect.h);
}

template <class T>
inline bool Rect<T>::overlaps(const Rect &rect) const
{
	return !(x > rect.x + rect.w || y > rect.y + rect.h ||
	         x + w < rect.x || y + h < rect.y);
}

template <class T>
inline bool Rect<T>::operator==(const Rect &rect) const
{
	return (x == rect.x && y == rect.y &&
	        w == rect.w && h == rect.h);
}

}

#endif
