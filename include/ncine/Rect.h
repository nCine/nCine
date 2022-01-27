#ifndef CLASS_NCINE_RECT
#define CLASS_NCINE_RECT

#include "Vector2.h"

namespace ncine {

/// A template based rectangle in a two dimensional space
template <class T>
class Rect
{
  public:
	/// Top-left X coordinate as a public property (left with positive width)
	T x;
	/// Top-left Y coordinate as a public property (top with positive height)
	T y;
	/// Width as a public property
	T w;
	/// Height as a public property
	T h;

	/// Default constructor, all zeros
	Rect()
	    : x(0), y(0), w(0), h(0) {}
	/// Constructs a rectangle from top-left point and size
	Rect(T xx, T yy, T ww, T hh)
	    : x(xx), y(yy), w(ww), h(hh) {}
	/// Constructs a rectangle from top-left point and size as two `Vector2`
	Rect(const Vector2<T> &point, const Vector2<T> &size)
	    : x(point.x), y(point.y), w(size.x), h(size.y) {}

	/// Creates a rectangle from center and size
	static Rect fromCenterSize(T xx, T yy, T ww, T hh);
	/// Creates a rectangle from center and size as two `Vector2`
	static Rect fromCenterSize(const Vector2<T> &center, const Vector2<T> &size);

	/// Creates a rectangle from minimum and maximum coordinates
	static Rect fromMinMax(T minX, T minY, T maxX, T maxY);
	/// Creates a rectangle from minimum and maximum coordinates as two `Vector2`
	static Rect fromMinMax(const Vector2<T> &min, const Vector2<T> &max);

	/// Calculates the center of the rectangle
	Vector2<T> center() const;
	/// Calculates the minimum coordinates of the rectangle
	Vector2<T> min() const;
	/// Calculates the maximum coordinates of the rectangle
	Vector2<T> max() const;

	/// Sets rectangle top-left point and size
	void set(T xx, T yy, T ww, T hh);
	/// Sets rectangle top-left point and size as two `Vector2`
	void set(const Vector2<T> &point, const Vector2<T> &size);
	/// Retains rectangle size but moves its center to another position
	void setCenter(float cx, float cy);
	/// Retains rectangle size but moves its center to another position with a `Vector2`
	void setCenter(const Vector2<T> &center);
	/// Retains rectangle center but changes its size
	void setSize(float ww, float hh);
	/// Retains rectangle center but changes its size with a `Vector2`
	void setSize(const Vector2<T> &size);

	/// Sets rectangle center and size
	void setCenterSize(T xx, T yy, T ww, T hh);
	/// Sets rectangle center and size as two `Vector2`
	void setCenterSize(const Vector2<T> &center, const Vector2<T> &size);

	/// Sets rectangle minimum and maximum coordinates
	void setMinMax(T minX, T minY, T maxX, T maxY);
	/// Sets rectangle minimum and maximum coordinates as two `Vector2`
	void setMinMax(const Vector2<T> &min, const Vector2<T> &max);

	/// Inverts rectangle size and moves (x, y) to a different angle
	void invertSize();

	/// \returns True if the point is inside this rectangle
	bool contains(T px, T py) const;
	/// \returns True if the point vector is inside this rectangle
	bool contains(const Vector2<T> &p) const;

	/// \returns True if the other rectangle is contained inside this one
	bool contains(const Rect<T> &rect) const;
	/// \returns True if this rect does overlap the other rectangle in any way
	bool overlaps(const Rect<T> &rect) const;

	/// Intersects this rectangle with the other rectangle
	void intersect(const Rect<T> &rect);

	/// Eqality operator
	bool operator==(const Rect &rect) const;
};

using Rectf = Rect<float>;
using Recti = Rect<int>;

template <class T>
inline Rect<T> Rect<T>::fromCenterSize(T xx, T yy, T ww, T hh)
{
	return Rect(xx - static_cast<T>(ww * 0.5f),
	            yy - static_cast<T>(hh * 0.5f),
	            ww, hh);
}

template <class T>
inline Rect<T> Rect<T>::fromCenterSize(const Vector2<T> &center, const Vector2<T> &size)
{
	return Rect(center.x - static_cast<T>(size.x * 0.5f),
	            center.y - static_cast<T>(size.y * 0.5f),
	            size.x, size.y);
}

template <class T>
inline Rect<T> Rect<T>::fromMinMax(T minX, T minY, T maxX, T maxY)
{
	return Rect(minX, minY, maxX - minX, maxY - minY);
}

template <class T>
inline Rect<T> Rect<T>::fromMinMax(const Vector2<T> &min, const Vector2<T> &max)
{
	return Rect(min.x, min.y, max.x - min.x, max.y - min.y);
}

template <class T>
inline Vector2<T> Rect<T>::center() const
{
	return Vector2<T>(x + static_cast<T>(w * 0.5f), y + static_cast<T>(h * 0.5f));
}

template <class T>
inline Vector2<T> Rect<T>::min() const
{
	return Vector2<T>((w > T(0)) ? x : x + w, (h > T(0)) ? y : y + h);
}

template <class T>
inline Vector2<T> Rect<T>::max() const
{
	return Vector2<T>((w > T(0)) ? x + w : x, (h > T(0)) ? y + h : y);
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
inline void Rect<T>::set(const Vector2<T> &point, const Vector2<T> &size)
{
	x = point.x;
	y = point.y;
	w = size.x;
	h = size.y;
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
inline void Rect<T>::setSize(float ww, float hh)
{
	w = ww;
	h = hh;
}

template <class T>
inline void Rect<T>::setSize(const Vector2<T> &size)
{
	w = size.x;
	h = size.y;
}

template <class T>
inline void Rect<T>::setCenterSize(T xx, T yy, T ww, T hh)
{
	x = xx - static_cast<T>(ww * 0.5f);
	y = yy - static_cast<T>(hh * 0.5f);
	w = ww;
	h = hh;
}

template <class T>
inline void Rect<T>::setCenterSize(const Vector2<T> &center, const Vector2<T> &size)
{
	x = center.x - static_cast<T>(size.x * 0.5f);
	y = center.y - static_cast<T>(size.y * 0.5f);
	w = size.x;
	h = size.y;
}

template <class T>
inline void Rect<T>::setMinMax(T minX, T minY, T maxX, T maxY)
{
	x = minX;
	y = minY;
	w = maxX - minX;
	h = maxY - minY;
}

template <class T>
inline void Rect<T>::setMinMax(const Vector2<T> &min, const Vector2<T> &max)
{
	x = min.x;
	y = min.y;
	w = max.x - min.x;
	h = max.y - min.y;
}

template <class T>
inline void Rect<T>::invertSize()
{
	x = x + w;
	y = y + h;
	w = -w;
	h = -h;
}

template <class T>
inline bool Rect<T>::contains(T px, T py) const
{
	const bool xAxis = (w > T(0)) ? (px >= x && px <= x + w) : (px <= x && px >= x + w);
	const bool yAxis = (h > T(0)) ? (py >= y && py <= y + h) : (py <= y && py >= y + h);
	return xAxis && yAxis;
}

template <class T>
inline bool Rect<T>::contains(const Vector2<T> &p) const
{
	return contains(p.x, p.y);
}

template <class T>
inline bool Rect<T>::contains(const Rect &rect) const
{
	const bool containsMin = contains(rect.min());
	const bool containsMax = contains(rect.max());
	return (containsMin && containsMax);
}

template <class T>
inline bool Rect<T>::overlaps(const Rect &rect) const
{
	const Vector2<T> rectMin = rect.min();
	const Vector2<T> rectMax = rect.max();
	const Vector2<T> thisMin = min();
	const Vector2<T> thisMax = max();

	const bool disjoint = (rectMax.x < thisMin.x || rectMin.x > thisMax.x ||
	                       rectMax.y < thisMin.y || rectMin.y > thisMax.y);
	return !disjoint;
}

template <class T>
inline void Rect<T>::intersect(const Rect &rect)
{
	const Vector2<T> rectMin = rect.min();
	const Vector2<T> rectMax = rect.max();

	Vector2<T> newMin = min();
	Vector2<T> newMax = max();
	if (rectMin.x > newMin.x)
		newMin.x = rectMin.x;
	if (rectMin.y > newMin.y)
		newMin.y = rectMin.y;
	if (rectMax.x < newMax.x)
		newMax.x = rectMax.x;
	if (rectMax.y < newMax.y)
		newMax.y = rectMax.y;

	if (w < T(0))
		nctl::swap(newMin.x, newMax.x);
	if (h < T(0))
		nctl::swap(newMin.y, newMax.y);

	setMinMax(newMin, newMax);
}

template <class T>
inline bool Rect<T>::operator==(const Rect &rect) const
{
	return (x == rect.x && y == rect.y &&
	        w == rect.w && h == rect.h);
}

}

#endif
